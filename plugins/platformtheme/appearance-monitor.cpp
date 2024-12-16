/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "plugins/platformtheme/appearance-monitor.h"
#include <kiran-session-daemon/appearance-i.h>
#include <QDBusConnection>
#include <QFont>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QApplication>
#include <QDBusServiceWatcher>
#include "plugins/platformtheme/appearance_proxy.h"
#include "plugins/platformtheme/display_proxy.h"
#include "appearance-monitor.h"

#define APPEARANCE_DBUS_NAME "com.kylinsec.Kiran.SessionDaemon.Appearance"
#define APPEARANCE_DBUS_OBJECT_PATH "/com/kylinsec/Kiran/SessionDaemon/Appearance"

#define DISPLAY_DBUS_NAME "com.kylinsec.Kiran.SessionDaemon.Display"
#define DISPLAY_DBUS_OBJECT_PATH "/com/kylinsec/Kiran/SessionDaemon/Display"

namespace Kiran
{
namespace Platformtheme
{
AppearanceMonitor::AppearanceMonitor(QObject *parent)
    : QObject(parent),
      m_appearanceServiceWatcher(nullptr),
      m_displayServiceWatcher(nullptr)
{

    m_polishCursorTimer.setInterval(500);
    m_polishCursorTimer.setSingleShot(true);
    connect(&m_polishCursorTimer, &QTimer::timeout, this, &AppearanceMonitor::handleCursorThemeChanged);

    m_appearanceIface = new AppearanceProxy(APPEARANCE_DBUS_NAME,
                                                 APPEARANCE_DBUS_OBJECT_PATH,
                                                 QDBusConnection::sessionBus(),
                                                 this);
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(APPEARANCE_DBUS_NAME))
    {
        loadAppearance();
    }
    else
    {
        m_appearanceServiceWatcher = new QDBusServiceWatcher(APPEARANCE_DBUS_NAME, QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForRegistration, this);
        connect(m_appearanceServiceWatcher, &QDBusServiceWatcher::serviceRegistered, this, &AppearanceMonitor::loadAppearance);
        qDebug() << "kiran session daemon appearance service isn't registered!";
    }

    connect(m_appearanceIface, &AppearanceProxy::FontChanged,
            this, &AppearanceMonitor::handleFontSettingChanged);
    connect(m_appearanceIface, &AppearanceProxy::ThemeChanged,
            this, &AppearanceMonitor::handleThemeSettingChanged);

    m_displayIface = new DisplayProxy("com.kylinsec.Kiran.SessionDaemon.Display",
                                      "/com/kylinsec/Kiran/SessionDaemon/Display",
                                      QDBusConnection::sessionBus(),
                                      this);
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.kylinsec.Kiran.SessionDaemon.Display"))
    {
        loadScalingFactor();
    }
    else
    {
        m_displayServiceWatcher = new QDBusServiceWatcher(DISPLAY_DBUS_NAME, QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForRegistration, this);
        connect(m_displayServiceWatcher, &QDBusServiceWatcher::serviceRegistered, this, &AppearanceMonitor::loadScalingFactor);

        qDebug() << "kiran session daemon display service isn't registered!";
    }

    connect(m_displayIface, &DisplayProxy::window_scaling_factorChanged,
            this, &AppearanceMonitor::handleWindowScaleFactorChanged);
    m_polishCursorTimer.setInterval(500);
    m_polishCursorTimer.setSingleShot(true);
}

AppearanceMonitor *AppearanceMonitor::instance()
{
    static QMutex mutex;
    static QScopedPointer<AppearanceMonitor> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new AppearanceMonitor);
        }
    }

    return pInst.data();
}

AppearanceMonitor::~AppearanceMonitor()
{
}

void AppearanceMonitor::handleFontSettingChanged(int type, const QString &fontValue)
{
    QString fontName;
    int fontSize;
    if (!parseFontValue(fontValue, fontName, fontSize))
    {
        return;
    }

    switch (type)
    {
    case APPEARANCE_FONT_TYPE_APPLICATION:
        m_appFontSize = fontSize;
        m_appFontName = fontName;
        emit appFontChanged(appFont());
        break;
    case APPEARANCE_FONT_TYPE_WINDOW_TITLE:
        m_titleBarFontSize = fontSize;
        m_titleBarFontName = fontName;
        emit titleBarFontChanged(titleBarFont());
        return;
    default:
        break;
    }
}

void AppearanceMonitor::handleWindowScaleFactorChanged(int scaleFactor)
{
    m_scaleFactor = scaleFactor;
    emit scaleFactorChanged(m_scaleFactor);
}

bool AppearanceMonitor::parseFontValue(const QString &font, QString &fontName, int &fontSize)
{
    int fontSizeIdx = font.lastIndexOf(' ');
    if (fontSizeIdx <= 0)
    {
        qDebug() << "Invalid font value format:" << font;
        return false;
    }

    // Extract font size substring
    QString fontSizeStr = font.mid(fontSizeIdx + 1);

    bool toIntOk = false;
    fontSize = fontSizeStr.toInt(&toIntOk);
    if (!toIntOk || fontSize <= 0)
    {
        qDebug() << "Invalid font size:" << fontSizeStr;
        return false;
    }

    // Extract font name substring
    fontName = font.left(fontSizeIdx);
    if (fontName.isEmpty())
    {
        qDebug() << "Invalid font name:" << fontName;
        return false;
    }

    QFontDatabase fontDatabase;
    if (!fontDatabase.hasFamily(fontName))
    {
        qDebug() << "Font not available in database:" << fontName;
        return false;
    }

    return true;
}

void AppearanceMonitor::loadAppearance()
{
    // application font
    QString tempFontName;
    int tempFontSize;
    QString fontValue = m_appearanceIface->GetFont(APPEARANCE_FONT_TYPE_APPLICATION);
    if (parseFontValue(fontValue, tempFontName, tempFontSize))
    {
        m_appFontName = tempFontName;
        m_appFontSize = tempFontSize;
        qDebug("application font: %s %d", m_appFontName.toStdString().c_str(), m_appFontSize);

        emit appFontChanged(appFont());
    }
    else
    {
        qWarning() << "appearance monitor: parse application font failed!";
    }

    // window titlebar font
    fontValue = m_appearanceIface->GetFont(APPEARANCE_FONT_TYPE_WINDOW_TITLE);
    if (parseFontValue(fontValue, tempFontName, tempFontSize))
    {
        m_titleBarFontName = tempFontName;
        m_titleBarFontSize = tempFontSize;
        qDebug("title bar font: %s %d", m_titleBarFontName.toStdString().c_str(), m_titleBarFontSize);

        emit titleBarFontChanged(titleBarFont());
    }
    else
    {
        qDebug() << "parse titlebar font failed!";
    }

    // icon theme
    auto themeReply = m_appearanceIface->GetTheme(APPEARANCE_THEME_TYPE_ICON);
    themeReply.waitForFinished();
    if (!themeReply.isError())
    {
        m_iconTheme = themeReply.value();
        qDebug("icon theme: %s", m_iconTheme.toStdString().c_str());

        emit iconThemeChanged(m_iconTheme);
    }
    else
    {
        qDebug() << "get  icon theme failed," << themeReply.error();
    }

    // gtk theme
    themeReply = m_appearanceIface->GetTheme(APPEARANCE_THEME_TYPE_GTK);
    themeReply.waitForFinished();
    if (!themeReply.isError())
    {
        QString gtkThemeName = themeReply.value();

        if (gtkThemeName.contains("dark", Qt::CaseInsensitive))
            m_gtkThemeName = "kiran-dark";
        else
            m_gtkThemeName = "kiran";

        qDebug("gtk theme: %s", m_gtkThemeName.toStdString().c_str());

        emit gtkThemeChanged(m_gtkThemeName);
    }
    else
    {
        qDebug() << "get gtk theme failed," << themeReply.error();
    }

    m_polishCursorTimer.start();
}

void AppearanceMonitor::loadScalingFactor()
{
    handleWindowScaleFactorChanged(m_displayIface->window_scaling_factor());
}

QFont AppearanceMonitor::appFont() const
{
    QFont font = QFont(QString());
    font.setFamily(m_appFontName);
    font.setPointSize(m_appFontSize);
    return font;
}

int AppearanceMonitor::scaleFactor() const
{
    return m_scaleFactor;
}

QFont AppearanceMonitor::titleBarFont() const
{
    QFont font = QFont(QString());
    font.setFamily(m_titleBarFontName);
    font.setPointSize(m_titleBarFontSize);
    return font;
}

QString AppearanceMonitor::iconTheme() const
{
    return m_iconTheme;
}

void AppearanceMonitor::handleThemeSettingChanged(int type, const QString &themeName)
{
    if (type == APPEARANCE_THEME_TYPE_ICON)
    {
        m_iconTheme = themeName;
        emit iconThemeChanged(m_iconTheme);
    }
    else if (type == APPEARANCE_THEME_TYPE_GTK)
    {
        QString gtkTheme;

        if (themeName.contains("dark", Qt::CaseInsensitive))
        {
            gtkTheme = "kiran-dark";
        }
        else
        {
            gtkTheme = "kiran";
        }

        if (gtkTheme != m_gtkThemeName)
        {
            qDebug() << "gtk theme changed:" << themeName;
            m_gtkThemeName = gtkTheme;
            emit gtkThemeChanged(m_gtkThemeName);
        }
    }
    else if (type == APPEARANCE_THEME_TYPE_CURSOR)
    {
        // 延迟通知,让QXcbCursor更新主题
        // 若未变化光标,qt5.15之前都需要合入修复补丁
        m_polishCursorTimer.start();
    }
}

void AppearanceMonitor::handleCursorThemeChanged()
{
    emit cursorThemeChanged();
}

QString AppearanceMonitor::gtkTheme() const
{
    return m_gtkThemeName;
}
}  // namespace Platformtheme
}  // namespace Kiran

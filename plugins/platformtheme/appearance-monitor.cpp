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

#include "appearance-monitor.h"
#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QGSettings>
#include <QMutex>
#include <QStyleFactory>
#include <QTimer>

namespace Kiran
{
namespace Platformtheme
{

// TODO：后面改为include "xsettings-i.h"
#define XSETTINGS_SCHEMA_ID "com.kylinsec.kiran.xsettings"
#define XSETTINGS_SCHEMA_GTK_FONT_NAME "gtkFontName"
#define XSETTINGS_SCHEMA_NET_ICON_THEME_NAME "netIconThemeName"
#define XSETTINGS_SCHEMA_NET_THEME_NAME "netThemeName"
#define XSETTINGS_SCHEMA_GTK_CURSOR_THEME_NAME "gtkCursorThemeName"
#define XSETTINGS_SCHEMA_WINDOW_SCALING_FACTOR "windowScalingFactor"

#define MARCO_SCHEMA_ID "org.mate.Marco.general"
#define MARCO_SCHAME_KEY_TITLEBAR_FONT "titlebarFont"

AppearanceMonitor::AppearanceMonitor(QObject *parent) : QObject(parent)
{
    m_polishCursorTimer = new QTimer(this);
    m_polishCursorTimer->setInterval(500);
    m_polishCursorTimer->setSingleShot(true);
    m_xsettingsSettings = new QGSettings(XSETTINGS_SCHEMA_ID, "", this);
    m_marcoSettings = new QGSettings(MARCO_SCHEMA_ID, "", this);

    loadAppearance();
    loadScalingFactor();

    connect(m_polishCursorTimer, &QTimer::timeout, this, &AppearanceMonitor::handleCursorThemeChanged);
    connect(m_xsettingsSettings, &QGSettings::changed, this, &AppearanceMonitor::processXSettingsSettingChanged);
    connect(m_marcoSettings, &QGSettings::changed, this, &AppearanceMonitor::processMarcoSettingChanged);
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

QFont AppearanceMonitor::appFont() const
{
    QFont font = QFont(QString());
    font.setFamily(m_appFontName);
    font.setPointSize(m_appFontSize);
    return font;
}

QFont AppearanceMonitor::titleBarFont() const
{
    QFont font = QFont(QString());
    font.setFamily(m_titleBarFontName);
    font.setPointSize(m_titleBarFontSize);
    return font;
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
    updateAppFont();
    updateWindowFont();
    updateIconTheme();
    updateWindowTheme();
    updateCursorTheme();
}

void AppearanceMonitor::loadScalingFactor()
{
    auto windowScaleFactor = m_xsettingsSettings->get(XSETTINGS_SCHEMA_WINDOW_SCALING_FACTOR).toInt();

    if (m_scaleFactor != windowScaleFactor)
    {
        m_scaleFactor = windowScaleFactor;
        emit scaleFactorChanged(m_scaleFactor);
    }
}

void AppearanceMonitor::updateAppFont()
{
    QString fontName;
    int fontSize = 10;

    auto fontValue = m_xsettingsSettings->get(XSETTINGS_SCHEMA_GTK_FONT_NAME).toString();
    if (!parseFontValue(fontValue, fontName, fontSize))
    {
        qWarning() << "Parse application font" << fontValue << "failed!";
        return;
    }

    qDebug() << "Application font is" << fontName << ", size is" << fontSize;

    if (m_appFontName != fontName || m_appFontSize != fontSize)
    {
        m_appFontName = fontName;
        m_appFontSize = fontSize;
        emit appFontChanged(appFont());
    }
}

void AppearanceMonitor::updateWindowFont()
{
    QString fontName;
    int fontSize = 11;

    // TODO：暂时先直接调用marco的gsettings，后面kiran-cc-daemon提供marco和kwin的兼容接口后再更换
    auto fontValue = m_marcoSettings->get(MARCO_SCHAME_KEY_TITLEBAR_FONT).toString();
    if (!parseFontValue(fontValue, fontName, fontSize))
    {
        qWarning() << "Parse titlebar font failed!";
    }

    qDebug() << "Titlebar font is" << fontName << ", size is" << fontSize;

    if (m_titleBarFontName != fontName || m_titleBarFontSize != fontSize)
    {
        m_titleBarFontName = fontName;
        m_titleBarFontSize = fontSize;
        emit titleBarFontChanged(titleBarFont());
    }
}

void AppearanceMonitor::updateIconTheme()
{
    auto iconTheme = m_xsettingsSettings->get(XSETTINGS_SCHEMA_NET_ICON_THEME_NAME).toString();
    qDebug() << "Icon theme is" << iconTheme;

    if (iconTheme != m_iconTheme)
    {
        m_iconTheme = iconTheme;
        emit iconThemeChanged(m_iconTheme);
    }
}

void AppearanceMonitor::updateWindowTheme()
{
    QString gtkThemeName = m_xsettingsSettings->get(XSETTINGS_SCHEMA_NET_THEME_NAME).toString();
    QString windowThemeName;
    if (gtkThemeName.contains("dark", Qt::CaseInsensitive))
        windowThemeName = "kiran-dark";
    else
        windowThemeName = "kiran";

    qDebug() << "Window theme is" << windowThemeName;

    if (windowThemeName != m_windowThemeName)
    {
        m_windowThemeName = windowThemeName;
        emit gtkThemeChanged(m_windowThemeName);
    }
}

void AppearanceMonitor::updateCursorTheme()
{
    // 延迟通知,让QXcbCursor更新主题
    // 若未变化光标,qt5.15之前都需要合入修复补丁
    m_polishCursorTimer->start();
}

void AppearanceMonitor::handleCursorThemeChanged()
{
    emit cursorThemeChanged();
}

void AppearanceMonitor::processXSettingsSettingChanged(const QString &key)
{
    qDebug() << "Xsettings gsettings key" << key << "is changed.";

    if (key == XSETTINGS_SCHEMA_GTK_FONT_NAME)
    {
        updateAppFont();
    }
    else if (key == XSETTINGS_SCHEMA_NET_ICON_THEME_NAME)
    {
        updateIconTheme();
    }
    else if (key == XSETTINGS_SCHEMA_NET_THEME_NAME)
    {
        updateWindowTheme();
    }
    else if (key == XSETTINGS_SCHEMA_GTK_CURSOR_THEME_NAME)
    {
        updateCursorTheme();
    }
    else if (key == XSETTINGS_SCHEMA_WINDOW_SCALING_FACTOR)
    {
        loadScalingFactor();
    }
}

void AppearanceMonitor::processMarcoSettingChanged(const QString &key)
{
    qDebug() << "Marco gsettings key" << key << "is changed.";

    if (key == MARCO_SCHAME_KEY_TITLEBAR_FONT)
    {
        updateWindowFont();
    }
}

}  // namespace Platformtheme
}  // namespace Kiran

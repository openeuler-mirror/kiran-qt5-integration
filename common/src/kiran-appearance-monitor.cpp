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

#include "kiran-appearance-monitor.h"
#include "kiran_appearance_proxy.h"
#include "kiran_display_proxy.h"
#include "logging-category.h"

#include <kiran-session-daemon/appearance-i.h>

#include <QFont>
#include <QFontDatabase>

KiranAppearanceMonitor::KiranAppearanceMonitor(QObject *parent)
    : QObject(parent)
{
    m_appearanceIface = new KiranAppearanceProxy("com.kylinsec.Kiran.SessionDaemon.Appearance",
                                                 "/com/kylinsec/Kiran/SessionDaemon/Appearance",
                                                 QDBusConnection::sessionBus(),
                                                 this);

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.kylinsec.Kiran.SessionDaemon.Appearance"))
    {
        //application font
        QString tempFontName;
        int tempFontSize;
        QString fontValue = m_appearanceIface->GetFont(APPEARANCE_FONT_TYPE_APPLICATION);
        if (parseFontValue(fontValue, tempFontName, tempFontSize))
        {
            m_appFontName = tempFontName;
            m_appFontSize = tempFontSize;
            qDebug(kiranPlatformThemeCommon,"application font: %s %d",m_appFontName.toStdString().c_str(),m_appFontSize);
        }
        else
        {
            qWarning(kiranPlatformThemeCommon) << "appearance monitor: parse application font failed!";
        }

        //window titlebar font
        fontValue = m_appearanceIface->GetFont(APPEARANCE_FONT_TYPE_WINDOW_TITLE);
        if(parseFontValue(fontValue,tempFontName,tempFontSize))
        {
            m_titleBarFontName = tempFontName;
            m_titleBarFontSize = tempFontSize;
            qDebug(kiranPlatformThemeCommon,"title bar font: %s %d",m_titleBarFontName.toStdString().c_str(),m_titleBarFontSize);
        }
        else
        {
            qDebug(kiranPlatformThemeCommon) << "parse titlebar font failed!";
        }

        //icon theme
        auto themeReply = m_appearanceIface->GetTheme(APPEARANCE_THEME_TYPE_ICON);
        themeReply.waitForFinished();
        if( !themeReply.isError() )
        {
            m_iconTheme = themeReply.value();
            qDebug(kiranPlatformThemeCommon,"icon theme: %s",m_iconTheme.toStdString().c_str());
        }
        else
        {
            qDebug(kiranPlatformThemeCommon) << "get  icon theme failed," << themeReply.error();
        }

        //gtk theme
        themeReply = m_appearanceIface->GetTheme(APPEARANCE_THEME_TYPE_GTK);
        themeReply.waitForFinished();
        if( !themeReply.isError() )
        {
            QString gtkThemeName = themeReply.value();

            if( gtkThemeName.contains("dark",Qt::CaseInsensitive) )
                m_gtkThemeName = "kiran-dark";
            else
                m_gtkThemeName = "kiran";

            qDebug(kiranPlatformThemeCommon,"gtk theme: %s",m_gtkThemeName.toStdString().c_str());
        }
        else
        {
            qDebug(kiranPlatformThemeCommon) << "get gtk theme failed," << themeReply.error();
        }
    }
    else
    {
        qDebug(kiranPlatformThemeCommon) << "kiran session daemon appearance service isn't registered!";
    }

    connect(m_appearanceIface, &KiranAppearanceProxy::FontChanged,
            this, &KiranAppearanceMonitor::handleFontSettingChanged);
    connect(m_appearanceIface,&KiranAppearanceProxy::ThemeChanged,
            this,&KiranAppearanceMonitor::handleThemeSettingChanged);


    m_displayIface = new KiranDisplayProxy("com.kylinsec.Kiran.SessionDaemon.Display",
                                           "/com/kylinsec/Kiran/SessionDaemon/Display",
                                           QDBusConnection::sessionBus(),
                                           this);
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.kylinsec.Kiran.SessionDaemon.Display"))
    {
        m_scaleFactor = m_displayIface->window_scaling_factor();
    }
    else
    {
        qDebug(kiranPlatformThemeCommon) << "kiran session daemon display service isn't registered!";
    }

    connect(m_displayIface, &KiranDisplayProxy::window_scaling_factorChanged,
            this, &KiranAppearanceMonitor::handleWindowScaleFactorChanged);
    m_polishCursorTimer.setInterval(500);
    m_polishCursorTimer.setSingleShot(true);

    connect(&m_polishCursorTimer,&QTimer::timeout,this,&KiranAppearanceMonitor::handleCursorThemeChanged);
}

KiranAppearanceMonitor *KiranAppearanceMonitor::instance()
{
    static QMutex mutex;
    static QScopedPointer<KiranAppearanceMonitor> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new KiranAppearanceMonitor);
        }
    }

    return pInst.data();
}

KiranAppearanceMonitor::~KiranAppearanceMonitor()
{

}

void KiranAppearanceMonitor::handleFontSettingChanged(int type, const QString &fontValue)
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

void KiranAppearanceMonitor::handleWindowScaleFactorChanged(int scaleFactor)
{
    m_scaleFactor = scaleFactor;
    emit scaleFactorChanged(m_scaleFactor);
}

bool KiranAppearanceMonitor::parseFontValue(const QString &font, QString &fontName, int &fontSize)
{
    int fontSizeIdx = font.lastIndexOf(' ');
    if (fontSizeIdx <= 0)
    {
        qDebug(kiranPlatformThemeCommon) << "Invalid font value format:" << font;
        return false;
    }

    // Extract font size substring
    QString fontSizeStr = font.mid(fontSizeIdx + 1);

    bool toIntOk = false;
    fontSize = fontSizeStr.toInt(&toIntOk);
    if (!toIntOk || fontSize <= 0)
    {
        qDebug(kiranPlatformThemeCommon) << "Invalid font size:" << fontSizeStr;
        return false;
    }

    // Extract font name substring
    fontName = font.left(fontSizeIdx);
    if (fontName.isEmpty())
    {
        qDebug(kiranPlatformThemeCommon) << "Invalid font name:" << fontName;
        return false;
    }

    QFontDatabase fontDatabase;
    if (!fontDatabase.hasFamily(fontName))
    {
        qDebug(kiranPlatformThemeCommon) << "Font not available in database:" << fontName;
        return false;
    }

    return true;
}

QFont KiranAppearanceMonitor::appFont() const
{
    QFont font = QFont(QString());
    font.setFamily(m_appFontName);
    font.setPointSize(m_appFontSize);
    return font;
}

int KiranAppearanceMonitor::scaleFactor() const
{
    return m_scaleFactor;
}

QFont KiranAppearanceMonitor::titleBarFont() const
{
    QFont font = QFont(QString());
    font.setFamily(m_titleBarFontName);
    font.setPointSize(m_titleBarFontSize);
    return font;
}

QString KiranAppearanceMonitor::iconTheme() const
{
    return m_iconTheme;
}

void KiranAppearanceMonitor::handleThemeSettingChanged(int type, const QString &themeName)
{
    if(type == APPEARANCE_THEME_TYPE_ICON)
    {
        m_iconTheme = themeName;
        emit iconThemeChanged(m_iconTheme);
    }
    else if(type == APPEARANCE_THEME_TYPE_GTK)
    {
        QString gtkTheme;

        if( themeName.contains("dark",Qt::CaseInsensitive) )
        {
            gtkTheme = "kiran-dark";
        }
        else
        {
            gtkTheme = "kiran";
        }

        if(gtkTheme!=m_gtkThemeName)
        {
            qDebug(kiranPlatformThemeCommon) << "gtk theme changed:" << themeName;
            m_gtkThemeName = gtkTheme;
            emit gtkThemeChanged(m_gtkThemeName);
        }
    }
    else if(type==APPEARANCE_THEME_TYPE_CURSOR)
    {
        // 延迟通知,让QXcbCursor更新主题
        // 若未变化光标,qt5.15之前都需要合入修复补丁
        m_polishCursorTimer.start();
    }
}

void KiranAppearanceMonitor::handleCursorThemeChanged()
{
    emit cursorThemeChanged();
}

QString KiranAppearanceMonitor::gtkTheme() const
{
    return m_gtkThemeName;
}

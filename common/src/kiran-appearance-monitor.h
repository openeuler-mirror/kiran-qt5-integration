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
#pragma once

#include <QObject>
#include <QFont>
#include <QTimer>

class KiranDisplayProxy;
class KiranAppearanceProxy;
class KiranAppearanceMonitor : public QObject
{
    Q_OBJECT
private:
    explicit KiranAppearanceMonitor(QObject* parent= nullptr);
public:
    static KiranAppearanceMonitor* instance();
    ~KiranAppearanceMonitor() override;

    QFont appFont() const;
    QFont titleBarFont() const;
    QString iconTheme() const;
    QString gtkTheme() const;
    int scaleFactor() const;

signals:
    void appFontChanged(QFont font);
    void titleBarFontChanged(QFont font);
    void scaleFactorChanged(int factor);
    void iconThemeChanged(QString iconTheme);
    void gtkThemeChanged(QString gtkTheme);
    void cursorThemeChanged();

private:
    static bool parseFontValue(const QString& font,QString& fontName,int& fontSize);

private slots:
    void handleFontSettingChanged(int type,const QString& fontValue);
    void handleWindowScaleFactorChanged(int scaleFactor);
    void handleThemeSettingChanged(int type,const QString& themeName);
    void handleCursorThemeChanged();

private:
    QString m_appFontName = "Noto Sans CJK";
    int m_appFontSize = 10;

    QString m_titleBarFontName = "Noto Sans CJK";
    int m_titleBarFontSize = 11;

    int m_scaleFactor = 0;

    QString m_iconTheme = "hicolor";
    QString m_gtkThemeName = "kiran";

    QTimer m_polishCursorTimer;
    KiranDisplayProxy* m_displayIface;
    KiranAppearanceProxy* m_appearanceIface;
};
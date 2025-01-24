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

#include <QFont>
#include <QObject>

class QGSettings;

namespace Kiran
{
namespace Platformtheme
{
class AppearanceMonitor : public QObject
{
    Q_OBJECT

private:
    explicit AppearanceMonitor(QObject* parent = nullptr);

public:
    static AppearanceMonitor* instance();
    ~AppearanceMonitor() override;

    QFont appFont() const;
    QFont titleBarFont() const;
    QString iconTheme() const { return m_iconTheme; };
    QString gtkTheme() const { return m_windowThemeName; };
    int scaleFactor() const { return m_scaleFactor; };

signals:
    void appFontChanged(QFont font);
    void titleBarFontChanged(QFont font);
    void scaleFactorChanged(int factor);
    void iconThemeChanged(QString iconTheme);
    void gtkThemeChanged(QString gtkTheme);
    void cursorThemeChanged();

private:
    static bool parseFontValue(const QString& font, QString& fontName, int& fontSize);
    void loadAppearance();
    void loadScalingFactor();

    void updateAppFont();
    void updateWindowFont();
    void updateIconTheme();
    void updateWindowTheme();
    void updateCursorTheme();

private slots:
    void handleCursorThemeChanged();
    void processXSettingsSettingChanged(const QString& key);
    void processMarcoSettingChanged(const QString& key);

private:
    QString m_appFontName = "Noto Sans CJK";
    int m_appFontSize = 10;

    QString m_titleBarFontName = "Noto Sans CJK";
    int m_titleBarFontSize = 11;

    int m_scaleFactor = 0;

    QString m_iconTheme = "hicolor";
    QString m_windowThemeName = "kiran";

    QTimer* m_polishCursorTimer;

    QGSettings* m_xsettingsSettings;
    QGSettings* m_marcoSettings;
};
}  // namespace Platformtheme
}  // namespace Kiran
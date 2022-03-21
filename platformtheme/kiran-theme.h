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
#ifndef KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_H_
#define KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_H_
#include <QObject>
#include <QScreen>
#include <QtThemeSupport/private/qgenericunixthemes_p.h>

class KiranAppearanceMonitor;
class KiranTheme : public QObject,public QGenericUnixTheme
{
    Q_OBJECT
public:
    explicit KiranTheme(const QStringList& paramList);
    ~KiranTheme() override;

    //是否使用系统本地提示框
    bool usePlatformNativeDialog(DialogType type) const override;
    //创建本地提示框帮助类
    QPlatformDialogHelper* createPlatformDialogHelper(DialogType type) const override;
    QIconEngine* createIconEngine(const QString& iconName) const override;
    QPixmap standardPixmap(StandardPixmap sp, const QSizeF& size) const override;
    QIcon fileIcon(const QFileInfo& fileInfo, QPlatformTheme::IconOptions iconOptions) const override;

    QVariant themeHint(ThemeHint hint) const override;

    const QPalette* palette(Palette type) const override;
    const QFont* font(Font type) const override;

private:
    void init();
    static bool enableRealTimeScaling();

private slots:
    void handleAppFontChanged();
    void handleTitleBarFontChanged();
    void handleIconThemeChanged();
    void handleScaleFactorChanged(int factor);
    void handleScreenAdded(QScreen* screen);
    void handleGtkThemeChanged(const QString& gtkTheme);

private:
    KiranAppearanceMonitor* m_settingsMonitor;
    int m_scaleFactor;
    QFont m_systemFont;
    QFont m_titleBarFont;
};

#endif  //KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_H_

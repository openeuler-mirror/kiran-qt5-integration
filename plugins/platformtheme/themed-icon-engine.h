/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
 * svg-icon-swithcer is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     youzhengcai <youzhengcai@kylinsec.com.cn>
 */
#pragma once

#include <QIconEngine>
#include <QList>
#include <QMap>
#include <QPixmap>
#include <QPixmapCache>
#include "lib/theme/palette.h"
#include "plugins/platformtheme/appearance-monitor.h"

namespace Kiran
{
namespace Platformtheme
{
class ThemedIconEngine : public QIconEngine
{
public:
    enum SvgConvertType
    {
        SVG_CONVERT_COMPOSITION,  /** < 合成模式 */
        SVG_CONVERT_INVERT_PIXELS /** < 像素反转 */
    };

public:
    explicit ThemedIconEngine(const QString &themeSvgIconName, const SvgConvertType type);
    ~ThemedIconEngine() override;

    static bool isValid(const QString &themeSvgIconName);
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine *clone() const override;

private:
    QByteArray pixmapCacheKey(const QSize &size, QIcon::Mode mode, QIcon::State state);
    static QStringList getScalableSvgIconFromTheme(const QString &iconName);
    void ensureLoaded();
    void virtual_hook(int id, void *data) override;

private slots:
    void changeSvgIconColor();

private:
    QColor m_svgColor = Kiran::Theme::Palette::getDefault()->getBaseColors().baseForeground;
    QString m_iconName;
    QString m_svgIconPath;

    QMap<QString, QPixmap> m_pixmapCache;

    uint m_iconLoaderThemeKey = 0;
    Platformtheme::AppearanceMonitor *m_settingsMonitor;

    SvgConvertType m_svgConvertType;
};
}  // namespace Platformtheme
}  // namespace Kiran

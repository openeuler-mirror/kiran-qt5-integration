/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd. 
 * kiranwidgets-qt5 is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     tangjie02 <tangjie02@kylinsec.com.cn>
 */

#pragma once

#include <QColor>
#include "lib/theme/palette.h"

namespace Kiran
{
namespace Theme
{
class PalettePrivate : public QObject
{
    Q_OBJECT

public:
    explicit PalettePrivate(Palette* qptr);
    ~PalettePrivate() override = default;;

    void setBaseColors(const Palette::BaseColors& baseColors);
    void setColorFactor(const Palette::ColorFactor& colorFactor);
    void setColorGroup(Palette::ColorGroup colorGroup,
                       const QColor& window,
                       const QColor& text,
                       const QColor& widget,
                       const QColor& icon,
                       const QColor& scroll,
                       const QColor& border);

    QColor getColor(Palette::ColorGroup colorGroup, Palette::ColorRole colorRole);

private:
    void calcWidgetColors();
    // 颜色混合
    QColor mixColor(const QColor& color1, const QColor& color2, double factor);
    // 计算禁用颜色
    QColor disabledColor(const QColor& color);
    // 打印颜色信息
    void dumpColors();

private:
    Palette* q_ptr;
    Q_DECLARE_PUBLIC(Palette)

private:
    Palette::BaseColors m_baseColors;
    Palette::ColorFactor m_colorFactors{};
    // 其他控件状态都通过正常状态的颜色混合计算，这里保持计算系数
    // static double m_colorFactor[Palette::WIDGET_PSEUDO_LAST][Palette::WIDGET_AREA_LAST];

    // 基于基础颜色组，生成每个控件类型在不同伪状态下的不同部位的颜色
    // QColor m_widgetColors[Palette::WIDGET_TYPE_LAST][Palette::WIDGET_PSEUDO_LAST][Palette::WIDGET_AREA_LAST];
    QColor m_colors[Palette::N_COLOR_GROUPS][Palette::N_COLOR_ROLES];
};

}  // namespace Theme

}  // namespace Kiran

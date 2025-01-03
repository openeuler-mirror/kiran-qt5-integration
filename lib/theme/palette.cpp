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

#include "lib/theme/palette.h"
#include <QColor>
#include <QDebug>
#include <QMetaEnum>
#include <QStyleOption>
#include "lib/theme/palette-private.h"

namespace Kiran
{
namespace Theme
{
// 定义内置的基础颜色，只包含浅色和深色
Palette::BaseColors g_lightBaseColors = {.baseBackground = QColor(232, 232, 232),        // #E8E8E8
                                         .baseForeground = QColor(34, 34, 34),           // #222222
                                         .widgetBackground = QColor(239, 239, 239),      // #EFEFEF
                                         .widgetBorder = QColor(204, 204, 204),          // #CCCCCC
                                         .widgetSelection = QColor(0, 162, 255),         // #00A2FF
                                         .widgetMain = QColor(46, 179, 255),             // #2EB3FF
                                         .widgetWarning = QColor(250, 73, 73),           // #FA4949
                                         .containerBackground = QColor(255, 255, 255)};  // #FFFFFF

Palette::BaseColors g_darkBaseColors = {.baseBackground = QColor(34, 34, 34),        // #222222
                                        .baseForeground = QColor(255, 255, 255),     // #FFFFFF
                                        .widgetBackground = QColor(57, 57, 57),      // #393939
                                        .widgetBorder = QColor(69, 69, 69),          // #454545
                                        .widgetSelection = QColor(0, 162, 255),      // #00A2FF
                                        .widgetMain = QColor(46, 179, 255),          // #2EB3FF
                                        .widgetWarning = QColor(250, 73, 73),        // #FA4949
                                        .containerBackground = QColor(45, 45, 45)};  // #2d2d2d
// 颜色计算因子
Palette::ColorFactor g_colorFactor = {.widgetHover = 0.9,
                                      .widgetSunken = 0.8,
                                      .iconHover = 0.3,
                                      .iconSunken = 0.4,
                                      .disabledAlpha = 0.65,
                                      .disabledLightness = 0.1};

PalettePrivate::PalettePrivate(Palette* qptr) : q_ptr(qptr)
{
    // 默认为深色主题
    this->m_baseColors = g_darkBaseColors;
    this->m_colorFactors = g_colorFactor;
    this->calcWidgetColors();
}

void PalettePrivate::setBaseColors(const Palette::BaseColors& baseColors)
{
    this->m_baseColors = baseColors;
    this->calcWidgetColors();
}

void PalettePrivate::setColorFactor(const Palette::ColorFactor& colorFactor)
{
    this->m_colorFactors = colorFactor;
    this->calcWidgetColors();
}

void PalettePrivate::setColorGroup(Palette::ColorGroup colorGroup,
                                   const QColor& window,
                                   const QColor& text,
                                   const QColor& widget,
                                   const QColor& icon,
                                   const QColor& scroll,
                                   const QColor& border)
{
    this->m_colors[colorGroup][Palette::WINDOW] = window;
    this->m_colors[colorGroup][Palette::TEXT] = text;
    this->m_colors[colorGroup][Palette::WIDGET] = widget;
    this->m_colors[colorGroup][Palette::ICON] = icon;
    this->m_colors[colorGroup][Palette::SCROLL] = scroll;
    this->m_colors[colorGroup][Palette::BORDER] = border;
}

QColor PalettePrivate::getColor(Palette::ColorGroup colorGroup, Palette::ColorRole colorRole)
{
    if (colorGroup > Palette::N_COLOR_GROUPS || colorRole > Palette::N_COLOR_ROLES)
    {
        return QColor();
    }

    return this->m_colors[colorGroup][colorRole];
}

void PalettePrivate::calcWidgetColors()
{
    this->setColorGroup(Palette::ACTIVE,
                        this->m_baseColors.baseBackground,
                        this->m_baseColors.baseForeground,
                        this->m_baseColors.widgetBackground,
                        this->m_baseColors.baseForeground,
                        this->m_baseColors.widgetBorder,
                        this->m_baseColors.widgetBorder);

    this->setColorGroup(Palette::DISABLED,
                        this->m_baseColors.baseBackground,
                        this->disabledColor(this->m_baseColors.baseForeground),
                        this->disabledColor(this->m_baseColors.widgetBackground),
                        this->disabledColor(this->m_baseColors.baseForeground),
                        this->disabledColor(this->m_baseColors.widgetBorder),
                        this->disabledColor(this->m_baseColors.widgetBorder));

    // 未激活状态暂时跟激活状态保持相同
    this->setColorGroup(Palette::INACTIVE,
                        this->m_baseColors.baseBackground,
                        this->m_baseColors.baseForeground,
                        this->m_baseColors.widgetBackground,
                        this->m_baseColors.baseForeground,
                        this->m_baseColors.widgetBorder,
                        this->m_baseColors.widgetBorder);

    this->setColorGroup(Palette::MOUSE_OVER,
                        this->m_baseColors.baseBackground,
                        this->m_baseColors.baseForeground,
                        this->mixColor(this->m_baseColors.widgetBackground,
                                       this->m_baseColors.baseForeground,
                                       this->m_colorFactors.widgetHover),
                        this->mixColor(this->m_baseColors.widgetBackground,
                                       this->m_baseColors.baseForeground,
                                       this->m_colorFactors.iconHover),
                        this->mixColor(this->m_baseColors.widgetBorder,
                                       this->m_baseColors.baseForeground,
                                       this->m_colorFactors.widgetHover),
                        this->m_baseColors.widgetBorder);

    this->setColorGroup(Palette::SUNKEN,
                        this->m_baseColors.baseBackground,
                        this->m_baseColors.baseForeground,
                        this->mixColor(this->m_baseColors.widgetBackground,
                                       this->m_baseColors.baseForeground,
                                       this->m_colorFactors.widgetSunken),
                        this->mixColor(this->m_baseColors.widgetBackground,
                                       this->m_baseColors.baseForeground,
                                       this->m_colorFactors.iconSunken),
                        this->m_baseColors.widgetSelection,
                        this->m_baseColors.widgetBorder);

    this->setColorGroup(Palette::SELECTED,
                        this->m_baseColors.baseBackground,
                        this->m_baseColors.baseForeground,
                        this->m_baseColors.widgetSelection,
                        this->m_baseColors.baseForeground,
                        this->m_baseColors.widgetBorder,
                        this->m_baseColors.widgetBorder);

    this->dumpColors();
}

QColor PalettePrivate::mixColor(const QColor& color1, const QColor& color2, double factor)
{
    return QColor(int(color1.red() * factor + color2.red() * (1 - factor)),
                  int(color1.green() * factor + color2.green() * (1 - factor)),
                  int(color1.blue() * factor + color2.blue() * (1 - factor)));
}

QColor PalettePrivate::disabledColor(const QColor& color)
{
    // 沿用GTK主题的禁用色计算算法
    auto disabledColor = color;

    if ((this->m_baseColors.baseBackground.lightness() > this->m_baseColors.baseForeground.lightness() &&
         color.lightness() < this->m_baseColors.baseBackground.lightness()) ||
        (this->m_baseColors.baseBackground.lightness() <= this->m_baseColors.baseForeground.lightness() &&
         color.lightness() > this->m_baseColors.baseBackground.lightness()))
    {
        disabledColor.setAlphaF(this->m_colorFactors.disabledAlpha);
    }

    disabledColor = disabledColor.lighter(100 * (1 - this->m_colorFactors.disabledLightness));
    return disabledColor;
}

void PalettePrivate::dumpColors()
{
    for (uint32_t colorGroup = 0; colorGroup < Palette::N_COLOR_GROUPS; ++colorGroup)
    {
        for (uint32_t colorRole = 0; colorRole < Palette::N_COLOR_ROLES; ++colorRole)
        {
            qDebug().nospace() << "The color of "
                               << QMetaEnum::fromType<Palette::ColorGroup>().valueToKey(colorGroup)
                               << "+"
                               << QMetaEnum::fromType<Palette::ColorRole>().valueToKey(colorRole)
                               << " is "
                               << qSetFieldWidth(2)
                               << qSetPadChar('0')
                               << "0x"
                               << hex
                               << this->m_colors[colorGroup][colorRole].red()
                               << this->m_colors[colorGroup][colorRole].green()
                               << this->m_colors[colorGroup][colorRole].blue()
                               << ", Alpha is "
                               << qSetFieldWidth(0)
                               << this->m_colors[colorGroup][colorRole].alphaF();
        }
    }
}

Palette::Palette(QObject* parent) : QObject(parent),
                                    d_ptr(new PalettePrivate(this))
{
}

Palette* Palette::m_instance = nullptr;
Palette* Palette::getDefault()
{
    if (!Palette::m_instance)
    {
        Palette::m_instance = new Palette();
    }
    return Palette::m_instance;
}

void Palette::setBaseColors(const BaseColors& baseColors)
{
    Q_D(Palette);

    d->setBaseColors(baseColors);
    emit baseColorsChanged();
}

const Palette::BaseColors& Palette::getBaseColors()
{
    Q_D(Palette);

    return d->m_baseColors;
}

QColor Palette::getColor(QStyle::State styleState, Palette::ColorRole colorRole)
{
    Q_D(Palette);

    auto colorGroups = Kiran::Theme::Palette::styleState2ColorGroups(styleState);

    // 按照列表中的优先级顺序查找对应颜色
    QVector<Palette::ColorGroup> colorGroupsOrder = {Palette::DISABLED,
                                                     Palette::SELECTED,
                                                     Palette::SUNKEN,
                                                     Palette::MOUSE_OVER,
                                                     Palette::ACTIVE,
                                                     Palette::INACTIVE};

    for (auto colorGroup : colorGroupsOrder)
    {
        if ((1 << colorGroup) & colorGroups)
        {
            return d->m_colors[colorGroup][colorRole];
        }
    }

    return d->m_colors[Palette::ACTIVE][colorRole];
}

QColor Palette::getColor(Palette::ColorGroup colorGroup, Palette::ColorRole colorRole)
{
    Q_D(Palette);

    return d->getColor(colorGroup, colorRole);
}

void Palette::polish(QPalette& qPalette)
{
    using ColorRoleInfo = QMap<QPalette::ColorRole, Palette::ColorRole>;
    ColorRoleInfo colorRoleInfo = {{QPalette::ColorRole::Window, Palette::WINDOW},
                                   {QPalette::ColorRole::Background, Palette::WINDOW},
                                   {QPalette::ColorRole::WindowText, Palette::TEXT},
                                   {QPalette::ColorRole::Foreground, Palette::TEXT},
                                   {QPalette::ColorRole::Base, Palette::WINDOW},
                                   {QPalette::ColorRole::AlternateBase, Palette::WINDOW},
                                   {QPalette::ColorRole::ToolTipBase, Palette::WINDOW},
                                   {QPalette::ColorRole::ToolTipText, Palette::TEXT},
                                   {QPalette::ColorRole::Text, Palette::TEXT},
                                   {QPalette::ColorRole::Button, Palette::WIDGET},
                                   {QPalette::ColorRole::ButtonText, Palette::TEXT}};

    using ColorGroupInfo = QMap<QPalette::ColorGroup, Palette::ColorGroup>;
    ColorGroupInfo colorGroupInfo = {{QPalette::ColorGroup::Active, Palette::ACTIVE},
                                     {QPalette::ColorGroup::Disabled, Palette::DISABLED},
                                     {QPalette::ColorGroup::Inactive, Palette::INACTIVE},
                                     {QPalette::ColorGroup::Normal, Palette::NORMAL}};

    for (auto colorRoleInter = colorRoleInfo.begin(); colorRoleInter != colorRoleInfo.end(); ++colorRoleInter)
    {
        for (auto colorGroupInter = colorGroupInfo.begin(); colorGroupInter != colorGroupInfo.end(); ++colorGroupInter)
        {
            qPalette.setColor(colorGroupInter.key(),
                              colorRoleInter.key(),
                              this->getColor(colorGroupInter.value(), colorRoleInter.value()));
        }
    }
}

uint32_t Palette::styleState2ColorGroups(QStyle::State styleState)
{
    uint32_t pseudos = 0;

    if (styleState & QStyle::State_Enabled)
    {
        pseudos |= (1 << Palette::ACTIVE);
    }
    else
    {
        pseudos |= (1 << Palette::DISABLED);
    }

    if (styleState & QStyle::State_Sunken)
    {
        pseudos |= (1 << Palette::SUNKEN);
    }

    if (styleState & QStyle::State_MouseOver)
    {
        pseudos |= (1 << Palette::MOUSE_OVER);
    }

    if ((styleState & QStyle::State_On) ||
        (styleState & QStyle::State_Selected))
    {
        pseudos |= (1 << Palette::SELECTED);
    }

    return pseudos;
}

}  // namespace Theme
}  // namespace Kiran

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

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SCROLL_BAR_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SCROLL_BAR_HELPER_H_

#include <QRect>
#include <QStyle>

class QStyle;
class QStyleOptionComplex;
class QPainter;
class QWidget;
namespace Kiran
{
namespace Style
{
class SchemeLoader;

bool scrollBarSubControlRect(const QStyle *style, const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget, QRect &controlRect);

bool drawCCScrollBar(const QStyle *style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget);

bool drawControlScrollBarGroove(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, Kiran::Style::SchemeLoader *scheme);
bool drawControlScrollBarSlider(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget);
}
}

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SCROLL_BAR_HELPER_H_

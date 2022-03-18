//
// Created by lxh on 2021/12/24.
//

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

bool drawCCScrollBar(const QStyle* style, const QStyleOptionComplex* option, QPainter *painter, const QWidget *widget,SchemeLoader* scheme);

bool drawControlScrollBarGroove(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, Kiran::Style::SchemeLoader *scheme);
bool drawControlScrollBarSlider(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, Kiran::Style::SchemeLoader *scheme);
}
}

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SCROLL_BAR_HELPER_H_

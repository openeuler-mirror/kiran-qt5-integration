//
// Created by lxh on 2021/12/29.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_GROUP_BOX_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_GROUP_BOX_HELPER_H_

#include <QStyle>

class QStyle;
class QWidget;
class QStyleOption;
class QStyleOptionComplex;

///绘制QGroupBox

namespace Kiran
{
namespace Style
{
class SchemeLoader;
QSize groupBoxSizeFromContents(const QStyle* style,const QStyleOption* option,const QSize& contentSize,const QWidget* widget);
bool groupBoxSubControlRect(const QStyle* style, const QStyleOptionComplex* opt, QStyle::SubControl sc, const QWidget* widget, QRect& controlRect);
bool drawCCGroupBox(const QStyle* style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget,SchemeLoader* scheme);
}
}

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_GROUP_BOX_HELPER_H_

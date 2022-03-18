//
// Created by lxh on 2021/12/27.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SPIN_BOX_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SPIN_BOX_HELPER_H_

#include <QStyle>

class QStyle;
class QStyleOptionComplex;
class QStyleOption;
class QWidget;
class QPainter;
class QSize;
class QRect;
namespace Kiran
{
namespace Style
{
class SchemeLoader;
QSize spinBoxSizeFromContents(const QStyle *style, const QStyleOption *option, const QSize &contentSize, const QWidget *widget);
bool spinBoxSubControlRect(const QStyle* style, const QStyleOptionComplex* option, QStyle::SubControl subControl, const QWidget* widget, QRect& controlRect);
bool drawCCSpinBox(const QStyle* style, const QStyleOptionComplex* option, QPainter *painter, const QWidget *widget,SchemeLoader* scheme);

}
}
#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SPIN_BOX_HELPER_H_

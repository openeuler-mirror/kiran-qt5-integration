//
// Created by lxh on 2022/1/11.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SLIDER_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SLIDER_HELPER_H_

#include <QSize>
#include <QRect>
#include <QStyle>

class QStyleOption;
class QWidget;
class QStyleOptionComplex;
class QWidget;
class QPainter;
namespace Kiran
{
namespace Style
{
class SchemeLoader;
QSize sliderSizeFromContents(const QStyle *style,const QStyleOption *option,const QSize &contentSize,const QWidget *widget);
bool drawCCSlider(const QStyle *style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
QRect sliderElementRect(const QStyle *style,QStyle::SubElement subElement,const QStyleOption *option,const QWidget *widget);
bool sliderSubControlRect(const QStyle *style, const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *widget, QRect &controlRect);
}
}

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_SLIDER_HELPER_H_

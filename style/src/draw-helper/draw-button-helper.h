//
// Created by lxh on 2021/12/20.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_BUTTON_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_BUTTON_HELPER_H_

#include <QStyle>

class QStyleOption;
class QPainter;
class QWidget;
class QStyleOptionComplex;
class QStyle;
namespace Kiran
{
namespace Style
{
class SchemeLoader;

//sizeFromContents
QSize pushButtonSizeFromContents(const QStyle* style,const QStyleOption* option,const QSize& contentSize,const QWidget* widget);
QSize toolButtonSizeFromContents(const QStyle* style,const QStyleOption* option,const QSize& contentSize,const QWidget* widget);

//subControlRect
bool toolButtonSubControlRect(const QStyle* style, const QStyleOptionComplex* opt, QStyle::SubControl sc, const QWidget* widget, QRect& controlRect);

//drawPrimitive
bool drawPEPanelButtonCommand(const QStyle* style,const QStyleOption *option, QPainter *painter, const QWidget *widget,SchemeLoader* scheme);
bool drawPEPanelButtonTool(const QStyle* style,const QStyleOption *option, QPainter *painter, const QWidget *widget,SchemeLoader* scheme);

//drawControl
bool drawControlToolButtonLabel(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, Kiran::Style::SchemeLoader *scheme);

//drawComplexControl
bool drawCCToolButton(const QStyle* style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget,SchemeLoader* scheme);
}  // namespace Style
}  // namespace Kiran

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_BUTTON_HELPER_H_

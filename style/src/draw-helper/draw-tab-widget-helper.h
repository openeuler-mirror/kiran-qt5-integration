//
// Created by lxh on 2021/12/20.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_TAB_WIDGET_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_TAB_WIDGET_HELPER_H_

#include <QSize>
#include <QStyle>

class QStyle;
class QStyleOption;
class QStyleOptionComplex;
class QPainter;
class QWidget;
class QRect;
namespace Kiran
{
namespace Style
{
class SchemeLoader;

QSize tabBarTabSizeFromContents(const QStyle *style, const QStyleOption *option, const QSize &contentSize, const QWidget *widget);
QRect tabBarSubElementRect(const QStyle *style, QStyle::SubElement subElement, const QStyleOption *option, const QWidget *widget);

bool drawPEFrameTabWidget(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
bool drawPEFrameTabBarBase(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);

bool drawControlTabBarTabLabel(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
bool drawControlTabBarTabShape(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
}  // namespace Style
}  // namespace Kiran

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_TAB_WIDGET_HELPER_H_

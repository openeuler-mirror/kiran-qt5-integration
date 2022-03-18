//
// Created by lxh on 2022/3/17.
//

#ifndef KIRAN_QT5_INTEGRATION_STYLE_SRC_DRAW_HELPER_DRAW_MENU_HELPER_H_
#define KIRAN_QT5_INTEGRATION_STYLE_SRC_DRAW_HELPER_DRAW_MENU_HELPER_H_

class QStyle;
class QStyleOption;
class QPainter;
class QWidget;

namespace Kiran
{
namespace Style
{
class SchemeLoader;
bool drawControlMenuBarItem(const QStyle *style,const QStyleOption *option,QPainter *painter,const QWidget *widget,SchemeLoader *scheme);
bool drawControlMenuBarEmptyArea(const QStyle *style,const QStyleOption *option,QPainter *painter,const QWidget *widget,SchemeLoader *scheme);
}
}
#endif  //KIRAN_QT5_INTEGRATION_STYLE_SRC_DRAW_HELPER_DRAW_MENU_HELPER_H_

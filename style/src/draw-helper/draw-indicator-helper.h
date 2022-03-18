//
// Created by lxh on 2021/12/20.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_INDICATOR_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_INDICATOR_HELPER_H_

class QStyle;
class QStyleOption;
class QPainter;
class QWidget;

namespace Kiran
{
namespace Style
{
class SchemeLoader;

bool drawPEIndicatorButtonDropDown(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);

bool drawPEIndicatorArrowUp(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
bool drawPEIndicatorArrowDown(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
bool drawPEIndicatorArrowLeft(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
bool drawPEIndicatorArrowRight(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);

bool drawPEIndicatorRadioButton(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
bool drawPEIndicatorCheckBox(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);

bool drawPEIndicatorBranch(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);

bool drawPEIndicatorToolBarSeparator(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
bool drawPEIndicatorToolBarHandle(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme);
}  // namespace Style
}  // namespace Kiran

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_INDICATOR_HELPER_H_

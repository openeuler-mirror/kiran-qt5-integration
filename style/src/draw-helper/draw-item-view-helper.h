//
// Created by lxh on 2021/12/23.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_ITEM_VIEW_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_ITEM_VIEW_HELPER_H_

class QStyle;
class QStyleOption;
class QPainter;
class QWidget;
namespace Kiran
{
namespace Style
{
class SchemeLoader;
bool drawControlHeaderSection(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget, SchemeLoader* scheme);
bool drawControlHeaderLabel(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget, SchemeLoader* scheme);
bool drawControlHeaderEmptyArea(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget, SchemeLoader* scheme);
}
}

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_ITEM_VIEW_HELPER_H_

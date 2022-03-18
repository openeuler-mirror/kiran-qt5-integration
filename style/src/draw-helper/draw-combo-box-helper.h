//
// Created by lxh on 2021/12/20.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_COMBO_BOX_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_COMBO_BOX_HELPER_H_

#include <QStyle>

class QStyleOptionComplex;
class QStyleOption;
class QWidget;
class QPainter;

namespace Kiran
{
namespace Style
{
class SchemeLoader;

QSize comboBoxSizeFromContents(const QStyle* style, const QStyleOption* option, const QSize& contentSize, const QWidget* widget);
bool comboBoxSubControlRect(const QStyle* style, const QStyleOptionComplex* opt, QStyle::SubControl sc, const QWidget* widget, QRect& controlRect);
bool drawCCComboBox(const QStyle* style, const QStyleOptionComplex* option, QPainter *painter, const QWidget *widget,SchemeLoader* scheme);
bool drawControlComboBoxLabel(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget, SchemeLoader* scheme);
}
}

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_COMBO_BOX_HELPER_H_

/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */
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

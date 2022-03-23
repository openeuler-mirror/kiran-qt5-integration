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
bool drawPEPanelButtonCommand(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget);
bool drawPEPanelButtonTool(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget);

//drawControl
bool drawControlToolButtonLabel(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget);

//drawComplexControl
bool drawCCToolButton(const QStyle* style, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget);
}  // namespace Style
}  // namespace Kiran

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_BUTTON_HELPER_H_

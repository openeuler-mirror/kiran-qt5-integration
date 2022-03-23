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

bool drawPEFrameTabWidget(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget);
bool drawPEFrameTabBarBase(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget);

bool drawControlTabBarTabLabel(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget);
bool drawControlTabBarTabShape(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget);
}  // namespace Style
}  // namespace Kiran

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_TAB_WIDGET_HELPER_H_

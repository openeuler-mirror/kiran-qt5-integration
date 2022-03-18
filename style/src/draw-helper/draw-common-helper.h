//
// Created by lxh on 2021/12/20.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_COMMON_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_COMMON_HELPER_H_

#include "define.h"

#include <QColor>
#include <QPainterPath>
#include <QRect>
#include <QRectF>

class QStyle;
class QStyleOption;
class QStyleOptionToolButton;

namespace Kiran
{
namespace Style
{
class SchemeLoader;

bool drawPEFrame(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget, SchemeLoader* scheme);
bool drawPEFrameFocusRect(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget, SchemeLoader* scheme);
bool drawPEFrameGroupBox(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget, SchemeLoader* scheme);

bool drawControlShapedFrame(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget, SchemeLoader* scheme);

QSize defaultSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget);
}  // namespace Style
}  // namespace Kiran

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_COMMON_HELPER_H_

//
// Created by lxh on 2022/1/4.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_PROGRESS_BAR_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_PROGRESS_BAR_HELPER_H_

#include <QRect>
#include <QStyle>

class QStyleOption;
class QPainter;
class QWidget;

namespace Kiran
{
namespace Style
{
class SchemeLoader;
QSize progressBarSizeFromContents(const QStyle *style, const QStyleOption *option, const QSize &contentSize, const QWidget *widget);
QRect progressBarElementRect(const QStyle* style, QStyle::SubElement subElement, const QStyleOption* option, const QWidget* widget);

bool drawControlProgressBar(const QStyle *style,const QStyleOption *option,QPainter *painter,const QWidget *widget,SchemeLoader *scheme);
bool drawControlProgressBarGroove(const QStyle *style,const QStyleOption *option,QPainter *painter,const QWidget *widget,SchemeLoader *scheme);
bool drawControlProgressBarContents(const QStyle *style,const QStyleOption *option,QPainter *painter,const QWidget *widget,SchemeLoader *scheme);
bool drawControlProgressBarLabel(const QStyle *style,const QStyleOption *option,QPainter *painter,const QWidget *widget,SchemeLoader *scheme);
}
}

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_DRAW_HELPER_DRAW_PROGRESS_BAR_HELPER_H_

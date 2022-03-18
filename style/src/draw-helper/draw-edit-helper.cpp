//
// Created by lxh on 2021/12/20.
//

#include "draw-edit-helper.h"
#include "draw-common-helper.h"
#include "render-helper.h"
#include "scheme-loader.h"

#include <QColor>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QComboBox>

bool Kiran::Style::drawPEFrameLineEdit(const QStyle *style,
                                       const QStyleOption *option,
                                       QPainter *painter,
                                       const QWidget *widget,
                                       Kiran::Style::SchemeLoader *scheme)
{
    const auto &rect = option->rect;

    QColor background = scheme->getColor(widget, option, SchemeLoader::LineEdit_BackgroundColor);
    QColor border = scheme->getColor(widget, option, SchemeLoader::LineEdit_BorderColor);

    //控件高度不足够绘制边框
    if (rect.height() < 2 + option->fontMetrics.height())
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRect(rect);
        painter->restore();
    }
    else
    {
        if( qobject_cast<const QComboBox*>(widget) )
            RenderHelper::renderFlatFrame(painter, rect, 4, background, border);
        else
            RenderHelper::renderFrame(painter, rect, 1,4, background, border);
    }

    return true;
}

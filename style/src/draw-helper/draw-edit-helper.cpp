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

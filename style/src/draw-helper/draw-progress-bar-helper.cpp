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

#include "draw-progress-bar-helper.h"
#include "draw-common-helper.h"
#include "metrics.h"
#include "render-helper.h"
#include "scheme-loader-fetcher.h"

#include <QDebug>
#include <QPainter>
#include <QString>
#include <QStyle>
#include <QStyleOption>
#include <QWidget>

using namespace Kiran::Style;

QSize Kiran::Style::progressBarSizeFromContents(const QStyle *style,
                                                const QStyleOption *option,
                                                const QSize &contentSize,
                                                const QWidget *widget)
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    if (!progressBarOption) return contentSize;

    bool horizontal(progressBarOption->orientation == Qt::Horizontal);

    QSize size(contentSize);

    if (horizontal)
    {
        bool textVisible(progressBarOption->textVisible);

        size.setWidth(qMax(size.width(), int(Metrics::ProgressBar_Thickness)));
        size.setHeight(qMax(size.height(), int(Metrics::ProgressBar_Thickness)));
        if (textVisible) size.setHeight(qMax(size.height(), option->fontMetrics.height()));
    }
    else
    {
        size.setHeight(qMax(size.height(), int(Metrics::ProgressBar_Thickness)));
        size.setWidth(qMax(size.width(), int(Metrics::ProgressBar_Thickness)));
    }

    return size;
}

QRect Kiran::Style::progressBarElementRect(const QStyle *style,
                                           QStyle::SubElement subElement,
                                           const QStyleOption *option,
                                           const QWidget *widget)
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    if (!progressBarOption) return option->rect;

    bool textVisible(progressBarOption->textVisible);
    bool horizontal(progressBarOption->orientation == Qt::Horizontal);
    bool inverted(progressBarOption->invertedAppearance);

    QRect rect(option->rect);
    int frameWidth(style->pixelMetric(QStyle::PM_DefaultFrameWidth, option, widget));

    if (horizontal)
        rect = RenderHelper::insideMargin(rect, frameWidth, 0);
    else
        rect = RenderHelper::insideMargin(rect, 0, frameWidth);

    switch (subElement)
    {
    case QStyle::SE_ProgressBarContents:
    {
        rect = style->subElementRect(QStyle::SE_ProgressBarGroove,option,widget);

        qreal progress(progressBarOption->progress - progressBarOption->minimum);
        int steps(qMax(progressBarOption->maximum - progressBarOption->minimum, 1));
        qreal widthFrac = qMin(qreal(1), progress / steps);
        int indicatorSize(widthFrac * (horizontal ? rect.width() : rect.height()));

        if (horizontal)
        {
            rect = QRect(inverted ? (rect.right() - indicatorSize + 1) : rect.left(), rect.y(), indicatorSize, rect.height());
            rect = QStyle::visualRect(option->direction, rect, rect);
        }
        else
        {
            rect = QRect(rect.x(), inverted ? rect.top() : (rect.bottom() - indicatorSize + 1), rect.width(), indicatorSize);
        }
        break;
    }
    case QStyle::SE_ProgressBarGroove:
    {
        if (textVisible && horizontal)
        {
            QRect textRect(style->subElementRect(QStyle::SE_ProgressBarLabel, option, widget));
            textRect = QStyle::visualRect(option->direction, option->rect, textRect);
            rect.setRight(textRect.left() - Metrics::ProgressBar_ItemSpacing - 1);
            rect = QStyle::visualRect(option->direction, option->rect, rect);
            rect = RenderHelper::centerRect(rect, rect.width(), Metrics::ProgressBar_Thickness);
        }
        else if (horizontal)
        {
            rect = RenderHelper::centerRect(rect, rect.width(), Metrics::ProgressBar_Thickness);
        }
        else
        {
            rect = RenderHelper::centerRect(rect, Metrics::ProgressBar_Thickness, rect.height());
        }
        break;
    }
    case QStyle::SE_ProgressBarLabel:
    {
        if (!textVisible)
            return QRect();

        int textWidth = qMax(
            option->fontMetrics.size(RenderHelper::mnemonicsTextFlags(), progressBarOption->text).width(),
            option->fontMetrics.size(RenderHelper::mnemonicsTextFlags(), QStringLiteral("100%")).width());

        rect = RenderHelper::insideMargin(option->rect, Metrics::Frame_FrameWidth, 0);
        rect.setLeft(rect.right() - textWidth + 1);
        rect = QStyle::visualRect(option->direction, option->rect, rect);
        break;
    }
    default:
        break;
    }

    return rect;
}

bool Kiran::Style::drawControlProgressBar(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    if (!progressBarOption) return true;

    QStyleOptionProgressBar tempOption(*progressBarOption);

    QRect grooveRect = style->subElementRect(QStyle::SE_ProgressBarGroove,option,widget);
    QRect contentsRect = style->subElementRect(QStyle::SE_ProgressBarContents,option,widget);
    QRect labelRect = style->subElementRect(QStyle::SE_ProgressBarLabel,option,widget);

    tempOption.rect = grooveRect;
    style->drawControl(QStyle::CE_ProgressBarGroove,&tempOption,painter,widget);

    //TODO: BUSY动画
    tempOption.rect = contentsRect;
    style->drawControl(QStyle::CE_ProgressBarContents,&tempOption,painter,widget);

    bool textVisible( progressBarOption->textVisible );
    bool busy( progressBarOption->minimum == 0 && progressBarOption->maximum == 0 );
    if( textVisible && !busy )
    {
        tempOption.rect = labelRect;
        style->drawControl(QStyle::CE_ProgressBarLabel,&tempOption,painter,widget);
    }

    return true;
}

bool Kiran::Style::drawControlProgressBarGroove(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    if (!progressBarOption) return true;

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto grooveColor = schemeLoader->getColor(widget,option,SchemeLoader::Progress_Groove);

    RenderHelper::renderFrame(painter,option->rect,1,4,grooveColor);
    return true;
}

bool Kiran::Style::drawControlProgressBarContents(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    if (!progressBarOption) return true;

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto contentsColor = schemeLoader->getColor(widget,option,SchemeLoader::Progress_Content);

    RenderHelper::renderFrame(painter,option->rect,1,4,contentsColor);
    return true;
}

bool Kiran::Style::drawControlProgressBarLabel(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return true;

    bool horizontal = progressBarOption->orientation == Qt::Horizontal;
    if( !horizontal ) return true;

    const QRect& rect( option->rect );
    const QPalette& palette( option->palette );

    // store state and direction
    const QStyle::State& state( option->state );
    bool enabled( state & QStyle::State_Enabled );

    // define text rect
    Qt::Alignment hAlign( ( progressBarOption->textAlignment == Qt::AlignLeft ) ? Qt::AlignHCenter : progressBarOption->textAlignment );
    style->drawItemText( painter, rect, Qt::AlignVCenter | hAlign, palette, enabled, progressBarOption->text, QPalette::WindowText );
    return true;
}
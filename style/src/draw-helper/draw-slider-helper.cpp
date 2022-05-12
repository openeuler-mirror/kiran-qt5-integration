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
#include "draw-slider-helper.h"
#include "draw-common-helper.h"
#include "metrics.h"
#include "render-helper.h"
#include "style.h"
#include "scheme-loader-fetcher.h"

#include <QPainter>
#include <QProxyStyle>
#include <QStyleOption>

//TODO: 之后整合所有的绘制过程中的是否启用的开关
static const bool sliderDrawTickMarks = true;
enum Side
{
    SideNone = 0x0,
    SideLeft = 0x1,
    SideTop = 0x2,
    SideRight = 0x4,
    SideBottom = 0x8,
    AllSides = SideLeft | SideTop | SideRight | SideBottom
};

QSize Kiran::Style::sliderSizeFromContents(const QStyle *style,
                                           const QStyleOption *option,
                                           const QSize &contentSize,
                                           const QWidget *widget)
{
    // cast option and check
    const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
    if (!sliderOption) return contentSize;

    // store tick position and orientation
    const QSlider::TickPosition &tickPosition(sliderOption->tickPosition);
    bool horizontal(sliderOption->orientation == Qt::Horizontal);
    bool disableTicks = !sliderDrawTickMarks;

    // do nothing if no ticks are requested
    if (tickPosition == QSlider::NoTicks) return contentSize;

    /*
     * Qt adds its own tick length directly inside QSlider.
     * Take it out and replace by ours, if needed
     */
    const int tickLength(disableTicks ? 0 : (Metrics::Slider_TickLength + Metrics::Slider_TickMarginWidth + (Metrics::Slider_GrooveThickness - Metrics::Slider_ControlThickness) / 2));

    int builtInTickLength(2);

    QSize size(contentSize);
    if (horizontal)
    {
        if (tickPosition & QSlider::TicksAbove) size.rheight() += tickLength - builtInTickLength;
        if (tickPosition & QSlider::TicksBelow) size.rheight() += tickLength - builtInTickLength;
    }
    else
    {
        if (tickPosition & QSlider::TicksAbove) size.rwidth() += tickLength - builtInTickLength;
        if (tickPosition & QSlider::TicksBelow) size.rwidth() += tickLength - builtInTickLength;
    }

    return size;
}

bool Kiran::Style::drawCCSlider(const QStyle *style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget)
{
    // cast option and check
    const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
    if (!sliderOption) return true;

    // copy rect and palette
    const QRect &rect(option->rect);
    const QPalette &palette(option->palette);

    // copy state
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));
    bool hasFocus(enabled && (state & QStyle::State_HasFocus));
    bool horizontal(sliderOption->orientation == Qt::Horizontal);
    Side tickSide{SideNone};
    if (horizontal && sliderOption->tickPosition == QSlider::TicksAbove) tickSide = (Side)((int)tickSide | (int)SideTop);
    if (horizontal && sliderOption->tickPosition == QSlider::TicksBelow) tickSide = (Side)((int)tickSide | (int)SideBottom);
    if (!horizontal && sliderOption->tickPosition == QSlider::TicksLeft) tickSide = (Side)((int)tickSide | (int)SideLeft);
    if (!horizontal && sliderOption->tickPosition == QSlider::TicksRight) tickSide = (Side)((int)tickSide | (int)SideRight);

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto grooveColor = schemeLoader->getColor(widget,option,SchemeLoader::Slider_Groove);
    auto contentColor = schemeLoader->getColor(widget,option,SchemeLoader::Slider_Content);
    // tickmarks
    if ( sliderDrawTickMarks && (sliderOption->subControls & QStyle::SC_SliderTickmarks) )
    {
        bool upsideDown(sliderOption->upsideDown);
        int tickPosition(sliderOption->tickPosition);
        int available(style->pixelMetric(QStyle::PM_SliderSpaceAvailable, option, widget));
        int interval = sliderOption->tickInterval;
        if (interval < 1) interval = sliderOption->pageStep;
        if (interval >= 1)
        {
            int fudge(style->pixelMetric(QStyle::PM_SliderLength, option, widget) / 2);
            int current(sliderOption->minimum);

            // store tick lines
            QRect grooveRect(style->subControlRect(QStyle::CC_Slider, sliderOption, QStyle::SC_SliderGroove, widget));
            QList<QLine> tickLines;
            if (horizontal)
            {
                if (tickPosition & QSlider::TicksAbove) tickLines.append(QLine(rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength));
                if (tickPosition & QSlider::TicksBelow) tickLines.append(QLine(rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength));
            }
            else
            {
                if (tickPosition & QSlider::TicksAbove) tickLines.append(QLine(grooveRect.left() - Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.left() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength, rect.top()));
                if (tickPosition & QSlider::TicksBelow) tickLines.append(QLine(grooveRect.right() + Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.right() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength, rect.top()));
            }

            while (current <= sliderOption->maximum)
            {
                // adjust color
                QColor color((enabled && current <= sliderOption->sliderPosition) ? contentColor : grooveColor);
                painter->setPen(color);

                // calculate positions and draw lines
                int position(QStyle::sliderPositionFromValue(sliderOption->minimum, sliderOption->maximum, current, available) + fudge);
                foreach (const QLine &tickLine, tickLines)
                {
                    if (horizontal)
                        painter->drawLine(tickLine.translated(upsideDown ? (rect.width() - position) : position, 0));
                    else
                        painter->drawLine(tickLine.translated(0, upsideDown ? (rect.height() - position) : position));
                }

                // go to next position
                current += interval;
            }
        }
    }

    // groove
    if (sliderOption->subControls & QStyle::SC_SliderGroove)
    {
        if (hasFocus)
        {
            QRect focusRect = style->subElementRect(QStyle::SE_SliderFocusRect, option, widget);

            QStyleOptionFocusRect fropt;
            fropt.QStyleOption::operator=(*option);
            fropt.rect = focusRect;
            style->drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, painter, widget);
        }

        // retrieve groove rect
        QRect grooveRect(style->subControlRect(QStyle::CC_Slider, sliderOption, QStyle::SC_SliderGroove, widget));

        if (!enabled)
        {
            RenderHelper::renderFrame(painter,grooveRect,1,0,grooveColor);
        }
        else
        {
            bool upsideDown(sliderOption->upsideDown);

            // handle rect
            QRect handleRect(style->subControlRect(QStyle::CC_Slider, sliderOption, QStyle::SC_SliderHandle, widget));

            if (sliderOption->orientation == Qt::Horizontal)
            {
                QRect leftRect(grooveRect);
                QRect rightRect(grooveRect);
                leftRect.setRight(handleRect.right() - Metrics::Slider_ControlThickness / 2);
                rightRect.setLeft(handleRect.left() + Metrics::Slider_ControlThickness / 2);

                if (upsideDown)
                {
                    RenderHelper::renderFrame(painter,leftRect,1,0,grooveColor);
                    RenderHelper::renderFrame(painter,rightRect,1,0,contentColor);
                }
                else
                {
                    RenderHelper::renderFrame(painter,leftRect,1,0,contentColor);
                    RenderHelper::renderFrame(painter,rightRect,1,0,grooveColor);
                }
            }
            else
            {
                QRect topRect(grooveRect);
                topRect.setBottom(handleRect.bottom() - Metrics::Slider_ControlThickness / 2);
                QRect bottomRect(grooveRect);
                bottomRect.setTop(handleRect.top() + Metrics::Slider_ControlThickness / 2);

                if (upsideDown)
                {
                    RenderHelper::renderFrame(painter,topRect,1,0,grooveColor);
                    RenderHelper::renderFrame(painter,bottomRect,1,0,contentColor);
                }
                else
                {
                    RenderHelper::renderFrame(painter,topRect,1,0,contentColor);
                    RenderHelper::renderFrame(painter,bottomRect,1,0,grooveColor);
                }
            }
        }
    }

    // handle
    if (sliderOption->subControls & QStyle::SC_SliderHandle)
    {
        // get rect and center
        QRectF handleRect(style->subControlRect(QStyle::CC_Slider, sliderOption, QStyle::SC_SliderHandle, widget));

        // handle state
        bool handleActive(sliderOption->activeSubControls & QStyle::SC_SliderHandle);
        bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));

        QColor handleBorder = schemeLoader->getColor(widget,option,SchemeLoader::Slider_HandleBorder);
        QColor handleBackground = schemeLoader->getColor(widget,option,SchemeLoader::Slider_HandleBackground);

        //draw handle
        painter->setRenderHint(QPainter::Antialiasing);
        handleRect.adjust(1, 1, -1, -1);

        if (handleBorder.isValid())
        {
            QPen pen;
            pen.setCapStyle(Qt::RoundCap);
            pen.setColor(handleBorder);
            pen.setWidth(2);
            painter->setPen(pen);
            handleRect = RenderHelper::strokedRect(handleRect, 2);
        }
        else
        {
            painter->setPen(Qt::NoPen);
        }
        painter->setBrush(handleBackground);
        painter->drawRoundedRect(handleRect, handleRect.width()/2, handleRect.width());
    }

    return true;
}

QRect Kiran::Style::sliderElementRect(const QStyle *style, QStyle::SubElement subElement, const QStyleOption *option, const QWidget *widget)
{
    QRect rect(option->rect);

    switch (subElement)
    {
    case QStyle::SE_SliderFocusRect:
    {
        const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
        QRect r(option->rect);
        if (sliderOption->orientation == Qt::Vertical)
        {
            int thickness = Metrics::Slider_GrooveThickness + 8;
            return QRect(r.center().x() - thickness / 2, r.top() + 1, thickness + 1, r.height() - 1);
        }
        else
        {
            int thickness = Metrics::Slider_GrooveThickness + 6;
            return QRect(r.left() + 1, r.center().y() - thickness / 2, r.width() - 1, thickness + 1);
        }
    }
    default:
        break;
    }

    return rect;
}

bool Kiran::Style::sliderSubControlRect(const QStyle *style, const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *widget, QRect &controlRect)
{
    // cast option and check
    const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(opt));
    if (!sliderOption)
        return false;

    switch (sc)
    {
    case QStyle::SC_SliderGroove:
    {
        // direction
        bool horizontal(sliderOption->orientation == Qt::Horizontal);

        // get base class rect
        //NOTE:该处直接调用父类的方法
        QRect grooveRect = qobject_cast<const ParentStyle*>(style)->ParentStyle::subControlRect(QStyle::CC_Slider,opt,sc,widget);
        grooveRect = RenderHelper::insideMargin(grooveRect, style->pixelMetric(QStyle::PM_DefaultFrameWidth, opt, widget));

        // centering
        if (horizontal)
            grooveRect = RenderHelper::centerRect(grooveRect, grooveRect.width(), Metrics::Slider_GrooveThickness);
        else
            grooveRect = RenderHelper::centerRect(grooveRect, Metrics::Slider_GrooveThickness, grooveRect.height());

        controlRect = grooveRect;
        break;
    }
    default:
        return false;
    }

    return true;
}

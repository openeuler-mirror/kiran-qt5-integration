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

#include "draw-scroll-bar-helper.h"
#include "draw-common-helper.h"
#include "metrics.h"
#include "render-helper.h"
#include "scheme-loader-fetcher.h"

#include <QAbstractScrollArea>
#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QStyleOptionComplex>
#include <QWidget>

using namespace Kiran::Style;

enum ScrollBarButtonType
{
    NoButton,
    SingleButton,
    DoubleButton
} _subLineButtons = NoButton,
  _addLineButtons = NoButton;

//* returns height for scrollbar buttons depending of button types
int scrollBarButtonHeight(ScrollBarButtonType type)
{
    switch (type)
    {
    case NoButton:
        return Metrics::ScrollBar_NoButtonHeight;
    case SingleButton:
        return Metrics::ScrollBar_SingleButtonHeight;
    case DoubleButton:
        return Metrics::ScrollBar_DoubleButtonHeight;
    default:
        return 0;
    }
}

QRect scrollBarInternalSubControlRect(const QStyleOptionComplex *option,
                                      QStyle::SubControl subControl)
{
    const QRect &rect = option->rect;
    const QStyle::State &state(option->state);
    bool horizontal(state & QStyle::State_Horizontal);

    switch (subControl)
    {
    case QStyle::SC_ScrollBarSubLine:
    {
        int majorSize(scrollBarButtonHeight(_subLineButtons));
        if (horizontal)
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(rect.left(), rect.top(), majorSize, rect.height()));
        }
        else
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(rect.left(), rect.top(), rect.width(), majorSize));
        }
    }
    case QStyle::SC_ScrollBarAddLine:
    {
        int majorSize(scrollBarButtonHeight(_addLineButtons));
        if (horizontal)
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(rect.right() - majorSize + 1, rect.top(), majorSize, rect.height()));
        }
        else
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(rect.left(), rect.bottom() - majorSize + 1, rect.width(), majorSize));
        }
    }

    default:
        return QRect();
    }
}

bool Kiran::Style::scrollBarSubControlRect(const QStyle *style,
                                           const QStyleOptionComplex *option,
                                           QStyle::SubControl subControl,
                                           const QWidget *widget,
                                           QRect &controlRect)
{
    const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
    if (!sliderOption)
        return false;

    const QStyle::State &state(option->state);
    bool horizontal(state & QStyle::State_Horizontal);

    switch (subControl)
    {
    case QStyle::SC_ScrollBarAddLine:
    case QStyle::SC_ScrollBarSubLine:
    case QStyle::SC_ScrollBarAddPage:
    case QStyle::SC_ScrollBarSubPage:
        controlRect = QRect();
        break;
    case QStyle::SC_ScrollBarSlider:
    {
        // handle RTL here to unreflect things if need be
        QRect groove = QStyle::visualRect(option->direction, option->rect, style->subControlRect(QStyle::CC_ScrollBar, option, QStyle::SC_ScrollBarGroove, widget));
        groove.adjust(0, 0, 1, 1);

        if (sliderOption->minimum == sliderOption->maximum)
        {
            controlRect = groove;
            break;
        }

        // Figure out how much room there is
        int space(horizontal ? groove.width() : groove.height());

        // Calculate the portion of this space that the slider should occupy
        int sliderSize = space * qreal(sliderOption->pageStep) / (sliderOption->maximum - sliderOption->minimum + sliderOption->pageStep);
        sliderSize = qMax(sliderSize, static_cast<int>(Metrics::ScrollBar_MinSliderHeight));
        sliderSize = qMin(sliderSize, space);

        space -= sliderSize;
        if (space <= 0)
        {
            controlRect = groove;
            break;
        }

        int pos = qRound(qreal(sliderOption->sliderPosition - sliderOption->minimum) / (sliderOption->maximum - sliderOption->minimum) * space);
        if (sliderOption->upsideDown)
        {
            pos = space - pos;
        }

        if (horizontal)
        {
            controlRect = QStyle::visualRect(option->direction,option->rect,QRect(groove.left() + pos, groove.top(), sliderSize, groove.height()));
        }
        else
        {
            controlRect = QStyle::visualRect(option->direction,option->rect,QRect(groove.left(), groove.top() + pos, groove.width(), sliderSize));
        }
        break;
    }
    case QStyle::SC_ScrollBarGroove:
    {
        QRect topRect = QStyle::visualRect(option->direction, option->rect, scrollBarInternalSubControlRect(option, QStyle::SC_ScrollBarSubLine));
        QRect bottomRect = QStyle::visualRect(option->direction, option->rect, scrollBarInternalSubControlRect(option, QStyle::SC_ScrollBarAddLine));

        QPoint topLeftCorner;
        QPoint botRightCorner;

        if (horizontal)
        {
            topLeftCorner = QPoint(topRect.right() + 1, topRect.top());
            botRightCorner = QPoint(bottomRect.left() - 1, topRect.bottom());
        }
        else
        {
            topLeftCorner = QPoint(topRect.left(), topRect.bottom() + 1);
            botRightCorner = QPoint(topRect.right(), bottomRect.top() - 1);
        }

        // define rect
        controlRect = QStyle::visualRect(option->direction,option->rect,QRect(topLeftCorner, botRightCorner));
        break;
    }
    default:
        return true;
    }

    return true;
}

bool Kiran::Style::drawCCScrollBar(const QStyle *style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget)
{
    const auto scrollbarOption = qstyleoption_cast<const QStyleOptionSlider *>(option);
    if (!scrollbarOption)
    {
        return true;
    }

    bool enabled( option->state & QStyle::State_Enabled );
    bool mouseOver( (option->state & QStyle::State_Active) && option->state & QStyle::State_MouseOver );

    if( option->subControls & QStyle::SC_ScrollBarGroove )
    {
        auto grooveRect = style->subControlRect(QStyle::CC_ScrollBar,option,QStyle::SC_ScrollBarGroove,widget);
//不绘制滑动槽
#if 0
        if( mouseOver )
        {
            QColor grooveColor = KiranPalette::instance()->color(widget,option,KiranPalette::Window,KiranPalette::Background);
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(grooveColor);
            painter->drawRect(grooveRect);
            painter->restore();
        }
#endif
    }

    if (scrollbarOption->subControls & QStyle::SC_ScrollBarSlider)
    {
        QStyleOptionSlider sliderOption = *scrollbarOption;
        sliderOption.state = scrollbarOption->state;
        sliderOption.rect = style->subControlRect(QStyle::CC_ScrollBar, &sliderOption, QStyle::SC_ScrollBarSlider, widget);
        if (sliderOption.rect.isValid())
        {
            style->drawControl(QStyle::CE_ScrollBarSlider, &sliderOption, painter, widget);
        }
    }

    return true;
}

bool Kiran::Style::drawControlScrollBarGroove(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, Kiran::Style::SchemeLoader *scheme)
{
    return false;
}

bool Kiran::Style::drawControlScrollBarSlider(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const QStyleOptionSlider *sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
    if (!sliderOption)
    {
        return true;
    }

    const QStyle::State &state(option->state);
    bool horizontal(state & QStyle::State_Horizontal);
    //const QRect &rect(horizontal ? option->rect.adjusted(-1, 4, 0, -4) : option->rect.adjusted(4, -1, -4, 0));
    const QRect &rect(option->rect);
    QRect handleRect ;

    bool enabled(state & QStyle::State_Enabled);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));
    bool sunken(enabled && (state & (QStyle::State_On | QStyle::State_Sunken)));

    const QWidget *parent = nullptr;
    if ((widget && widget->parentWidget()))
    {
        auto scrollBar = qobject_cast<const QScrollBar *>(widget);

        QAbstractScrollArea *scrollArea = nullptr;
        if (!(scrollArea = qobject_cast<QAbstractScrollArea *>(widget->parentWidget())))
        {
            scrollArea = qobject_cast<QAbstractScrollArea *>(widget->parentWidget()->parentWidget());
        }

        if (scrollArea && (scrollBar == scrollArea->verticalScrollBar() || scrollBar == scrollArea->horizontalScrollBar()))
        {
            parent = scrollArea;
        }
    }

    bool hasFocus(enabled && parent && parent->hasFocus());

    handleRect = rect;
    if( !mouseOver && horizontal )
    {
        handleRect = rect.adjusted(0,1,0,-1);
    }
    else if( !mouseOver && !horizontal)
    {
        handleRect = rect.adjusted(1,0,-1,0);
    }

    QStyleOption tempOption(*option);
    if(sunken)
    {
        tempOption.state |= QStyle::State_Sunken;
    }

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto sliderColor = schemeLoader->getColor(widget,option,SchemeLoader::Scroll_Slider);

    painter->setRenderHint(QPainter::Antialiasing,true);
    QPainterPath painterPath = RenderHelper::roundedPath(handleRect,AllCorners,2);
    painter->fillPath(painterPath,sliderColor);
    return true;
}

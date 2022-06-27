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
#include "draw-indicator-helper.h"
#include "draw-common-helper.h"
#include "metrics.h"
#include "render-helper.h"
#include "scheme-loader-fetcher.h"

#include <QDebug>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QSvgRenderer>
#include <QWidget>

namespace Kiran
{
bool drawPEIndicatorButtonDropDown(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto toolButtonOption(qstyleoption_cast<const QStyleOptionToolButton *>(option));
    if (!toolButtonOption)
        return true;

    const QStyle::State &state(option->state);
    bool autoRaise(state & QStyle::State_AutoRaise);
    bool enabled(state & QStyle::State_Enabled);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));
    bool sunken(enabled && (state & QStyle::State_Sunken));

    // do nothing for autoraise buttons
    if ((autoRaise && !sunken && !mouseOver) || !(toolButtonOption->subControls & QStyle::SC_ToolButtonMenu))
        return true;

    const QPalette &palette(option->palette);
    const QRect &rect(option->rect);

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto backgroundColor = schemeLoader->getColor(widget, option, SchemeLoader::Button_Background);
    auto borderColor = schemeLoader->getColor(widget, option, SchemeLoader::Button_Border);
    auto separatorColor = borderColor;

    QRect frameRect(rect);
    painter->setClipRect(rect);
    frameRect.adjust(-Metrics::Frame_FrameRadius - 1, 0, 0, 0);
    frameRect = QStyle::visualRect(option->direction, option->rect, frameRect);

    // render
    RenderHelper::renderFrame(painter, frameRect, 1, 4, backgroundColor, borderColor);

    // also render separator
    QRect separatorRect(rect.adjusted(0, 2, -2, -2));
    separatorRect.setWidth(1);
    separatorRect = QStyle::visualRect(option->direction, option->rect, separatorRect);
    RenderHelper::renderSeparator(painter, separatorRect, true, separatorColor);

    return true;
}

bool drawPEIndicatorArrow(ArrowOrientation orientation, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto arrowColor = schemeLoader->getColor(widget, option, SchemeLoader::Indicator_Arrow);

    RenderHelper::renderArrow(painter, option->rect, orientation, arrowColor);
    return true;
}

bool drawPEIndicatorArrowUp(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    return drawPEIndicatorArrow(Arrow_Up, option, painter, widget);
}

bool drawPEIndicatorArrowDown(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    return drawPEIndicatorArrow(Arrow_Down, option, painter, widget);
}

bool drawPEIndicatorArrowLeft(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    return drawPEIndicatorArrow(Arrow_Left, option, painter, widget);
}

bool drawPEIndicatorArrowRight(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    return drawPEIndicatorArrow(Arrow_Right, option, painter, widget);
}

bool drawPEIndicatorRadioButton(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto &rect(option->rect);

    bool checked = option->state & QStyle::State_On;
    QString state = "normal";
    if (!(option->state & QStyle::State_Enabled))
    {
        state = "disabled";
    }
    else if ((option->state & QStyle::State_Sunken))
    {
        state = "active";
    }
    else if ((option->state & QStyle::State_MouseOver))
    {
        state = "hover";
    }
    QString iconUrl = QString(":/style-helper/images/radio-%1-%2.svg").arg(checked ? "checked" : "unchecked").arg(state);

    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    QSvgRenderer renderer(iconUrl);
    renderer.render(painter, rect);
    painter->restore();
    return true;
}

bool drawPEIndicatorCheckBox(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto &rect(option->rect);

    bool checked = option->state & QStyle::State_On;
    QString state = "normal";
    if (!(option->state & QStyle::State_Enabled))
    {
        state = "disabled";
    }
    else if ((option->state & QStyle::State_Sunken))
    {
        state = "active";
    }
    else if ((option->state & QStyle::State_MouseOver))
    {
        state = "hover";
    }
    QString iconUrl = QString(":/style-helper/images/check-%1-%2.svg").arg(checked ? "checked" : "unchecked").arg(state);

    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    QSvgRenderer renderer(iconUrl);
    renderer.render(painter, rect);
    painter->restore();
    return true;
}

bool drawPEIndicatorBranch(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const QRect &rect(option->rect);
    const QPalette &palette(option->palette);

    const QStyle::State &state(option->state);
    bool reverseLayout(option->direction == Qt::RightToLeft);

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();

    // 绘制扩展的箭头
    int expanderAdjust = 0;
    if (state & QStyle::State_Children)
    {
        // state
        bool expanderOpen(state & QStyle::State_Open);
        bool enabled(state & QStyle::State_Enabled);
        bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));

        // expander rect
        int expanderSize = qMin(rect.width(), rect.height());
        expanderSize = qMin(expanderSize, int(Metrics::ItemView_ArrowSize));
        expanderAdjust = expanderSize / 2 + 1;
        QRect arrowRect = RenderHelper::centerRect(rect, expanderSize, expanderSize);

        // get orientation from option
        ArrowOrientation orientation;
        if (expanderOpen)
        {
            orientation = Arrow_Down;
        }
        else if (reverseLayout)
        {
            orientation = Arrow_Left;
        }
        else
        {
            orientation = Arrow_Right;
        }
        auto arrowColor = schemeLoader->getColor(widget, option, SchemeLoader::Indicator_Arrow);
        RenderHelper::renderArrow(painter, arrowRect, orientation, arrowColor, QSize(12, 12));
    }

    if (!RenderHelper::drawTreeBranches())
    {
        return true;
    }

    QPoint center(rect.center());
    auto lineColor = schemeLoader->getColor(widget, option, SchemeLoader::Frame_Border);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(0.5, 0.5);
    painter->setPen(QPen(lineColor, 1));
    if (state & (QStyle::State_Item | QStyle::State_Children | QStyle::State_Sibling))
    {
        QLineF line(QPointF(center.x(), rect.top()), QPointF(center.x(), center.y() - expanderAdjust - 1));
        painter->drawLine(line);
    }

    // 如果存在Item则绘制左/右取决与方向的线
    if (state & QStyle::State_Item)
    {
        const QLineF line = reverseLayout ? QLineF(QPointF(rect.left(), center.y()), QPointF(center.x() - expanderAdjust, center.y())) : QLineF(QPointF(center.x() + expanderAdjust, center.y()), QPointF(rect.right(), center.y()));
        painter->drawLine(line);
    }

    // 如果存在同级节点的话，则绘制上下的线
    if (state & QStyle::State_Sibling)
    {
        QLineF line(QPointF(center.x(), center.y() + expanderAdjust), QPointF(center.x(), rect.bottom()));
        painter->drawLine(line);
    }
    painter->restore();

    return true;
}

bool drawPEIndicatorToolBarSeparator(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const QStyle::State &state = option->state;
    bool enable = (state & QStyle::State_Enabled);
    bool separatorIsVertical(state & QStyle::State_Horizontal);

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto separatorColor = schemeLoader->getColor(widget, option, SchemeLoader::Widget_Border);
    RenderHelper::renderSeparator(painter, option->rect, separatorIsVertical, separatorColor);
    return true;
}

bool drawPEIndicatorToolBarHandle(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const QStyle::State &state = option->state;
    QRect rect(option->rect);
    bool separatorIsVertical(state & QStyle::State_Horizontal);
    bool enabled(state & QStyle::State_Enabled);

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto separatorColor = schemeLoader->getColor(widget, option, SchemeLoader::Widget_Border);
    if (separatorIsVertical)
    {
        rect.setWidth(Metrics::ToolBar_HandleWidth);
        rect = RenderHelper::centerRect(option->rect, rect.size());
        rect.setWidth(3);
        RenderHelper::renderSeparator(painter, rect, separatorIsVertical, separatorColor);

        rect.translate(2, 0);
        RenderHelper::renderSeparator(painter, rect, separatorIsVertical, separatorColor);
    }
    else
    {
        rect.setHeight(Metrics::ToolBar_HandleWidth);
        rect = RenderHelper::centerRect(option->rect, rect.size());
        rect.setHeight(3);
        RenderHelper::renderSeparator(painter, rect, separatorIsVertical, separatorColor);

        rect.translate(0, 2);
        RenderHelper::renderSeparator(painter, rect, separatorIsVertical, separatorColor);
    }

    return true;
}
}  // namespace Kiran
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

#include "draw-spin-box-helper.h"
#include "define.h"
#include "draw-common-helper.h"
#include "metrics.h"
#include "render-helper.h"
#include "scheme-loader-fetcher.h"

#include <QDebug>
#include <QPainter>
#include <QRect>
#include <QStyle>
#include <QStyleOption>
#include <QWidget>
#include <QtMath>

using namespace Kiran::Style;

QSize Kiran::Style::spinBoxSizeFromContents(const QStyle *style,
                                            const QStyleOption *option,
                                            const QSize &contentSize,
                                            const QWidget *widget)
{
    const auto spinBoxOption(qstyleoption_cast<const QStyleOptionSpinBox *>(option));
    if (!spinBoxOption) return contentSize;

    bool flat(!spinBoxOption->frame);

    // copy size
    QSize size(contentSize);

    // add editor margins
    int frameWidth(style->pixelMetric(QStyle::PM_SpinBoxFrameWidth, option, widget));
    if (!flat) size = RenderHelper::expandSize(size, frameWidth);

    // add button width and spacing
    size.rwidth() += 2 * size.height() - 1;

    return size;
}

bool Kiran::Style::spinBoxSubControlRect(const QStyle *style,
                                         const QStyleOptionComplex *option,
                                         QStyle::SubControl subControl,
                                         const QWidget *widget,
                                         QRect &controlRect)
{
    const auto spinBoxOption(qstyleoption_cast<const QStyleOptionSpinBox *>(option));
    if (!spinBoxOption) return false;

    bool flat(!spinBoxOption->frame);

    QRect rect(option->rect);
    switch (subControl)
    {
    case QStyle::SC_SpinBoxFrame:
        controlRect = flat ? QRect() : rect;
        break;
    case QStyle::SC_SpinBoxUp:
        if (rect.width() > 2 * rect.height() + 24)
        {
            controlRect = QRect(rect.right() - rect.height(), rect.top(), rect.height(), rect.height() - 1);
        }
        else
        {
            controlRect = QRect(rect.right() - 0.6 * rect.height() + 1, rect.top(), rect.height() * 0.6, rect.height() / 2 + 3);
        }
        break;
    case QStyle::SC_SpinBoxDown:
        if (rect.width() > 2 * rect.height() + 24)
        {
            controlRect = QRect(rect.right() - 2 * rect.height() + 1,rect.top(),rect.height(),rect.height() - 1);
        }
        else
        {
            controlRect = QRect(rect.right() - 0.6 * rect.height() + 1,rect.top() + rect.height() / 2 - 2,rect.height() * 0.6,rect.height() / 2 + 2);
        }
        break;
    case QStyle::SC_SpinBoxEditField:
    {
        int frameWidth(style->pixelMetric(QStyle::PM_SpinBoxFrameWidth, option, widget));
        if (rect.width() > 2 * rect.height() + 24)
            controlRect = QRect(
                rect.left(), rect.top(),
                rect.width() - 2 * rect.height() - frameWidth,
                rect.height());
        else
            controlRect = QRect(
                rect.left(), rect.top(),
                rect.width() - 0.6 * rect.height() - frameWidth,
                rect.height());

        // remove right side line editor margins
        if (!flat && controlRect.height() >= option->fontMetrics.height() + 2 * frameWidth)
        {
            controlRect.adjust(frameWidth, frameWidth, 0, -frameWidth);
        }

        controlRect = QStyle::visualRect(option->direction, option->rect, controlRect);
        break;
    }
    default:
        return  false;
    }

    return true;
}

void renderSpinBoxArrow(const QStyle *style, const QStyle::SubControl &subControl, const QStyleOptionSpinBox *option, QPainter *painter, const QWidget *widget)
{
    const QStyle::State &state(option->state);

    QRectF arrowRect(style->subControlRect(QStyle::CC_SpinBox, option, subControl, widget));

    bool enabled(state & QStyle::State_Enabled);
    const bool atLimit = (subControl == QStyle::SC_SpinBoxUp && !(option->stepEnabled & QAbstractSpinBox::StepUpEnabled)) ||
                         (subControl == QStyle::SC_SpinBoxDown && !(option->stepEnabled & QAbstractSpinBox::StepDownEnabled));
    enabled &= !atLimit;

    const bool mouseOver(enabled && (state & QStyle::State_MouseOver));
    bool sunken(state & QStyle::State_Sunken && option->activeSubControls & subControl);
    bool hasFocus(state & QStyle::State_HasFocus);

    const bool subControlHover(enabled && mouseOver && (option->activeSubControls & subControl));
    const bool subControlSunken(enabled && (sunken) && (option->activeSubControls & subControl));

    const QMap<QStyle::State, bool> subControlFlagMap = {
        {QStyle::State_MouseOver, subControlHover},
        {QStyle::State_Sunken, subControlSunken},
        {QStyle::State_Enabled, enabled}};

    //复制一个子控件StyleOption,更新其中的状态
    QStyleOption tempOption(*option);
    for (auto iter = subControlFlagMap.begin(); iter != subControlFlagMap.end(); iter++)
    {
        if (iter.value())
        {
            tempOption.state |= iter.key();
        }
        else
        {
            tempOption.state &= ~iter.key();
        }
    }

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto borderColor = schemeLoader->getColor(widget,option,SchemeLoader::SpinBox_Border);
    auto backgroundColor = schemeLoader->getColor(widget,option,SchemeLoader::SpinBox_Background);
    auto signColor = schemeLoader->getColor(widget,option,SchemeLoader::SpinBox_SignColor);

    //绘制按钮与输入区域之间的分割线
    if (subControl == QStyle::SC_SpinBoxDown)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(borderColor);
        int highlight = hasFocus ? 1 : 0;
        painter->drawLine(arrowRect.left(), arrowRect.top() + 2 + highlight, arrowRect.left(), arrowRect.bottom() - 1 - highlight);
    }

    if (subControl == QStyle::SC_SpinBoxUp)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(borderColor);
        int highlight = hasFocus ? 1 : 0;
        painter->drawLine(arrowRect.left(), arrowRect.top() + 2 + highlight, arrowRect.left(), arrowRect.bottom() - 1 - highlight);
    }

    //绘制按钮背景色
    painter->setPen(Qt::NoPen);
    QColor background = backgroundColor;
    painter->setBrush(background);
    painter->drawRect(arrowRect.adjusted(0.5,0.5,-0.5,-0.5));

    //绘制按钮之上的符号 “+”，“-”
    QPen pen(signColor, 2);
    pen.setCapStyle(Qt::FlatCap);
    QRect r = arrowRect.adjusted(1, 2, 0, 0).toRect();
    painter->setPen(pen);
    painter->drawLine(r.center() - QPointF(5, 0), r.center() + QPointF(5, 0));
    if (subControl == QStyle::SC_SpinBoxUp)
        painter->drawLine(r.center() - QPointF(0, 5), r.center() + QPointF(0, 5));
}

bool Kiran::Style::drawCCSpinBox(const QStyle *style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget)
{
    const auto spinBoxOption(qstyleoption_cast<const QStyleOptionSpinBox *>(option));
    if (!spinBoxOption) return true;

    const QPalette &palette(option->palette);
    const QRect &rect(option->rect);

    if (option->subControls & QStyle::SC_SpinBoxFrame)
    {
        // detect flat spinboxes
        bool flat(!spinBoxOption->frame);
        flat |= (rect.height() < 2 * Metrics::Frame_FrameWidth + Metrics::SpinBox_ArrowButtonWidth);
        if (flat)
        {
            const auto &background = palette.color(QPalette::Base);

            painter->setBrush(background);
            painter->setPen(Qt::NoPen);
            painter->drawRect(rect);
        }
        else
        {
            style->drawPrimitive(QStyle::PE_FrameLineEdit, option, painter, widget);
        }
    }

    //裁剪掉按钮露出圆角边框的区域
    QPainterPath catPath = RenderHelper::roundedPath(RenderHelper::insideMargin(option->rect, 1), AllCorners, 4);
    PainterSaver painterSaver(painter);
    painter->setClipPath(catPath);
    if (option->subControls & QStyle::SC_SpinBoxUp) renderSpinBoxArrow(style, QStyle::SC_SpinBoxUp, spinBoxOption, painter, widget);
    if (option->subControls & QStyle::SC_SpinBoxDown) renderSpinBoxArrow(style, QStyle::SC_SpinBoxDown, spinBoxOption, painter, widget);

    return true;
}

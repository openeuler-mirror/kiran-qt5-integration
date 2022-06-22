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
#include "draw-combo-box-helper.h"
#include "draw-common-helper.h"
#include "metrics.h"
#include "render-helper.h"
#include "scheme-loader-fetcher.h"

#include <QComboBox>
#include <QDebug>
#include <QPainter>
#include <QStyleOption>

namespace Kiran
{
QSize comboBoxSizeFromContents(const QStyle *style, const QStyleOption *option, const QSize &contentSize, const QWidget *widget)
{
    const auto comboBoxOption(qstyleoption_cast<const QStyleOptionComboBox *>(option));
    if (!comboBoxOption) return contentSize;

    QSize size(contentSize);

    // 若存在Frame绘制，大小扩展开  FrameWidth
    const bool flat(!comboBoxOption->frame);
    const int frameWidth(style->pixelMetric(QStyle::PM_ComboBoxFrameWidth, option, widget));
    if (!flat)
    {
        size = RenderHelper::expandSize(size, frameWidth);
    }

    size.rwidth() += Metrics::MenuButton_IndicatorWidth;
    size.rwidth() += Metrics::Button_ItemSpacing;

    size.rwidth() += size.height();

    // 确保有足够的高度绘制下拉按钮指示器
    size.setHeight(qMax(size.height(), int(Metrics::MenuButton_IndicatorWidth)));

    //    size = RenderHelper::expandSize(size, Metrics::ComboBox_MarginWidth, Metrics::ComboBox_MarginHeight);

    // 确保至少有最小大小
    size.setHeight(qMax(size.height(), int(Metrics::ComboBox_MinHeight)));
    size.setWidth(qMax(size.width(), int(Metrics::ComboBox_MinWidth)));

    return size;
}

bool comboBoxSubControlRect(const QStyle *style, const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *widget, QRect &controlRect)
{
    const auto comboBoxOption(qstyleoption_cast<const QStyleOptionComboBox *>(opt));
    if (!comboBoxOption) return false;

    const bool editable(comboBoxOption->editable);
    const bool flat(editable && !comboBoxOption->frame);

    auto rect(opt->rect);

    const int frameWidth(style->pixelMetric(QStyle::PM_ComboBoxFrameWidth, opt, widget));

    switch (sc)
    {
    case QStyle::SC_ComboBoxFrame:
        controlRect = flat ? rect : QRect();
        break;
    case QStyle::SC_ComboBoxListBoxPopup:
        controlRect = rect;
        break;
    case QStyle::SC_ComboBoxArrow:
        controlRect = QRect(rect.right() - rect.height() + 1,
                            rect.top(),
                            rect.height(), rect.height());
        break;
    case QStyle::SC_ComboBoxEditField:
    {
        controlRect = QRect(rect.left(), rect.top(), rect.width() - rect.height() - 4, rect.height());
        controlRect.adjust(frameWidth, frameWidth, 0, -frameWidth);
        controlRect = QStyle::visualRect(opt->direction, opt->rect, controlRect);
        break;
    }
    default:
        return false;
    }
    return true;
}

bool drawCCComboBox(const QStyle *style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget)
{
    const auto comboBoxOption(qstyleoption_cast<const QStyleOptionComboBox *>(option));
    if (!comboBoxOption)
    {
        return true;
    }

    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool editable(comboBoxOption->editable);
    bool arrowActive(comboBoxOption->activeSubControls & QStyle::SC_ComboBoxArrow);
    bool flat(!comboBoxOption->frame);
    bool sunken;

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();

    if (editable)
    {
        sunken = arrowActive && enabled && (state & (QStyle::State_On | QStyle::State_Sunken));
    }
    else
    {
        sunken = enabled && (state & (QStyle::State_On | QStyle::State_Sunken));
    }

    const auto comboBox = qobject_cast<const QComboBox *>(widget);
    if (!comboBox)
        return true;

    const bool empty(comboBox && !comboBox->count());

    // TODO: 当ComboBox为空时，应当特别绘制
    //  frame
    if (option->subControls & QStyle::SC_ComboBoxFrame)
    {
        if (editable)
        {
            // FIXME:下拉框编辑状态悬浮判断没有区域
            flat |= (option->rect.height() <= 2 * Metrics::Frame_FrameWidth + Metrics::MenuButton_IndicatorWidth);
            if (flat)
            {
                const auto &background = option->palette.color(QPalette::Base);
                painter->setBrush(background);
                painter->setPen(Qt::NoPen);
                painter->drawRect(option->rect);
            }
            else
            {
                QRectF arrowRect = style->subControlRect(QStyle::CC_ComboBox, comboBoxOption, QStyle::SC_ComboBoxArrow, widget);

                // ComboBox 箭头按钮
                arrowRect.adjust(1.5, 1.5, -1.5, -1.5);
                QPainterPath indicatorPainterPath = RenderHelper::roundedPath(arrowRect, CornersRight, 4);

                auto background = schemeLoader->getColor(widget, option, SchemeLoader::Combo_Background);
                auto border = schemeLoader->getColor(widget, option, SchemeLoader::Combo_Border);

                QPen pen = painter->pen();
                painter->setRenderHints(QPainter::Antialiasing);
                painter->setPen(border);
                painter->setBrush(background);
                painter->drawPath(indicatorPainterPath);

                // 输入框
                QStyleOptionComplex tmpOpt(*option);
                // 让输入框长度加上一个宽度进行绘制用来去掉输入框的圆角
                tmpOpt.rect.setWidth(tmpOpt.rect.width() - arrowRect.width() + 3);
                style->drawPrimitive(QStyle::PE_FrameLineEdit, &tmpOpt, painter, widget);
            }
        }
        else
        {
            if (flat)
            {
                auto background = option->palette.color(QPalette::Base);
                if (sunken)
                {
                    background = schemeLoader->getColor(widget, option, SchemeLoader::Combo_Background);
                }
                RenderHelper::renderFrame(painter, option->rect, 1, 0, background);
            }
            else
            {
                QStyleOptionComboBox tempOption(*comboBoxOption);
                auto backgroundColor = schemeLoader->getColor(widget, &tempOption, SchemeLoader::Combo_Background);
                auto borderColor = schemeLoader->getColor(widget, &tempOption, SchemeLoader::Combo_Border);
                RenderHelper::renderFrame(painter, option->rect, 1, 4, backgroundColor, borderColor);
            }
        }
    }

    // arrow
    if (option->subControls & QStyle::SC_ComboBoxArrow)
    {
        auto arrowRect(style->subControlRect(QStyle::CC_ComboBox, option, QStyle::SC_ComboBoxArrow, widget));
        auto arrowColor = schemeLoader->getColor(widget, option, SchemeLoader::Indicator_Arrow);
        RenderHelper::renderArrow(painter, arrowRect, Arrow_Down, arrowColor);
    }

    return true;
}

bool drawControlComboBoxLabel(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto comboBoxOption(qstyleoption_cast<const QStyleOptionComboBox *>(option));
    if (!comboBoxOption) return false;
    if (comboBoxOption->editable) return false;

    const QStyle::State &state(option->state);
    const bool enabled(state & QStyle::State_Enabled);
    const bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    const bool mouseOver(enabled && (option->state & QStyle::State_MouseOver));
    const bool hasFocus(enabled && !mouseOver && (option->state & QStyle::State_HasFocus));
    const bool flat(!comboBoxOption->frame);

    QPalette::ColorRole textRole;
    if (flat)
    {
        textRole = QPalette::WindowText;
    }
    else if (hasFocus)
    {
        textRole = QPalette::ButtonText;
    }
    else
    {
        textRole = QPalette::ButtonText;
    }

    painter->setPen(QPen(option->palette.color(textRole), 1));

    // 获取文本框位置
    auto editRect = style->subControlRect(QStyle::CC_ComboBox, comboBoxOption, QStyle::SC_ComboBoxEditField, widget);

    painter->save();
    painter->setClipRect(editRect);
    // 绘制图标
    if (!comboBoxOption->currentIcon.isNull() && qobject_cast<const QComboBox *>(widget))
    {
        QIcon::Mode mode;

        if ((comboBoxOption->state & QStyle::State_Selected) && (comboBoxOption->state & QStyle::State_Active))
        {
            mode = QIcon::Selected;
        }
        else if (comboBoxOption->state & QStyle::State_Enabled)
        {
            mode = QIcon::Normal;
        }
        else
        {
            mode = QIcon::Disabled;
        }

        QPixmap pixmap = comboBoxOption->currentIcon.pixmap(comboBoxOption->iconSize, mode);
        QRect iconRect(editRect);
        iconRect.setWidth(comboBoxOption->iconSize.width() + 4);
        iconRect = QStyle::alignedRect(comboBoxOption->direction,
                                       Qt::AlignLeft | Qt::AlignVCenter,
                                       iconRect.size(), editRect);

        if (comboBoxOption->editable)
        {
            painter->fillRect(iconRect, option->palette.brush(QPalette::Base));
        }

        style->drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);

        if (comboBoxOption->direction == Qt::RightToLeft)
        {
            editRect.translate(-4 - comboBoxOption->iconSize.width(), 0);
        }
        else
        {
            editRect.translate(comboBoxOption->iconSize.width() + 4, 0);
        }
    }

    // 绘制文本
    if (!comboBoxOption->currentText.isEmpty() && !comboBoxOption->editable)
    {
        QRect itemTextRect = editRect.adjusted(Metrics::ComboBox_FrameWidth, 0, -1, 0);
        int itemTextFlags = QStyle::visualAlignment(comboBoxOption->direction, Qt::AlignLeft | Qt::AlignVCenter);
        bool itemTextEnable = comboBoxOption->state & QStyle::State_Enabled;
        style->drawItemText(painter, itemTextRect, itemTextFlags, comboBoxOption->palette, itemTextEnable, comboBoxOption->currentText);
    }

    painter->restore();

    return true;
}
}  // namespace Kiran
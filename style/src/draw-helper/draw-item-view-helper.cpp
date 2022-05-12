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
#include "draw-item-view-helper.h"
#include "scheme-loader-fetcher.h"

#include <QIcon>
#include <QPainter>
#include <QStyleOption>

bool Kiran::Style::drawControlHeaderSection(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const QRect &rect(option->rect);
    const QPalette &palette(option->palette);
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));

    const auto *headerOption(qstyleoption_cast<const QStyleOptionHeader *>(option));
    if (!headerOption)
    {
        return true;
    }

    bool horizontal(headerOption->orientation == Qt::Horizontal);
    bool isFirst(horizontal && (headerOption->position == QStyleOptionHeader::Beginning));
    bool isCorner(widget && widget->inherits("QTableCornerButton"));
    bool reverseLayout(option->direction == Qt::RightToLeft);

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();

    // outline
    painter->setBrush(Qt::NoBrush);
    auto outlineColor = schemeLoader->getColor(widget, option, SchemeLoader::ItemView_Branch);
    painter->setPen(outlineColor);

    if (isCorner)
    {
        if (reverseLayout)
        {
            painter->drawPoint(rect.bottomLeft());
        }
        else
        {
            painter->drawPoint(rect.bottomRight());
        }
    }
    else if (horizontal)
    {
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    }
    else
    {
        if (reverseLayout)
        {
            painter->drawLine(rect.topLeft(), rect.bottomLeft());
        }
        else
        {
            painter->drawLine(rect.topRight(), rect.bottomRight());
        }
    }

    // separators
    if (horizontal)
    {
        if (headerOption->section != 0 || isFirst)
        {
            if (reverseLayout)
            {
                painter->drawLine(rect.topLeft(), rect.bottomLeft() - QPoint(0, 1));
            }
            else
            {
                painter->drawLine(rect.topRight(), rect.bottomRight() - QPoint(0, 1));
            }
        }
    }
    else
    {
        if (reverseLayout)
        {
            painter->drawLine(rect.bottomLeft() + QPoint(1, 0), rect.bottomRight());
        }
        else
        {
            painter->drawLine(rect.bottomLeft(), rect.bottomRight() - QPoint(1, 0));
        }
    }

    return true;
}

bool Kiran::Style::drawControlHeaderLabel(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option))
    {
        QRect rect = header->rect;
        if (!header->icon.isNull())
        {
            QPixmap pixmap = header->icon.pixmap(style->pixelMetric(QStyle::PM_SmallIconSize), (header->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled);
            int pixw = pixmap.width();

            QRect aligned = QStyle::alignedRect(header->direction, QFlag(header->iconAlignment), pixmap.size(), rect);
            QRect inter = aligned.intersected(rect);
            painter->drawPixmap(inter.x(), inter.y(), pixmap, inter.x() - aligned.x(), inter.y() - aligned.y(), inter.width(), inter.height());

            if (header->direction == Qt::LeftToRight)
                rect.setLeft(rect.left() + pixw + 2);
            else
                rect.setRight(rect.right() - pixw - 2);
        }

        QFont fnt = painter->font();
        painter->setFont(fnt);
        QPalette palette(header->palette);
        style->drawItemText(painter, rect, header->textAlignment, palette, (header->state & QStyle::State_Active), header->text, QPalette::Text);
    }
    return true;
}

bool Kiran::Style::drawControlHeaderEmptyArea(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    // use the same background as in drawHeaderPrimitive
    const QRect &rect(option->rect);
    QPalette palette(option->palette);

    bool horizontal(option->state & QStyle::State_Horizontal);
    bool reverseLayout(option->direction == Qt::RightToLeft);

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();

    // fill
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setBrush(palette.color(QPalette::Base));
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);

    // outline
    auto outlineColor = schemeLoader->getColor(widget,option,SchemeLoader::ItemView_Branch);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(outlineColor);

    if (horizontal)
    {
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    }
    else
    {
        if (reverseLayout)
        {
            painter->drawLine(rect.topLeft(), rect.bottomLeft());
        }
        else
        {
            painter->drawLine(rect.topRight(), rect.bottomRight());
        }
    }

    return true;
}

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
#include "draw-menu-helper.h"
#include "render-helper.h"
#include "scheme-loader-fetcher.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QWidget>

bool Kiran::Style::drawControlMenuBarItem(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    auto menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>(option);
    const QStyle::State& state = option->state;
    const bool enabled(option->state & QStyle::State_Enabled);
    const bool mouseOver((option->state & QStyle::State_MouseOver) && enabled);
    const bool sunken((option->state & QStyle::State_Sunken) && enabled);

    Kiran::Style::PainterSaver painterSaver(painter);
    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();

#if 0
    if(mouseOver && sunken)
    {
        auto background = schemeLoader->getColor(widget,option,SchemeLoader::MenuBar_ItemBackground);
        painter->setRenderHints(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRoundedRect(option->rect,4,4);
    }
#endif
    //TODO: menubaritem 和 menubar背景存在差异
    if( menuItemOption )
    {
        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

        if( !style->styleHint(QStyle::SH_UnderlineShortcut,option,widget) )
            alignment |= Qt::TextHideMnemonic;

        int iconExtent = style->pixelMetric(QStyle::PM_SmallIconSize);

        QPixmap pix = menuItemOption->icon.pixmap(widget?widget->window()->windowHandle(): nullptr,QSize(iconExtent,iconExtent),enabled?(mouseOver?QIcon::Active:QIcon::Normal):QIcon::Disabled);

        if( !pix.isNull() )
        {
            style->drawItemPixmap(painter,option->rect,alignment,pix);
        }
        else
        {
            QStyleOptionMenuItem itemOption = *menuItemOption;
            auto background = schemeLoader->getColor(widget,option,SchemeLoader::MenuBar_ItemBackground);
            painter->fillRect(option->rect,background);
            style->drawItemText(painter,itemOption.rect,alignment,itemOption.palette,enabled,itemOption.text,QPalette::ButtonText);
        }
    }
    return true;
}

bool Kiran::Style::drawControlMenuBarEmptyArea(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    QColor background = schemeLoader->getColor(widget,option,SchemeLoader::MenuBar_EmptyAreaBackground);
    painter->fillRect(option->rect,background);
    return true;
}

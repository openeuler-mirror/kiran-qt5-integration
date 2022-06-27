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
#include "draw-common-helper.h"
#include "render-helper.h"

#include "scheme-loader-fetcher.h"
#include "style-palette.h"

#define private public
#include <private/qsvgtinydocument_p.h>
#undef private

#include <QPainter>
#include <QRect>
#include <QStyle>
#include <QStyleOption>

namespace Kiran
{
bool drawPEFrame(const QStyle *style,
                               const QStyleOption *option,
                               QPainter *painter,
                               const QWidget *widget)
{
    QColor border,background;

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    background = schemeLoader->getColor(widget,option,SchemeLoader::Frame_Background);
    border = schemeLoader->getColor(widget,option,SchemeLoader::Frame_Border);

    RenderHelper::renderFrame(painter, option->rect, 1, 0, background,border );
    return true;
}

bool drawPEFrameFocusRect(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    //TODO:聚焦虚线框 颜色? 增加配置文件聚焦虚线框可配置
    return true;
}

bool drawPEFrameGroupBox(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto frameOption(qstyleoption_cast<const QStyleOptionFrame *>(option));
    if (!frameOption) return true;

    // no frame for flat groupboxes
    if (frameOption->features & QStyleOptionFrame::Flat) return true;

    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto background = schemeLoader->getColor(widget,option,SchemeLoader::Frame_Background);
    auto border = schemeLoader->getColor(widget,option,SchemeLoader::Frame_Border);

    RenderHelper::renderFrame(painter, option->rect,1, 0, background, border);
    return true;
}

bool drawControlShapedFrame(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    const auto *frameOpt = qstyleoption_cast<const QStyleOptionFrame *>(option);
    if (!frameOpt)
    {
        return false;
    }

    switch (frameOpt->frameShape)
    {
    case QFrame::Box:
    {
        if (option->state & QStyle::State_Sunken)
        {
            return true;
        }
        else
        {
            break;
        }

    }
    case QFrame::HLine:
    case QFrame::VLine:
    {
        const QRect &rect(option->rect);
        bool isVertical(frameOpt->frameShape == QFrame::VLine);

        QColor color = StylePalette::instance()->color(widget,option, StylePalette::Window, StylePalette::Border);
        RenderHelper::renderSeparator(painter, option->rect, isVertical, color);
        return true;
    }
    case QFrame::StyledPanel:
    {
        if (RenderHelper::isQtQuickControl(option, widget))
        {
            style->drawPrimitive(QStyle::PE_FrameMenu,option,painter,widget);
            return true;
        }
        break;
    }
    default:
        break;
    }

    return false;
}

QSize defaultSizeFromContents(const QStyleOption* option, const QSize &contentSize, const QWidget *widget)
{
    return contentSize;
}
}

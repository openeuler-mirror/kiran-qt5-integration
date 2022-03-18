//
// Created by lxh on 2021/12/20.
//

#include "draw-common-helper.h"
#include "render-helper.h"
#include "scheme-loader.h"

#define private public
#include <private/qsvgtinydocument_p.h>
#undef private

#include <QPainter>
#include <QRect>
#include <QStyle>
#include <QStyleOption>

using namespace Kiran::Style;


bool Kiran::Style::drawPEFrame(const QStyle *style,
                               const QStyleOption *option,
                               QPainter *painter,
                               const QWidget *widget,
                               SchemeLoader *scheme)
{
    QColor border,background;
    if( widget && widget->inherits("QAbstractItemView") )
    {
        border = scheme->getColor(widget, option, SchemeLoader::ItemView_BorderColor);
        background = scheme->getColor(widget, option, SchemeLoader::ItemView_BackgroundColor);
    }
    else
    {
        border = scheme->getColor(widget, option, SchemeLoader::Widget_BorderColor);
        background = scheme->getColor(widget, option, SchemeLoader::Widget_BackgroundColor);
    }
    RenderHelper::renderFrame(painter, option->rect, 1, 0, background,border );
    return true;
}

bool Kiran::Style::drawPEFrameFocusRect(const QStyle *style,
                          const QStyleOption *option,
                          QPainter *painter,
                          const QWidget *widget,
                          SchemeLoader *scheme)
{
    //TODO:聚焦虚线框 颜色?
    return true;
}

bool Kiran::Style::drawPEFrameGroupBox(const QStyle *style,
                         const QStyleOption *option,
                         QPainter *painter,
                         const QWidget *widget,
                         SchemeLoader *scheme)
{
    const auto frameOption(qstyleoption_cast<const QStyleOptionFrame *>(option));
    if (!frameOption) return true;

    // no frame for flat groupboxes
    if (frameOption->features & QStyleOptionFrame::Flat) return true;

    QColor backgroundColor = scheme->getColor(widget, option, SchemeLoader::Window_BackgroundColor);
    QColor borderColor = scheme->getColor(widget, option, SchemeLoader::Window_BorderColor);

    RenderHelper::renderFrame(painter, option->rect,1, 0, backgroundColor, borderColor);
    return true;
}

bool Kiran::Style::drawControlShapedFrame(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme)
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
        QColor color = scheme->getColor(widget, option, SchemeLoader::Widget_BorderColor);
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

QSize Kiran::Style::defaultSizeFromContents(const QStyleOption* option, const QSize &contentSize, const QWidget *widget)
{
    return contentSize;
}

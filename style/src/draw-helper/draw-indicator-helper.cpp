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
#include "scheme-loader.h"

#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QWidget>
#include <QSvgRenderer>
#include <QDebug>

using namespace Kiran::Style;

bool Kiran::Style::drawPEIndicatorButtonDropDown(const QStyle *style,
                                                 const QStyleOption *option,
                                                 QPainter *painter,
                                                 const QWidget *widget,
                                                 SchemeLoader *scheme)
{
    const auto toolButtonOption( qstyleoption_cast<const QStyleOptionToolButton*>( option ) );
    if( !toolButtonOption )
        return true;

    const QStyle::State& state( option->state );
    bool autoRaise( state & QStyle::State_AutoRaise );
    bool enabled( state & QStyle::State_Enabled );
    bool mouseOver( (state & QStyle::State_Active) && enabled && ( state & QStyle::State_MouseOver ) );
    bool sunken( enabled && ( state & QStyle::State_Sunken ) );

    // do nothing for autoraise buttons
    if( (autoRaise && !sunken && !mouseOver) || !(toolButtonOption->subControls & QStyle::SC_ToolButtonMenu) )
        return true;

    const QPalette& palette( option->palette );
    const QRect& rect( option->rect );

    QColor borderColor = scheme->getColor(widget,option,SchemeLoader::Button_BorderColor);
    QColor backgroundColor = scheme->getColor(widget,option,SchemeLoader::Button_BackgroundColor);

    QRect frameRect( rect );
    painter->setClipRect( rect );
    frameRect.adjust( -Metrics::Frame_FrameRadius - 1, 0, 0, 0 );
    frameRect = QStyle::visualRect( option->direction,option->rect, frameRect );

    // render
    RenderHelper::renderFrame(painter, frameRect, 1, 4, backgroundColor, borderColor);

    // also render separator
    QRect separatorRect( rect.adjusted( 0, 2, -2, -2 ) );
    separatorRect.setWidth( 1 );
    separatorRect = QStyle::visualRect( option->direction,option->rect, separatorRect );
    QColor separatorColor = scheme->getColor(SchemeLoader::Widget_BorderColor, enabled ? PseudoClass_Unspecified : PseudoClass_Unspecified | PseudoClass_Disabled);
    RenderHelper::renderSeparator(painter, separatorRect, true, separatorColor);

    return true;
}

bool drawPEIndicatorArrow(ArrowOrientation orientation,
                          const QStyleOption *option,
                          QPainter *painter,
                          const QWidget *widget,
                          SchemeLoader* scheme)
{
    quint64 specifiedClass = (option->state & QStyle::State_Enabled)?PseudoClass_Unspecified:PseudoClass_Disabled;
    QColor color = scheme->getColor(SchemeLoader::Widget_ForegroundColor, specifiedClass);
    RenderHelper::renderArrow(painter, option->rect, orientation, color);
    return true;
}

bool Kiran::Style::drawPEIndicatorArrowUp(const QStyle *style,
                                          const QStyleOption *option,
                                          QPainter *painter,
                                          const QWidget *widget,
                                          SchemeLoader *scheme)
{
    return drawPEIndicatorArrow(Arrow_Up,option,painter,widget,scheme);
}

bool Kiran::Style::drawPEIndicatorArrowDown(const QStyle *style,
                                            const QStyleOption *option,
                                            QPainter *painter,
                                            const QWidget *widget,
                                            SchemeLoader *scheme)
{
    return drawPEIndicatorArrow(Arrow_Down,option,painter,widget,scheme);
}

bool Kiran::Style::drawPEIndicatorArrowLeft(const QStyle *style,
                                            const QStyleOption *option,
                                            QPainter *painter,
                                            const QWidget *widget,
                                            SchemeLoader *scheme)
{
    return drawPEIndicatorArrow(Arrow_Left,option,painter,widget,scheme);
}

bool Kiran::Style::drawPEIndicatorArrowRight(const QStyle *style,
                                             const QStyleOption *option,
                                             QPainter *painter,
                                             const QWidget *widget,
                                             SchemeLoader *scheme)
{
    return drawPEIndicatorArrow(Arrow_Right,option,painter,widget,scheme);
}

bool Kiran::Style::drawPEIndicatorRadioButton(const QStyle *style,
                                              const QStyleOption *option,
                                              QPainter *painter,
                                              const QWidget *widget,
                                              SchemeLoader *scheme)
{
    const auto &rect(option->rect);
    QString iconUrl = scheme->getUrl(widget, option, SchemeLoader::RadioButton_IndicatorIcon);
    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing);
    QSvgRenderer renderer(iconUrl);
    renderer.render(painter, rect);
    painter->restore();
    return true;
}

bool Kiran::Style::drawPEIndicatorCheckBox(const QStyle *style,
                                           const QStyleOption *option,
                                           QPainter *painter,
                                           const QWidget *widget,
                                           SchemeLoader *scheme)
{
    const auto &rect(option->rect);
    QString iconUrl = scheme->getUrl(widget, option, SchemeLoader::CheckBox_IndicatorIcon);
    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing);
    QSvgRenderer renderer(iconUrl);
    renderer.render(painter, rect);
    painter->restore();
    return true;
}

bool Kiran::Style::drawPEIndicatorBranch(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme)
{
    const QRect &rect(option->rect);
    const QPalette &palette(option->palette);

    const QStyle::State &state(option->state);
    bool reverseLayout(option->direction == Qt::RightToLeft);

    //绘制扩展的箭头
    int expanderAdjust = 0;
    if (state & QStyle::State_Children) {
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
        if (expanderOpen) {
            orientation = Arrow_Down;
        } else if (reverseLayout) {
            orientation = Arrow_Left;
        } else {
            orientation = Arrow_Right;
        }

        QColor arrowColor = scheme->getColor(widget,option,SchemeLoader::ItemView_ArrowColor);
        RenderHelper::renderArrow(painter,arrowRect,orientation,arrowColor);
    }

    //TODO: 是否绘制树的分支线开关
//    // tree branches
//    if (!Adwaita::Config::ViewDrawTreeBranchLines) {
//        return true;
//    }

    QPoint center(rect.center());
    QColor lineColor = scheme->getColor(widget,option,SchemeLoader::ItemView_BranchLineColor);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(0.5, 0.5);
    painter->setPen(QPen(lineColor, 1));
    if (state & (QStyle::State_Item | QStyle::State_Children | QStyle::State_Sibling)) {
        QLineF line(QPointF(center.x(), rect.top()), QPointF(center.x(), center.y() - expanderAdjust - 1));
        painter->drawLine(line);
    }

    //如果存在Item则绘制左/右取决与方向的线
    if (state & QStyle::State_Item) {
        const QLineF line = reverseLayout ?
                            QLineF(QPointF(rect.left(), center.y()), QPointF(center.x() - expanderAdjust, center.y())) :
                            QLineF(QPointF(center.x() + expanderAdjust, center.y()), QPointF(rect.right(), center.y()));
        painter->drawLine(line);
    }

    // 如果存在同级节点的话，则绘制上下的线
    if (state & QStyle::State_Sibling) {
        QLineF line(QPointF(center.x(), center.y() + expanderAdjust), QPointF(center.x(), rect.bottom()));
        painter->drawLine(line);
    }
    painter->restore();

    return true;
}

bool Kiran::Style::drawPEIndicatorToolBarSeparator(const QStyle *style, const QStyleOption *option,
                                                   QPainter *painter, const QWidget *widget,
                                                   SchemeLoader *scheme)
{
    const QStyle::State & state = option->state;
    bool enable = (state & QStyle::State_Enabled);
    bool separatorIsVertical( state & QStyle::State_Horizontal );

    QColor separatorColor = scheme->getColor(SchemeLoader::ItemView_BranchLineColor,enable?PseudoClass_Unspecified:PseudoClass_Disabled);
    RenderHelper::renderSeparator(painter,option->rect,separatorIsVertical,separatorColor);
    return true;
}

bool Kiran::Style::drawPEIndicatorToolBarHandle(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget, SchemeLoader *scheme)
{
    const QStyle::State& state = option->state;
    QRect rect( option->rect );
    bool separatorIsVertical( state & QStyle::State_Horizontal );
    bool enabled(state & QStyle::State_Enabled);

    QColor separatorColor = scheme->getColor(SchemeLoader::ItemView_BranchLineColor,enabled?PseudoClass_Unspecified:PseudoClass_Disabled);
    if( separatorIsVertical )
    {
        rect.setWidth( Metrics::ToolBar_HandleWidth );
        rect = RenderHelper::centerRect( option->rect, rect.size() );
        rect.setWidth( 3 );
        RenderHelper::renderSeparator( painter, rect, separatorIsVertical,separatorColor );

        rect.translate( 2, 0 );
        RenderHelper::renderSeparator( painter, rect, separatorIsVertical,separatorColor );

    } else {

        rect.setHeight( Metrics::ToolBar_HandleWidth );
        rect = RenderHelper::centerRect( option->rect, rect.size() );
        rect.setHeight( 3 );
        RenderHelper::renderSeparator( painter, rect, separatorIsVertical,separatorColor );

        rect.translate( 0, 2 );
        RenderHelper::renderSeparator( painter, rect, separatorIsVertical,separatorColor );

    }

    return true;

}

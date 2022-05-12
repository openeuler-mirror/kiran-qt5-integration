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
#include "draw-tab-widget-helper.h"
#include "kiran-palette.h"
#include "draw-common-helper.h"
#include "metrics.h"
#include "render-helper.h"
#include "scheme-loader-fetcher.h"

#include <QColor>
#include <QStyleOption>
#include <QPainter>
#include <QTabBar>
#include <QDebug>

//FIXME:需要和sizeFromContents获取的大小对应

using namespace Kiran::Style;

void tabLayout(const QStyle *style, const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect, QRect *iconRect)
{
    QRect tr = opt->rect;
    bool verticalTabs = RenderHelper::isVerticalTab(opt->shape);

    if (verticalTabs)
        tr.setRect(0, 0, tr.height(), tr.width()); // 0, 0 as we will have a translate transform

    /// 垂直和水平偏移量,以突出当前选中的Tab标签
    int verticalShift = style->pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt, widget);
    int horizontalShift = style->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt, widget);

    int hpadding = style->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2;
    int vpadding = style->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;

    if (opt->shape == QTabBar::RoundedSouth || opt->shape == QTabBar::TriangularSouth)
        verticalShift = -verticalShift;

    tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);
    bool selected = opt->state & QStyle::State_Selected;
    if (selected) {
        tr.setTop(tr.top() - verticalShift);
        tr.setRight(tr.right() - horizontalShift);
    }

    // left widget
    if (!opt->leftButtonSize.isEmpty()) {
        tr.setLeft(tr.left() + 4 +
            (verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
    }
    // right widget
    if (!opt->rightButtonSize.isEmpty()) {
        tr.setRight(tr.right() - 4 -
            (verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));
    }

    // icon
    if (!opt->icon.isNull()) {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid()) {
            int iconExtent = style->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize = opt->icon.actualSize(iconSize,
                                                 (opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                                                 (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);
        // High-dpi icons do not need adjustment; make sure tabIconSize is not larger than iconSize
        tabIconSize = QSize(qMin(tabIconSize.width(), iconSize.width()), qMin(tabIconSize.height(), iconSize.height()));

        *iconRect = QRect(tr.left(), tr.center().y() - tabIconSize.height() / 2,
                          tabIconSize.width(), tabIconSize.height());
        if (!verticalTabs)
            *iconRect = style->visualRect(opt->direction, opt->rect, *iconRect);
        tr.setLeft(tr.left() + tabIconSize.width() + 4);
    }

    if (!verticalTabs)
        tr = style->visualRect(opt->direction, opt->rect, tr);

    *textRect = tr;
}

QSize Kiran::Style::tabBarTabSizeFromContents(const QStyle* style, const QStyleOption* option, const QSize& contentSize, const QWidget* widget)
{
    const auto tabOption(qstyleoption_cast<const QStyleOptionTab *>(option));
    const bool hasText(tabOption && !tabOption->text.isEmpty());
    const bool hasIcon(tabOption && !tabOption->icon.isNull());
    const bool hasLeftButton(tabOption && !tabOption->leftButtonSize.isEmpty());
    const bool hasRightButton(tabOption && !tabOption->leftButtonSize.isEmpty());

    // calculate width increment for horizontal tabs
    int widthIncrement = 0;
    if (hasIcon && !(hasText || hasLeftButton || hasRightButton)) widthIncrement -= 4;
    if (hasText && hasIcon) widthIncrement += Metrics::TabBar_TabItemSpacing;
    if (hasLeftButton && (hasText || hasIcon)) widthIncrement += Metrics::TabBar_TabItemSpacing;
    if (hasRightButton && (hasText || hasIcon || hasLeftButton)) widthIncrement += Metrics::TabBar_TabItemSpacing;

    // add margins
    QSize size(contentSize);

    // compare to minimum size
    const bool verticalTabs(tabOption && RenderHelper::isVerticalTab(tabOption->shape));
    if (verticalTabs)
    {
        size.rheight() += widthIncrement;
        if (hasIcon && !hasText)
            size = size.expandedTo(QSize(Metrics::TabBar_TabMinHeight, 0));
        else
            size = size.expandedTo(QSize(Metrics::TabBar_TabMinHeight, Metrics::TabBar_TabMinWidth));
    }
    else
    {
        size.rwidth() += widthIncrement;
        if (hasIcon && !hasText)
            size = size.expandedTo(QSize(0, Metrics::TabBar_TabMinHeight));
        else
            size = size.expandedTo(QSize(Metrics::TabBar_TabMinWidth, Metrics::TabBar_TabMinHeight));
    }

    return size;
}

bool Kiran::Style::drawPEFrameTabWidget(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    auto schemeLoader = SchemeLoaderFetcher::getSchemeLoader();
    auto background = schemeLoader->getColor(widget,option,SchemeLoader::Frame_Background);
    auto border = schemeLoader->getColor(widget,option,SchemeLoader::Frame_Border);
    RenderHelper::renderFrame(painter, option->rect,1, 0, background, border);
    return true;
}

bool Kiran::Style::drawPEFrameTabBarBase(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    return true;
}

bool Kiran::Style::drawControlTabBarTabLabel(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    return false;
}

bool Kiran::Style::drawControlTabBarTabShape(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    const auto tabOption(qstyleoption_cast<const QStyleOptionTab *>(option));
    if (!tabOption) return true;

    const QPalette &palette(option->palette);
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool selected(state & QStyle::State_Selected);
    bool mouseOver((state & QStyle::State_Active) && !selected && (state & QStyle::State_MouseOver) && enabled);

    // check if tab is being dragged
    bool isDragged(widget && selected && painter->device() != widget);
    //bool isLocked(widget && _tabBarData->isLocked(widget));

    const QStyleOptionTab::TabPosition &position = tabOption->position;
    const bool isSingle(position == QStyleOptionTab::OnlyOneTab);
    const bool isQtQuickControl(RenderHelper::isQtQuickControl(option, widget));
    bool isFirst(isSingle || position == QStyleOptionTab::Beginning);
    bool isLast(isSingle || position == QStyleOptionTab::End);
    bool isLeftOfSelected(/*!isLocked &&*/ tabOption->selectedPosition == QStyleOptionTab::NextIsSelected);
    bool isRightOfSelected(/*!isLocked &&*/ tabOption->selectedPosition == QStyleOptionTab::PreviousIsSelected);

    // true if widget is aligned to the frame
    // need to check for 'isRightOfSelected' because for some reason the isFirst flag is set when active tab is being moved
    isFirst &= !isRightOfSelected;
    isLast &= !isLeftOfSelected;

    // 若是反向布局交换状态，改变与方向相关属性
    const bool reverseLayout(option->direction == Qt::RightToLeft);
    const bool verticalTabs(RenderHelper::isVerticalTab(tabOption->shape));
    if (reverseLayout && !verticalTabs)
    {
        qSwap(isFirst, isLast);
        qSwap(isLeftOfSelected, isRightOfSelected);
    }

    //根据TabBar方向调整选项卡圆角的位置
    Corners corners;
    switch (tabOption->shape)
    {
    case QTabBar::RoundedNorth:
    case QTabBar::TriangularNorth:
        corners = CornersTop;
        break;
    case QTabBar::RoundedSouth:
    case QTabBar::TriangularSouth:
        corners = CornersBottom;
        break;
    case QTabBar::RoundedWest:
    case QTabBar::TriangularWest:
        corners = CornersLeft;
        break;
    case QTabBar::RoundedEast:
    case QTabBar::TriangularEast:
        corners = CornersRight;
        break;
    default:
        break;
    }

//    quint64 specialPseudoClass = PseudoClass_Unspecified;
//    if (selected)
//    {
//        specialPseudoClass = PseudoClass_Pressed;
//    }
//    QColor backgroundColor = scheme->getColor(widget, option, SchemeLoader::Button_BackgroundColor, specialPseudoClass);
    QStyleOption tempOption(*option);
    if( selected )
    {
        tempOption.state |= QStyle::State_Sunken;
    }
    QColor backgroundColor = KiranPalette::instance()->color(widget,&tempOption,KiranPalette::Widget,KiranPalette::Background);
    RenderHelper::renderTabBarTab(painter, option->rect, corners,4, backgroundColor );

    return true;
}

QRect Kiran::Style::tabBarSubElementRect(const QStyle* style, QStyle::SubElement subElement, const QStyleOption* option, const QWidget* widget)
{
    const auto *tabOption(qstyleoption_cast<const QStyleOptionTab *>(option));

    if(  !widget || !widget->inherits("QTabBar") )
    {
        return QRect();
    }

    const auto tabBar = qobject_cast<const QTabBar*>(widget);
    const QRect& rect = option->rect;

    switch (subElement)
    {
    case QStyle::SE_TabBarTearIndicator:
    case QStyle::SE_TabBarTearIndicatorRight:
        return QRect();

    //QTabBar::setTabButton所设置位于单个Tab左右的控件
    case QStyle::SE_TabBarTabLeftButton:
    case QStyle::SE_TabBarTabRightButton:
    {
        if( !tabOption )
        {
            return QRect();
        }

        bool isLeft = (subElement == QStyle::SE_TabBarTabLeftButton);

        QSize size( isLeft?tabOption->leftButtonSize:tabOption->rightButtonSize );
        if( size.isEmpty() )
        {
            return QRect();
        }

        QRect buttonRect( QPoint(0,0), size );

        switch( tabOption->shape )
        {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:

        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            if( isLeft)
            {
                buttonRect.moveLeft( rect.left() + Metrics::TabBar_TabMarginWidth );
            }
            else
            {
                buttonRect.moveRight( rect.right() - Metrics::TabBar_TabMarginWidth );
            }
            buttonRect.moveTop( ( rect.height() - buttonRect.height() )/2 );
            buttonRect = QStyle::visualRect( option->direction,option->rect, buttonRect );
            break;

        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            if(isLeft)
            {
                buttonRect.moveBottom( rect.bottom() - Metrics::TabBar_TabMarginWidth );
            }
            else
            {
                buttonRect.moveTop( rect.top() + Metrics::TabBar_TabMarginWidth );
            }
            buttonRect.moveLeft( ( rect.width() - buttonRect.width() )/2 );
            break;

        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            if(isLeft)
            {
                buttonRect.moveTop( rect.top() + Metrics::TabBar_TabMarginWidth );
            }
            else
            {
                buttonRect.moveBottom( rect.bottom() - Metrics::TabBar_TabMarginWidth );
            }
            buttonRect.moveLeft( ( rect.width() - buttonRect.width() )/2 );
            break;

        default: break;
        }

        return buttonRect;
    }

    case QStyle::SE_TabBarTabText:
    {
        QRect textRect;
        QRect iconRect;
        tabLayout(style, tabOption, widget, &textRect, &iconRect);
        return textRect;
    }

    //当QTabBar过多时，QTabBar左右滑动的按钮
    case QStyle::SE_TabBarScrollLeftButton:
    case QStyle::SE_TabBarScrollRightButton:
    {
        bool isLeft = (subElement == QStyle::SE_TabBarScrollLeftButton);
        const int buttonWidth = style->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, nullptr,widget);
        QRect scrollButtonRect ;

        if(RenderHelper::isVerticalTab(tabBar->shape()) )
        {
            scrollButtonRect = QRect(0,isLeft?0:(rect.height()-buttonWidth),rect.width(),buttonWidth);
        }
        else
        {
            scrollButtonRect = QRect(isLeft?0:(rect.width()-buttonWidth),0,buttonWidth,rect.height());
            QStyle::visualRect(option->direction,option->rect,scrollButtonRect);
        }
        return scrollButtonRect;
    }
    default:
        break;
    }

    return QRect();
}

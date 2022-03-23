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
#include "draw-button-helper.h"
#include "kiran-palette.h"
#include "define.h"
#include "draw-common-helper.h"
#include "metrics.h"
#include "render-helper.h"

#include <QAbstractButton>
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QToolButton>
#include <QWidget>

using namespace Kiran::Style;

//根据按钮内容获取按钮尺寸
QSize Kiran::Style::pushButtonSizeFromContents(const QStyle* style,
                                               const QStyleOption* option,
                                               const QSize& contentSize,
                                               const QWidget* widget)
{
    const auto buttonOption(qstyleoption_cast<const QStyleOptionButton*>(option));
    if (!buttonOption) return contentSize;

    QSize size;

    const bool hasText(!buttonOption->text.isEmpty());
    const bool flat(buttonOption->features & QStyleOptionButton::Flat);
    bool hasIcon(!buttonOption->icon.isNull());

    if (!(hasText || hasIcon))  //没有文本以及图标，采用自定义按钮作为内容大小的起点
    {
        size = contentSize;
    }
    else
    {  //不管Qt在内容大小如何计算，完全根据按钮选项重新计算按钮大小,保证渲染阶段的一致性
        hasIcon &= (RenderHelper::showIconsOnPushButtons() || flat || !hasText);

        //文本大小
        if (hasText) size = buttonOption->fontMetrics.size(Qt::TextShowMnemonic, buttonOption->text);

        //图标大小
        if (hasIcon)
        {
            QSize iconSize = buttonOption->iconSize;
            if (!iconSize.isValid()) iconSize = QSize(style->pixelMetric(QStyle::PM_SmallIconSize, option, widget), style->pixelMetric(QStyle::PM_SmallIconSize, option, widget));

            size.setHeight(qMax(size.height(), iconSize.height()));
            size.rwidth() += iconSize.width();

            if (hasText) size.rwidth() += Metrics::Button_ItemSpacing;
        }
    }

    //菜单
    const bool hasMenu(buttonOption->features & QStyleOptionButton::HasMenu);
    if (hasMenu)
    {
        size.rwidth() += Metrics::MenuButton_IndicatorWidth;
        if (hasText || hasIcon) size.rwidth() += Metrics::Button_ItemSpacing;
    }

    //扩展按钮内边距
    size = RenderHelper::expandSize(size, Metrics::Button_MarginWidth, Metrics::Button_MarginHeight);

    //扩张按钮边框宽度
    size = RenderHelper::expandSize(size, Metrics::Frame_FrameWidth);

    //确保按钮有最小的宽度
    if (hasText)
    {
        size.setWidth(qMax(size.width(), int(Metrics::Button_MinWidth)));
    }

    //确保按钮有最小高度
    size.setHeight(qMax(size.height(), int(Metrics::Button_MinHeight)));

    return size;
}

//根据ToolButton内容大小获取尺寸
QSize Kiran::Style::toolButtonSizeFromContents(const QStyle* style,
                                               const QStyleOption* option,
                                               const QSize& contentSize,
                                               const QWidget* widget)
{
    const auto toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton*>(option);
    if (!toolButtonOption) return contentSize;

    QSize size = contentSize;

    const QStyle::State& state(option->state);

    //Auto Raise标志： true表示自动突出，false表示与父窗口齐平
    const bool autoRaise(state & QStyle::State_AutoRaise);
    const bool hasPopupMenu(toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup);
    const bool hasInlineIndicator(toolButtonOption->features & QStyleOptionToolButton::HasMenu && toolButtonOption->features & QStyleOptionToolButton::PopupDelay && !hasPopupMenu);

    //若存在内部指示器，宽度加上一个内部指示器的宽度
    if (hasInlineIndicator) size.rwidth() += Metrics::ToolButton_InlineIndicatorWidth;

    //总边距宽度=按钮边距宽度+Frame宽度
    int marginWidth = Metrics::Button_MarginWidth + Metrics::Frame_FrameWidth;
    //四周扩展边距宽度
    size = RenderHelper::expandSize(size, marginWidth);

    return size;
}

bool Kiran::Style::toolButtonSubControlRect(const QStyle* style,
                                            const QStyleOptionComplex* opt,
                                            QStyle::SubControl sc,
                                            const QWidget* widget,
                                            QRect& controlRect)
{
    const auto toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton*>(opt);
    if (!toolButtonOption) return false;

    bool hasPopupMenu(toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup);
    const bool hasInlineIndicator(toolButtonOption->features & QStyleOptionToolButton::HasMenu &&
                                  toolButtonOption->features & QStyleOptionToolButton::PopupDelay &&
                                  !hasPopupMenu);

    const QRect& rect(opt->rect);
    int menuButtonWidth(Metrics::MenuButton_IndicatorWidth);

    switch (sc)
    {
    case QStyle::SC_ToolButtonMenu:
    {
        if (!(hasPopupMenu || hasInlineIndicator))
        {
            controlRect = QRect();
            break;
        }

        QRect menuRect(rect);
        //定位菜单按钮为ToolButton右侧
        menuRect.setLeft(rect.right() - menuButtonWidth + 1);
        if (hasInlineIndicator)
        {
            menuRect.setTop(menuRect.bottom() - menuButtonWidth);
        }
        controlRect = QStyle::visualRect(opt->direction, opt->rect, menuRect);
        break;
    }
    case QStyle::SC_ToolButton:
    {
        if (hasPopupMenu)
        {
            QRect contentsRect(rect);
            contentsRect.setRight(rect.right() - menuButtonWidth);
            controlRect = QStyle::visualRect(opt->direction, opt->rect, contentsRect);
        }
        else
        {
            controlRect = rect;
        }
        break;
    }
    default:
        return false;
    }

    return true;
}

bool Kiran::Style::drawPEPanelButtonCommand(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    const auto buttonOption(qstyleoption_cast<const QStyleOptionButton*>(option));
    if (!buttonOption)
        return true;

    const QRect& rect(option->rect);

    const QStyle::State& state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));
    bool hasFocus((enabled && (state & QStyle::State_HasFocus)) && !(widget && widget->focusProxy()));
    bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    bool flat(buttonOption->features & QStyleOptionButton::Flat);

    auto background = KiranPalette::instance()->color(widget,option,KiranPalette::Widget,KiranPalette::Background);
    //按钮不绘制聚焦边框,只考虑启用和禁用状态
    auto border = KiranPalette::instance()->color(enabled?KiranPalette::Normal:KiranPalette::Disabled,KiranPalette::Widget,KiranPalette::Border);

    if (flat)
    {
        if (!sunken)
        {
            background = Qt::transparent;
        }
    }

    RenderHelper::renderFrame(painter, option->rect,1, 4, background, border);
    return true;
}

bool Kiran::Style::drawPEPanelButtonTool(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    const QPalette& palette(option->palette);
    QRect rect(option->rect);

    const QStyle::State& state(option->state);
    bool autoRaise(state & QStyle::State_AutoRaise);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    bool mouseOver((state & QStyle::State_Active) && enabled && (option->state & QStyle::State_MouseOver));
    bool hasFocus(enabled && (option->state & (QStyle::State_HasFocus | QStyle::State_Sunken)));

    if (!autoRaise || mouseOver || sunken)
    {
        // need to check widget for popup mode, because option is not set properly
        const auto* toolButton(qobject_cast<const QToolButton*>(widget));
        bool hasPopupMenu(toolButton && toolButton->popupMode() == QToolButton::MenuButtonPopup);

        // adjust frame in case of menu
        if (hasPopupMenu)
        {
            painter->setClipRect(rect);
            rect.adjust(0, 0, Metrics::Frame_FrameRadius + 2, 0);
            rect = QStyle::visualRect(option->direction, option->rect, rect);
        }

        auto background = KiranPalette::instance()->color(widget,option,KiranPalette::Widget,KiranPalette::Background);
        //按钮不绘制聚焦边框,只考虑启用和禁用状态
        auto border = KiranPalette::instance()->color(enabled?KiranPalette::Normal:KiranPalette::Disabled,KiranPalette::Widget,KiranPalette::Border);
        RenderHelper::renderFrame(painter, rect,1, 4, background, border);
    }
    else
    {
        RenderHelper::renderFrame(painter, rect,1, 4, Qt::transparent);
    }

    return true;
}

bool Kiran::Style::drawControlToolButtonLabel(const QStyle* style, const QStyleOption* option, QPainter* painter, const QWidget* widget)
{
    const auto toolButtonOption(qstyleoption_cast<const QStyleOptionToolButton*>(option));
    if (!toolButtonOption)
        return false;

    const QRect& rect = option->rect;
    const QPalette& palette = option->palette;

    const QStyle::State& state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    bool mouseOver((state & QStyle::State_Active) && enabled && (option->state & QStyle::State_MouseOver));
    bool flat(state & QStyle::State_AutoRaise);

    bool hasFocus(false);
    if (flat)
        hasFocus = enabled && !mouseOver && (option->state & QStyle::State_HasFocus);
    else
        hasFocus = enabled && !mouseOver && (option->state & (QStyle::State_HasFocus | QStyle::State_Sunken));

    bool hasArrow = toolButtonOption->features & QStyleOptionToolButton::Arrow;
    bool hasIcon(!(hasArrow || toolButtonOption->icon.isNull()));
    bool hasText(!toolButtonOption->text.isEmpty());

    QRect contentsRect(rect);
    QSize iconSize(toolButtonOption->iconSize);
    int textFlags(RenderHelper::mnemonicsTextFlags());
    QSize textSize(option->fontMetrics.size(textFlags, toolButtonOption->text));

    QRect iconRect;
    QRect textRect;

    if (hasText && (!(hasArrow || hasIcon) || toolButtonOption->toolButtonStyle == Qt::ToolButtonTextOnly))  //只显示文字
    {
        textRect = contentsRect;
        textFlags |= Qt::AlignCenter;
    }
    else if ((hasArrow || hasIcon) && (!hasText || toolButtonOption->toolButtonStyle == Qt::ToolButtonIconOnly))  //只显示图标
    {
        iconRect = contentsRect;
    }
    else if (toolButtonOption->toolButtonStyle == Qt::ToolButtonTextUnderIcon)  //文字位于图标之下
    {
        int contentsHeight(iconSize.height() + textSize.height() + Metrics::ToolButton_ItemSpacing);
        iconRect = QRect(QPoint(contentsRect.left() + (contentsRect.width() - iconSize.width()) / 2, contentsRect.top() + (contentsRect.height() - contentsHeight) / 2), iconSize);
        textRect = QRect(QPoint(contentsRect.left() + (contentsRect.width() - textSize.width()) / 2, iconRect.bottom() + Metrics::ToolButton_ItemSpacing + 1), textSize);
        textFlags |= Qt::AlignCenter;
    }
    else
    {
        //        bool leftAlign(widget && widget->property(PropertyNames::toolButtonAlignment).toInt() == Qt::AlignLeft);
        //        if (leftAlign)
        //            iconRect = QRect(QPoint(contentsRect.left(), contentsRect.top() + (contentsRect.height() - iconSize.height()) / 2), iconSize);
        //        else
        {
            int contentsWidth(iconSize.width() + textSize.width() + Metrics::ToolButton_ItemSpacing);
            iconRect = QRect(QPoint(contentsRect.left() + (contentsRect.width() - contentsWidth) / 2, contentsRect.top() + (contentsRect.height() - iconSize.height()) / 2), iconSize);
        }

        textRect = QRect(QPoint(iconRect.right() + Metrics::ToolButton_ItemSpacing + 1, contentsRect.top() + (contentsRect.height() - textSize.height()) / 2), textSize);

        // handle right to left layouts
        iconRect = QStyle::visualRect(option->direction, option->rect, iconRect);
        textRect = QStyle::visualRect(option->direction, option->rect, textRect);

        textFlags |= Qt::AlignLeft | Qt::AlignVCenter;
    }

    // make sure there is enough room for icon
    if (iconRect.isValid()) iconRect = RenderHelper::centerRect(iconRect, iconSize);

    // render arrow or icon
    if (hasArrow && iconRect.isValid())
    {
        QStyleOptionToolButton copy(*toolButtonOption);
        copy.rect = iconRect;
        switch (toolButtonOption->arrowType)
        {
        case Qt::LeftArrow:
            style->drawPrimitive(QStyle::PE_IndicatorArrowLeft, &copy, painter, widget);
            break;
        case Qt::RightArrow:
            style->drawPrimitive(QStyle::PE_IndicatorArrowRight, &copy, painter, widget);
            break;
        case Qt::UpArrow:
            style->drawPrimitive(QStyle::PE_IndicatorArrowUp, &copy, painter, widget);
            break;
        case Qt::DownArrow:
            style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &copy, painter, widget);
            break;
        default:
            break;
        }
    }
    else if (hasIcon && iconRect.isValid())
    {
        // icon state and mode
        const QIcon::State iconState(sunken ? QIcon::On : QIcon::Off);
        QIcon::Mode iconMode;
        if (!enabled)
            iconMode = QIcon::Disabled;
        else if (!flat && hasFocus)
            iconMode = QIcon::Selected;
        else if (mouseOver && flat)
            iconMode = QIcon::Active;
        else
            iconMode = QIcon::Normal;

        QPixmap pixmap = toolButtonOption->icon.pixmap(iconSize, iconMode, iconState);
        style->drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);
    }

    // render text
    if (hasText && textRect.isValid())
    {
        QPalette::ColorRole textRole(QPalette::ButtonText);
        if (flat)
            textRole = (hasFocus && sunken && !mouseOver) ? QPalette::HighlightedText : QPalette::WindowText;
        else if (hasFocus && !mouseOver)
            textRole = QPalette::HighlightedText;

        painter->setFont(toolButtonOption->font);
        style->drawItemText(painter, textRect, textFlags, palette, enabled, toolButtonOption->text, textRole);
    }
    return true;
}

bool Kiran::Style::drawCCToolButton(const QStyle* style, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget)
{
    const auto* toolButtonOption(qstyleoption_cast<const QStyleOptionToolButton*>(option));
    if (!toolButtonOption) return true;

    const QStyle::State& state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool mouseOver((state & QStyle::State_Active) && enabled && (option->state & QStyle::State_MouseOver));
    bool hasFocus(enabled && (option->state & QStyle::State_HasFocus));
    bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    bool flat(state & QStyle::State_AutoRaise);

    bool isDockWidgetTitleButton(widget && widget->inherits("QDockWidgetTitleButton"));
    bool inTabBar(widget && qobject_cast<const QTabBar*>(widget->parentWidget()));
    bool isMenuTitle(RenderHelper::isMenuTitle(widget));

    if (isMenuTitle)
    {
        QStyleOptionToolButton copy(*toolButtonOption);
        copy.font.setBold(false);
        copy.state = QStyle::State_Enabled;

        auto separatorColor = KiranPalette::instance()->color(widget,option,KiranPalette::Widget,KiranPalette::Border);
        RenderHelper::renderMenuTitle(style, painter, &copy, widget, separatorColor);
        return true;
    }

    // copy option and alter palette
    QStyleOptionToolButton copy(*toolButtonOption);

    if (isDockWidgetTitleButton)
    {
        const QAbstractButton* button(qobject_cast<const QAbstractButton*>(widget));
        if (button->isChecked() || button->isDown())
        {
            copy.state |= QStyle::State_Enabled | QStyle::State_On | QStyle::State_Sunken;
        }
        if (button->underMouse())
        {
            copy.state |= QStyle::State_Enabled | QStyle::State_MouseOver | QStyle::State_Active;
        }
    }

    bool hasPopupMenu(toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup);
    const bool hasInlineIndicator(
        toolButtonOption->features & QStyleOptionToolButton::HasMenu && toolButtonOption->features & QStyleOptionToolButton::PopupDelay && !hasPopupMenu);

    QRect buttonRect(style->subControlRect(QStyle::CC_ToolButton, option, QStyle::SC_ToolButton, widget));
    QRect menuRect(style->subControlRect(QStyle::CC_ToolButton, option, QStyle::SC_ToolButtonMenu, widget));

    // frame
    if (toolButtonOption->subControls & QStyle::SC_ToolButton || isDockWidgetTitleButton)
    {
        copy.rect = buttonRect;
        if (inTabBar)
        {
            QRect rect(option->rect);

            auto background = KiranPalette::instance()->color(widget,option,KiranPalette::Widget,KiranPalette::Background);
            auto border = KiranPalette::instance()->color(widget,option,KiranPalette::Widget,KiranPalette::Border);

            painter->setPen(background);
            painter->setBrush(background);

            switch (toolButtonOption->arrowType)
            {
            case Qt::UpArrow:
                painter->drawRect(rect.adjusted(1, 1, -2, -1));
                break;
            case Qt::DownArrow:
                painter->drawRect(rect.adjusted(1, 0, -2, -2));
                break;
            case Qt::LeftArrow:
                painter->drawRect(rect.adjusted(1, 1, -1, -2));
                break;
            case Qt::RightArrow:
                painter->drawRect(rect.adjusted(0, 1, -2, -2));
                break;
            }

            painter->setPen(border);
            switch (toolButtonOption->arrowType)
            {
            case Qt::DownArrow:
                painter->drawLine(rect.bottomLeft(), rect.bottomRight());
                break;
            case Qt::RightArrow:
                painter->drawLine(rect.topRight(), rect.bottomRight());
                break;
            }
            switch (toolButtonOption->arrowType)
            {
            case Qt::UpArrow:
            case Qt::DownArrow:
                painter->drawLine(rect.topLeft(), rect.bottomLeft());
                painter->drawLine(rect.topLeft(), rect.bottomLeft());
                break;
            case Qt::LeftArrow:
            case Qt::RightArrow:
                painter->drawLine(rect.topLeft(), rect.topRight());
                painter->drawLine(rect.bottomLeft(), rect.bottomRight());
                break;
            }

        }
        else if (sunken && hasPopupMenu && !(toolButtonOption->activeSubControls & QStyle::SC_ToolButton))
        {
            // Only menu button is active. so draw left hand side od button raised
            QStyleOptionToolButton btn(copy);
            btn.state |= QStyle::State_Raised;
            btn.state &= ~QStyle::State_Sunken;
            btn.state &= ~QStyle::State_AutoRaise;
            style->drawPrimitive(QStyle::PE_PanelButtonTool, &btn, painter, widget);
        }
        else
        {
            style->drawPrimitive(QStyle::PE_PanelButtonTool, &copy, painter, widget);
        }
    }

    // arrow
    if (hasPopupMenu)
    {
        copy.rect = menuRect;

        if (!flat || mouseOver || sunken)
            style->drawPrimitive(QStyle::PE_IndicatorButtonDropDown, &copy, painter, widget);

        style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &copy, painter, widget);
    }

    else if (hasInlineIndicator)
    {
        copy.rect = menuRect;

        style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &copy, painter, widget);
    }

    // contents
    {
        // restore state
        copy.state = state;

        // define contents rect
        QRect contentsRect(buttonRect);

        // detect dock widget title button
        // for dockwidget title buttons, do not take out margins, so that icon do not get scaled down
        if (isDockWidgetTitleButton)
        {
            // cast to abstract button
            // adjust state to have correct icon rendered
            const QAbstractButton* button(qobject_cast<const QAbstractButton*>(widget));
            if (button->isChecked() || button->isDown())
            {
                copy.state |= QStyle::State_Enabled | QStyle::State_On | QStyle::State_Sunken;
            }
            if (button->underMouse())
            {
                copy.state |= QStyle::State_Enabled | QStyle::State_MouseOver | QStyle::State_Active;
            }
        }
        else if (!inTabBar && hasInlineIndicator)
        {
            int marginWidth(flat ? Metrics::ToolButton_MarginWidth : Metrics::Button_MarginWidth + Metrics::Frame_FrameWidth);
            contentsRect = RenderHelper::insideMargin(contentsRect, marginWidth, 0);
            contentsRect.setRight(contentsRect.right() - Metrics::ToolButton_InlineIndicatorWidth);
            contentsRect = QStyle::visualRect(option->direction,option->rect, contentsRect);
        }

        copy.rect = contentsRect;

        // render
        style->drawControl(QStyle::CE_ToolButtonLabel, &copy, painter, widget);
    }
    return true;
}
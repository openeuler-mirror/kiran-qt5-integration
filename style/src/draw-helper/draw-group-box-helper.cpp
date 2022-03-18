//
// Created by lxh on 2021/12/29.
//

#include "draw-group-box-helper.h"
#include <QGroupBox>
#include <QPainter>
#include <QStyleOption>
#include <QWidget>

//copy from QCommonStyle
QSize Kiran::Style::groupBoxSizeFromContents(const QStyle *style, const QStyleOption *option, const QSize &contentSize, const QWidget *widget)
{
    if (!widget || !widget->inherits("QGroupBox"))
    {
        return {};
    }

    auto groupBox = qobject_cast<const QGroupBox *>(widget);
    QSize size = contentSize + QSize(groupBox->isFlat() ? 16 : 0, 0);

    return size;
}

//copy from QCommonStyle
bool Kiran::Style::groupBoxSubControlRect(const QStyle *style, const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *widget, QRect &controlRect)
{
    const auto groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(opt);
    if(!groupBox)
        return false;

    switch (sc)
    {
    case QStyle::SC_GroupBoxFrame:
    case QStyle::SC_GroupBoxContents:
    {
        int topMargin = 0;
        int topHeight = 0;
        int verticalAlignment = style->styleHint(QStyle::SH_GroupBox_TextLabelVerticalAlignment, groupBox, widget);
        if (groupBox->text.size() || (groupBox->subControls & QStyle::SC_GroupBoxCheckBox))
        {
            topHeight = groupBox->fontMetrics.height();
            if (verticalAlignment & Qt::AlignVCenter)
                topMargin = topHeight / 2;
            else if (verticalAlignment & Qt::AlignTop)
                topMargin = topHeight;
        }

        QRect frameRect = groupBox->rect;
        frameRect.setTop(topMargin);

        if (sc == QStyle::SC_GroupBoxFrame)
        {
            controlRect = frameRect;
            break;
        }

        int frameWidth = 0;
        if ((groupBox->features & QStyleOptionFrame::Flat) == 0)
            frameWidth = style->pixelMetric(QStyle::PM_DefaultFrameWidth, groupBox, widget);
        controlRect = frameRect.adjusted(frameWidth, frameWidth + topHeight - topMargin, -frameWidth, -frameWidth);
        break;
    }
    case QStyle::SC_GroupBoxCheckBox:
    case QStyle::SC_GroupBoxLabel:
    {
        QFontMetrics fontMetrics = groupBox->fontMetrics;
        int h = fontMetrics.height();
        int tw = fontMetrics.size(Qt::TextShowMnemonic, groupBox->text + QLatin1Char(' ')).width();
        int marg = (groupBox->features & QStyleOptionFrame::Flat) ? 0 : 8;
        controlRect = groupBox->rect.adjusted(marg, 0, -marg, 0);
        controlRect.setHeight(h);

        int indicatorWidth = style->pixelMetric(QStyle::PM_IndicatorWidth, opt, widget);
        int indicatorSpace = style->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, opt, widget) - 1;
        bool hasCheckBox = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;
        int checkBoxSize = hasCheckBox ? (indicatorWidth + indicatorSpace) : 0;

        // Adjusted rect for label + indicatorWidth + indicatorSpace
        QRect totalRect = QStyle::alignedRect(groupBox->direction, groupBox->textAlignment,
                                              QSize(tw + checkBoxSize, h), controlRect);

        // Adjust totalRect if checkbox is set
        if (hasCheckBox)
        {
            bool ltr = groupBox->direction == Qt::LeftToRight;
            int left = 0;
            // Adjust for check box
            if (sc == QStyle::SC_GroupBoxCheckBox)
            {
                int indicatorHeight = style->pixelMetric(QStyle::PM_IndicatorHeight, opt, widget);
                left = ltr ? totalRect.left() : (totalRect.right() - indicatorWidth);
                int top = totalRect.top() + qMax(0, fontMetrics.height() - indicatorHeight) / 2;
                totalRect.setRect(left, top, indicatorWidth, indicatorHeight);
                // Adjust for label
            }
            else
            {
                left = ltr ? (totalRect.left() + checkBoxSize - 2) : totalRect.left();
                totalRect.setRect(left, totalRect.top(),
                                  totalRect.width() - checkBoxSize, totalRect.height());
            }
        }
        controlRect = totalRect;
        break;
    }
    default:
        return false;
    }

    return true;
}

//copy from QCommonStyle
bool Kiran::Style::drawCCGroupBox(const QStyle *style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget, Kiran::Style::SchemeLoader *scheme)
{
    const auto *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option);
    if (!groupBox)
    {
        return false;
    }

    // Draw frame
    QRect textRect = style->subControlRect(QStyle::CC_GroupBox, option, QStyle::SC_GroupBoxLabel, widget);
    QRect checkBoxRect = style->subControlRect(QStyle::CC_GroupBox, option, QStyle::SC_GroupBoxCheckBox, widget);
    if (groupBox->subControls & QStyle::SC_GroupBoxFrame)
    {
        QStyleOptionFrame frame;
        frame.QStyleOption::operator=(*groupBox);
        frame.features = groupBox->features;
        frame.lineWidth = groupBox->lineWidth;
        frame.midLineWidth = groupBox->midLineWidth;
        frame.rect = style->subControlRect(QStyle::CC_GroupBox, option, QStyle::SC_GroupBoxFrame, widget);
        painter->save();
        QRegion region(groupBox->rect);
        if (!groupBox->text.isEmpty())
        {
            bool ltr = groupBox->direction == Qt::LeftToRight;
            QRect finalRect;
            if (groupBox->subControls & QStyle::SC_GroupBoxCheckBox)
            {
                finalRect = checkBoxRect.united(textRect);
                finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
            }
            else
            {
                finalRect = textRect;
            }
            region -= finalRect;
        }
        painter->setClipRegion(region);
        style->drawPrimitive(QStyle::PE_FrameGroupBox, &frame, painter, widget);
        painter->restore();
    }

    // Draw title
    if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty())
    {
        QColor textColor = groupBox->textColor;
        if (textColor.isValid())
            painter->setPen(textColor);
        int alignment = int(groupBox->textAlignment);
        if (!style->styleHint(QStyle::SH_UnderlineShortcut, option, widget))
            alignment |= Qt::TextHideMnemonic;

        style->drawItemText(painter, textRect, Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                            groupBox->palette, groupBox->state & QStyle::State_Enabled, groupBox->text,
                            textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);

        if (groupBox->state & QStyle::State_HasFocus)
        {
            QStyleOptionFocusRect fropt;
            fropt.QStyleOption::operator=(*groupBox);
            fropt.rect = textRect;
            style->drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, painter, widget);
        }
    }

    // Draw checkbox
    if (groupBox->subControls & QStyle::SC_GroupBoxCheckBox)
    {
        QStyleOptionButton box;
        box.QStyleOption::operator=(*groupBox);
        box.rect = checkBoxRect;
        style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &box, painter, widget);
    }

    return true;
}

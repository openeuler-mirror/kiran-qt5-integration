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
#include "render-helper.h"
#include "metrics.h"

#include <QMenu>
#include <QPainter>
#include <QStyleOption>
#include <QWidgetAction>

#define private public
#include <private/qsvgtinydocument_p.h>
#undef private

namespace Kiran
{
PainterSaver::PainterSaver(QPainter *painter) : _painter(painter)
{
    _painter->save();
}

PainterSaver::~PainterSaver()
{
    _painter->restore();
}

int RenderHelper::mnemonicsTextFlags()
{
    // Qt::TextHideMnemonic Qt::TextShowMnemonic
    return Qt::TextHideMnemonic;
}

bool RenderHelper::showIconsOnPushButtons()
{
    return true;
}

bool RenderHelper::drawTreeBranches()
{
    return false;
}

bool RenderHelper::isQtQuickControl(const QStyleOption *option, const QWidget *widget)
{
    return (widget == nullptr) && option && option->styleObject && option->styleObject->inherits("QQuickItem");
}

bool RenderHelper::isVerticalTab(const QTabBar::Shape &shape)
{
    return shape == QTabBar::RoundedEast || shape == QTabBar::RoundedWest || shape == QTabBar::TriangularEast || shape == QTabBar::TriangularWest;
}

#define property_menu_title "_kiran_property_menu_title_"
bool RenderHelper::isMenuTitle(const QWidget *widget)
{
    if (!widget) return false;

    // FIXME:若控件后续移动，该属性不会自动更新，将会导致问题
    QVariant property(widget->property(property_menu_title));
    if (property.isValid()) return property.toBool();

    QWidget *parent = widget->parentWidget();
    if (qobject_cast<QMenu *>(parent))
    {
        foreach (auto child, parent->findChildren<QWidgetAction *>())
        {
            if (child->defaultWidget() != widget) continue;
            const_cast<QWidget *>(widget)->setProperty(property_menu_title, true);
            return true;
        }
    }

    const_cast<QWidget *>(widget)->setProperty(property_menu_title, false);
    return false;
}

QWindow* RenderHelper::getWindow(const QWidget* widget)
{
    return widget? widget->window()->windowHandle(): nullptr;
}

QRectF RenderHelper::strokedRect(const QRectF &rect, const qreal penWidth)
{
    qreal adjustment = 0.5 * penWidth;
    return rect.adjusted(adjustment, adjustment, -adjustment, -adjustment);
}

QRect RenderHelper::insideMargin(const QRect &r, int margin)
{
    return insideMargin(r, margin, margin);
}

QRect RenderHelper::insideMargin(const QRect &r, int marginWidth, int marginHeight)
{
    return r.adjusted(marginWidth, marginHeight, -marginWidth, -marginHeight);
}

QRect RenderHelper::centerRect(const QRect &rect, int width, int height)
{
    return {rect.left() + (rect.width() - width) / 2, rect.top() + (rect.height() - height) / 2, width, height};
}

QRect RenderHelper::centerRect(const QRect &rect, const QSize &size)
{
    return centerRect(rect, size.width(), size.height());
}

QSize RenderHelper::expandSize(const QSize &size, int margin)
{
    return expandSize(size, margin, margin);
}

QSize RenderHelper::expandSize(const QSize &size, int marginWidth, int marginHeight)
{
    return size + 2 * QSize(marginWidth, marginHeight);
}

QPainterPath RenderHelper::roundedPath(const QRectF &rect, Corners corners, int radius)
{
    QPainterPath path;

    if (corners == 0)
    {
        path.addRect(rect);
        return path;
    }

    if (corners == AllCorners)
    {
        path.addRoundedRect(rect, radius, radius);
        return path;
    }

    const QSizeF cornerSize(2 * radius, 2 * radius);

    // rotate counterclockwise
    // top left corner
    if (corners & CornerTopLeft)
    {
        path.moveTo(rect.topLeft() + QPointF(radius, 0));
        path.arcTo(QRectF(rect.topLeft(), cornerSize), 90, 90);
    }
    else
        path.moveTo(rect.topLeft());

    // bottom left corner
    if (corners & CornerBottomLeft)
    {
        path.lineTo(rect.bottomLeft() - QPointF(0, radius));
        path.arcTo(QRectF(rect.bottomLeft() - QPointF(0, 2 * radius), cornerSize), 180, 90);
    }
    else
        path.lineTo(rect.bottomLeft());

    // bottom right corner
    if (corners & CornerBottomRight)
    {
        path.lineTo(rect.bottomRight() - QPointF(radius, 0));
        path.arcTo(QRectF(rect.bottomRight() - QPointF(2 * radius, 2 * radius), cornerSize), 270, 90);
    }
    else
        path.lineTo(rect.bottomRight());

    // top right corner
    if (corners & CornerTopRight)
    {
        path.lineTo(rect.topRight() + QPointF(0, radius));
        path.arcTo(QRectF(rect.topRight() - QPointF(2 * radius, 0), cornerSize), 0, 90);
    }
    else
        path.lineTo(rect.topRight());

    path.closeSubpath();
    return path;
}

void RenderHelper::renderFrame(QPainter *painter, const QRect &rect, int penWidth, int radius, const QColor &color, const QColor &outline)
{
    PainterSaver painterSave(painter);

    QRectF frameRect(rect);

    // set pen
    if (outline.isValid())
    {
        painter->setPen(outline);
        frameRect = strokedRect(frameRect, penWidth / 2.0);
    }
    else
    {
        painter->setPen(Qt::NoPen);
    }

    // set brush
    if (color.isValid())
        painter->setBrush(color);
    else
        painter->setBrush(Qt::NoBrush);

    // render
    painter->drawRoundedRect(frameRect, radius, radius);
}

void RenderHelper::renderFlatFrame(QPainter *painter, const QRect &rect, int radius, const QColor &color, const QColor &outline)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF frameRect(rect.adjusted(1, 1, -1, -1));

    // set pen
    if (outline.isValid())
    {
        painter->setPen(outline);
        frameRect = strokedRect(frameRect, 1);
    }
    else
    {
        painter->setPen(Qt::NoPen);
    }

    // set brush
    if (color.isValid())
        painter->setBrush(color);
    else
        painter->setBrush(Qt::NoBrush);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(frameRect.adjusted(2 * radius, 0, 0, 0));
    path.addRoundedRect(frameRect.adjusted(0, 0, -2 * radius, 0), radius, radius);

    painter->drawPath(path.simplified());
}

void RenderHelper::renderSeparator(QPainter *painter, const QRect &rect, bool vertical, const QColor &color)
{
    PainterSaver painterSave(painter);

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(color);

    if (vertical)
    {
        painter->translate(rect.width() / 2, 0);
        painter->drawLine(rect.topLeft(), rect.bottomLeft());
    }
    else
    {
        painter->translate(0, rect.height() / 2);
        painter->drawLine(rect.topLeft(), rect.topRight());
    }
}

void RenderHelper::renderMenuTitle(const QStyle *style,
                                   QPainter *painter,
                                   const QStyleOptionToolButton *option,
                                   const QWidget *widget,
                                   const QColor &separatorColor)
{
    // 底部渲染分割线
    const QPalette &palette(option->palette);
    auto separatorRect = QRect(option->rect.bottomLeft() - QPoint(0, Metrics::MenuItem_MarginWidth), QSize(option->rect.width(), 1));
    renderSeparator(painter, separatorRect, false, separatorColor);

    // 渲染文本居中,丢弃图标
    painter->setFont(option->font);
    QRect contentsRect = insideMargin(option->rect, Metrics::MenuItem_MarginWidth);
    style->drawItemText(painter, contentsRect, Qt::AlignCenter, palette, true, option->text, QPalette::WindowText);
}

QPixmap RenderHelper::changeSVGFillColor(const QString &svgFile, const QColor &fillColor, const QSize &size)
{
    // 调用Qt私有接口，修改所有命名节点其所有的fill属性为指定颜色
    auto tinyDoc = QSvgTinyDocument::load(svgFile);

    auto namedNodes = tinyDoc->m_namedNodes;
    for (auto iter = namedNodes.begin(); iter != namedNodes.end(); iter++)
    {
        auto styleProperty = iter.value()->styleProperty(QSvgStyleProperty::FILL);
        if (styleProperty != nullptr)
        {
            auto fillProperty = dynamic_cast<QSvgFillStyle *>(styleProperty);
            fillProperty->m_fill.setColor(fillColor);
        }
    }

    // 渲染到Pixmap之中
    QPixmap tempPixmap(size);
    tempPixmap.fill(Qt::transparent);

    QPainter tempPainter;
    tempPainter.begin(&tempPixmap);
    tempPainter.setRenderHint(QPainter::Antialiasing);
    tempPainter.setRenderHint(QPainter::SmoothPixmapTransform);
    tinyDoc->draw(&tempPainter);
    tempPainter.end();

    return tempPixmap;
}

void RenderHelper::renderArrow(QPainter *painter, const QRect &rect, ArrowOrientation orientation, const QColor &color, const QSize &arrowSize)
{
    QString svgFile = QString(":/style-helper/images/arrow.svg");
    PainterSaver painterSave(painter);
    painter->setRenderHint(QPainter::Antialiasing);

    // 修改箭头默认颜色
    QPixmap tempPixmap = changeSVGFillColor(svgFile, color, arrowSize.isEmpty() ? QSize(10, 8) : arrowSize);

    // 旋转图片
    int rotateAngle = 0;
    switch (orientation)
    {
    case Arrow_Up:
        rotateAngle = 180;
        break;
    case Arrow_Down:
        break;
    case Arrow_Left:
        rotateAngle = 90;
        break;
    case Arrow_Right:
        rotateAngle = 270;
        break;
    default:
        break;
    }
    QMatrix matrix;
    matrix.rotate(rotateAngle);
    QPixmap arrowPixmap = tempPixmap.transformed(matrix, Qt::SmoothTransformation);

    // 绘制图片
    QRect arrowRect = centerRect(rect, arrowPixmap.size());
    painter->drawPixmap(arrowRect, arrowPixmap);
}

void RenderHelper::renderTabBarTab(QPainter *painter,
                                   const QRect &rect,
                                   Corners corners,
                                   int radius,
                                   const QColor &background,
                                   const QColor &border)
{
    PainterSaver painterSave(painter);

    QRectF frameRect(rect);

    // set pen
    if (border.isValid())
    {
        painter->setPen(border);
        frameRect = strokedRect(frameRect, 1);
    }
    else
    {
        painter->setPen(Qt::NoPen);
    }

    painter->setBrush(background);
    QPainterPath painterPath = roundedPath(frameRect, corners, radius);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(painterPath);
}

}  // namespace Kiran
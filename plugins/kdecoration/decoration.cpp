/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-qt5-integraiton is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "decoration.h"
#include <QtCore/qglobal.h>
#include <KConfig>
#include <KDecoration2/DecoratedClient>
#include <KDecoration2/DecorationSettings>
#include <KDecoration2/DecorationShadow>
#include <KSharedConfig>
#include <QDebug>
#include <QPainter>
#include <QPalette>
#include <QPoint>
#include <QSharedPointer>
#include "box-shadow-helper.h"
#include "button.h"
#include "palette.h"
#include "shadow-params.h"

namespace Kiran
{
namespace KDecoration
{
// 预定义组合阴影参数，参考 Material Design
// https://material.io/design/environment/elevation.html#default-elevations
// https://material.io/design/environment/elevation.html#shadows
const CompositeShadowParams s_shadowParams = CompositeShadowParams(
    QPoint(0, 18),
    ShadowParams(QPoint(0, 0), 64, 0.8),
    ShadowParams(QPoint(0, -10), 24, 0.1));

// 阴影颜色，固定为深灰色
static QColor s_shadowColor(33, 33, 33);

// 当前Decoration实例计数，用于管理全局资源释放
static int s_decoCount = 0;

// 全局缓存的阴影对象，避免重复生成，提升性能
static QSharedPointer<KDecoration2::DecorationShadow> s_cachedShadow;

Decoration::Decoration(QObject *parent, const QVariantList &args)
    : KDecoration2::Decoration(parent, args)
{
    ++s_decoCount;
}

Decoration::~Decoration()
{
    if (--s_decoCount == 0)
    {
        s_cachedShadow.clear();
    }
}

void Decoration::paint(QPainter *painter, const QRect &repaintRegion)
{
    auto decoratedClient = client().toStrongRef();
    if (!decoratedClient->isShaded())
    {
        paintFrameBackground(painter, repaintRegion);
    }

    paintTitleBarBackground(painter, repaintRegion);
    paintButtons(painter, repaintRegion);
    paintCaption(painter, repaintRegion);
}

void Decoration::init()
{
    auto c = client().toStrongRef();
    auto s = settings();

    connect(c.data(), &KDecoration2::DecoratedClient::widthChanged,
            this, &Decoration::updateTitleBar);
    connect(c.data(), &KDecoration2::DecoratedClient::widthChanged,
            this, &Decoration::updateButtonsGeometry);
    connect(c.data(), &KDecoration2::DecoratedClient::maximizedChanged,
            this, &Decoration::updateButtonsGeometry);

    auto repaintTitleBar = [this]
    {
        update(titleBar());
    };
    connect(c.data(), &KDecoration2::DecoratedClient::captionChanged,
            this, repaintTitleBar);
    connect(c.data(), &KDecoration2::DecoratedClient::activeChanged,
            this, repaintTitleBar);
    connect(Theme::Palette::getDefault(), &Theme::Palette::baseColorsChanged,
            this, repaintTitleBar);

    updateBorders();
    updateResizeBorders();
    updateTitleBar();

    auto buttonCreator = [this](KDecoration2::DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent)
        -> KDecoration2::DecorationButton *
    {
        Q_UNUSED(decoration)
        if (!Button::isSupported(type))
        {
            return nullptr;
        }
        return new Button(type, this, parent);
    };

    m_leftButtons = new KDecoration2::DecorationButtonGroup(
        KDecoration2::DecorationButtonGroup::Position::Left,
        this,
        buttonCreator);

    m_rightButtons = new KDecoration2::DecorationButtonGroup(
        KDecoration2::DecorationButtonGroup::Position::Right,
        this,
        buttonCreator);

    updateButtonsGeometry();

    // For some reason, the shadow should be installed the last. Otherwise,
    // the Window Decorations KCM crashes.
    updateShadow();
}

void Decoration::updateBorders()
{
    QMargins borders;
    borders.setTop(titleBarHeight());
    setBorders(borders);
}

void Decoration::updateResizeBorders()
{
    QMargins borders;

    const int extender = settings()->largeSpacing();
    borders.setLeft(extender);
    borders.setTop(extender);
    borders.setRight(extender);
    borders.setBottom(0);

    setResizeOnlyBorders(borders);
}

void Decoration::updateTitleBar()
{
    auto decoratedClient = client().toStrongRef();
    setTitleBar(QRect(0, 0, decoratedClient->width(), titleBarHeight()));
}

void Decoration::updateButtonsGeometry()
{
    if (!m_leftButtons->buttons().isEmpty())
    {
        m_leftButtons->setPos(QPointF(0, 0));
        m_leftButtons->setSpacing(0);
    }

    if (!m_rightButtons->buttons().isEmpty())
    {
        m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width(), 0));
        m_rightButtons->setSpacing(0);
    }

    update();
}

void Decoration::updateShadow()
{
    if (!s_cachedShadow.isNull())
    {
        setShadow(s_cachedShadow);
        return;
    }

    auto withOpacity = [](const QColor &color, qreal opacity) -> QColor
    {
        QColor c(color);
        c.setAlphaF(opacity);
        return c;
    };

    // 为了保证阴影完整显示，需根据最大阴影半径计算绘制区域：
    // shadowSize：取两层阴影中较大的半径，确保所有阴影都能被容纳。
    // box：实际用于绘制阴影的矩形区域，中心为(shadowSize, shadowSize)，宽高为2*shadowSize+1。
    // rect：最终阴影图像区域，比box再向四周扩展shadowSize，保证模糊扩散不会被裁剪。
    const int shadowSize = qMax(s_shadowParams.shape.radius, s_shadowParams.contrast.radius);
    const QRect box(shadowSize, shadowSize, 2 * shadowSize + 1, 2 * shadowSize + 1);
    const QRect rect = box.adjusted(-shadowSize, -shadowSize, shadowSize, shadowSize);

    QImage shadow(rect.size(), QImage::Format_ARGB32_Premultiplied);
    shadow.fill(Qt::transparent);

    QPainter painter(&shadow);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制主体阴影
    BoxShadowHelper::boxShadow(
        &painter,
        box,
        s_shadowParams.shape.offset,
        s_shadowParams.shape.radius,
        withOpacity(s_shadowColor, s_shadowParams.shape.opacity));

    // 绘制对比阴影
    BoxShadowHelper::boxShadow(
        &painter,
        box,
        s_shadowParams.contrast.offset,
        s_shadowParams.contrast.radius,
        withOpacity(s_shadowColor, s_shadowParams.contrast.opacity));

    // 去除内矩形，只保留外环阴影
    const QMargins padding = QMargins(
        shadowSize - s_shadowParams.offset.x(),
        shadowSize - s_shadowParams.offset.y(),
        shadowSize + s_shadowParams.offset.x(),
        shadowSize + s_shadowParams.offset.y());
    const QRect innerRect = rect - padding;

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.drawRect(innerRect);
    painter.end();

    // 生成阴影对象并缓存
    s_cachedShadow = QSharedPointer<KDecoration2::DecorationShadow>::create();
    s_cachedShadow->setPadding(padding);
    s_cachedShadow->setInnerShadowRect(QRect(shadow.rect().center(), QSize(1, 1)));
    s_cachedShadow->setShadow(shadow);

    // 安装阴影
    setShadow(s_cachedShadow);
}

int Decoration::titleBarHeight() const
{
    return 35;
}

void Decoration::paintFrameBackground(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)

    const auto decoratedClient = client().toStrongRef();
    const auto group = decoratedClient->isActive()
                           ? KDecoration2::ColorGroup::Active
                           : KDecoration2::ColorGroup::Inactive;
    const auto frameColor = decoratedClient->color(group, KDecoration2::ColorRole::Frame);

    painter->save();
    painter->fillRect(rect(), Qt::transparent);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(frameColor);
    painter->setClipRect(0, borderTop(), size().width(), size().height() - borderTop(), Qt::IntersectClip);
    painter->drawRect(rect());
    painter->restore();
}

QColor Decoration::titleBarBackgroundColor() const
{
    const auto decoratedClient = client().toStrongRef();
    Q_UNUSED(decoratedClient);

    auto palette = Theme::Palette::getDefault();
    auto bg = palette->getColor(Theme::Palette::ACTIVE, Theme::Palette::WINDOW);
    return bg;
}

QColor Decoration::titleBarForegroundColor() const
{
    const auto decoratedClient = client().toStrongRef();
    Q_UNUSED(decoratedClient);

    auto palette = Theme::Palette::getDefault();
    auto fg = palette->getColor(Theme::Palette::ACTIVE, Theme::Palette::TEXT);
    return fg;
}

void Decoration::paintTitleBarBackground(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)

    const auto decoratedClient = client().toStrongRef();
    auto rect = titleBar();

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(titleBarBackgroundColor());
    painter->drawRect(rect);
    painter->restore();
}

void Decoration::paintCaption(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)

    const auto decoratedClient = client().toStrongRef();
    QRect availableRect = titleBar();

    // 去除左右两个ButtonGroup占位
    availableRect.adjust(m_leftButtons->geometry().width() + settings()->largeSpacing(), 0,
                         -(m_rightButtons->geometry().width() + settings()->largeSpacing()), 0);

    // 绘制Icon
    if (!decoratedClient->icon().isNull())
    {
        auto icon = decoratedClient->icon();
        auto iconSize = QSize(20, 20);
        auto iconRect = QRect(availableRect.left(),
                              (availableRect.height() - iconSize.height()) / 2,
                              iconSize.width(),
                              iconSize.height());

        icon.paint(painter, iconRect, Qt::AlignLeft | Qt::AlignVCenter, QIcon::Normal, decoratedClient->isActive() ? QIcon::On : QIcon::Off);
        availableRect.adjust(iconRect.height() + settings()->largeSpacing(), 0, 0, 0);
    }

    // 定位文本
    const int textWidth = settings()->fontMetrics().boundingRect(decoratedClient->caption()).width();
    const QRect textRect(availableRect.topLeft(), QSize(textWidth, titleBarHeight()));
    Q_UNUSED(textRect);

    QRect captionRect = availableRect;
    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;

    const QString caption = painter->fontMetrics().elidedText(
        decoratedClient->caption(),
        Qt::ElideMiddle,
        captionRect.width());

    painter->save();
    painter->setFont(settings()->font());
    painter->setPen(titleBarForegroundColor());
    painter->drawText(captionRect, alignment, caption);
    painter->restore();
}

void Decoration::paintButtons(QPainter *painter, const QRect &repaintRegion) const
{
    m_leftButtons->paint(painter, repaintRegion);
    m_rightButtons->paint(painter, repaintRegion);
}

}  // namespace KDecoration
}  // namespace Kiran
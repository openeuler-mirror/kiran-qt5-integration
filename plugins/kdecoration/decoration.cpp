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
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QPoint>
#include <QSharedPointer>
#include "button.h"
#include "internel-setting.h"
#include "palette.h"
#include "utils.h"

namespace Kiran
{
namespace KDecoration
{
// 预定义组合阴影参数，参考 Material Design
// https://material.io/design/environment/elevation.html#default-elevations
// https://material.io/design/environment/elevation.html#shadows
const CompositeShadowParams s_shadowParams = CompositeShadowParams(
    QPoint(0, 4),
    ShadowParams(QPoint(0, 0), 16, 0.6),
    ShadowParams(QPoint(0, -2), 8, 0.1));

// 阴影颜色，固定为深灰色
static QColor s_shadowColor(33, 33, 33);

// 按主题类型（borderRadius）分别缓存的阴影对象，避免重复生成，提升性能
// key: borderRadius值，value: 缓存的阴影对象
static QMap<int, QSharedPointer<KDecoration2::DecorationShadow>> s_cachedShadows;
// 每个主题类型的使用计数，用于管理资源释放
// key: borderRadius值，value: 使用该主题的Decoration实例数量
static QMap<int, int> s_themeUsageCount;

Decoration::Decoration(QObject *parent, const QVariantList &args)
    : KDecoration2::Decoration(parent, args)
{
    QVariantMap map;
    if (args.size() >= 1 && args.at(0).canConvert<QVariantMap>())
    {
        map = args.at(0).toMap();
    }
    m_internelSetting = new InternelSetting(map,this, this);
    // 增加当前主题类型的使用计数
    int borderRadius = m_internelSetting->borderRadius();
    s_themeUsageCount[borderRadius]++;
}

Decoration::~Decoration()
{
    // 减少当前主题类型的使用计数
    int borderRadius = m_internelSetting->borderRadius();
    if (--s_themeUsageCount[borderRadius] == 0)
    {
        // 如果没有其他实例使用该主题，清除对应的阴影缓存和使用计数
        s_cachedShadows.remove(borderRadius);
        s_themeUsageCount.remove(borderRadius);
    }
}

void Decoration::paint(QPainter *painter, const QRect &repaintRegion)
{
    auto decoratedClient = client().toStrongRef();
    if (!decoratedClient->isShaded())
    {
        // 绘制边框背景
        paintFrameBackground(painter, repaintRegion);
    }

    paintTitleBarBackground(painter, repaintRegion);
    paintButtons(painter, repaintRegion);
    paintCaption(painter, repaintRegion);

    if (!decoratedClient->isMaximized())
    {
        paintBorder(painter, repaintRegion);
    }
}

void Decoration::init()
{
    auto c = client().toStrongRef();
    auto s = settings();
    auto themePalette = Theme::Palette::getDefault();

    Q_UNUSED(s)

    updateBorders();
    updateTitleBar();
    updateResizeBorders();

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

    connect(themePalette, &Theme::Palette::baseColorsChanged,
            this, [this]
            { update(); });
    connect(c.data(), &KDecoration2::DecoratedClient::widthChanged, this, [this]
            {
                updateTitleBar();
                updateButtonsGeometry();
                update();
            });
    connect(c.data(), &KDecoration2::DecoratedClient::maximizedChanged,
            this, [this]
            {
                updateBorders();
                updateTitleBar();
                updateButtonsGeometry();
                update();
            });
    auto repaintTitleBar = [this]
    {
        update(titleBar());
    };
    connect(c.data(), &KDecoration2::DecoratedClient::captionChanged,
            this, repaintTitleBar);
    connect(c.data(), &KDecoration2::DecoratedClient::activeChanged,
            this, repaintTitleBar);
}

bool Decoration::atEdge(Qt::Edge edge) const
{
    auto c = client().toStrongRef();
    const bool drawBorderOnMaximizedWindows = false;

    // 如果启用最大化窗口时绘制边框，则不考虑边缘情况
    if( drawBorderOnMaximizedWindows )
    {
        return false;
    }

    if( c->isMaximized() )
    {
        return true;
    }

    if( edge == Qt::LeftEdge || edge == Qt::RightEdge )
    {
        if( c->isMaximizedHorizontally() )
        {
            return true;
        }
    }
    else if (edge == Qt::TopEdge || edge == Qt::BottomEdge)
    {
        if( c->isMaximizedVertically() )
        {
            return true;
        }
    }

    return c->adjacentScreenEdges().testFlag(edge);
}

void Decoration::updateBorders()
{
    QMargins margins;
    const int border = borderWidth();
    const int titleBarHeight = m_internelSetting->titleBarHeight();
    margins.setTop(titleBarHeight + border);
    margins.setBottom(border);
    margins.setLeft(border);
    margins.setRight(border);
    setBorders(margins);
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
    // 窗口标题栏填充边框区域，边框绘制在标题栏上方
    auto decoratedClient = client().toStrongRef();
    const int border = borderWidth();
    QRect titleBarRect(0, 0,
                       decoratedClient->width() + 2 * border,
                       m_internelSetting->titleBarHeight() + border);
    setTitleBar(titleBarRect);
}

void Decoration::updateButtonsGeometry()
{
    // 根据按钮大小以及标题栏高度，保持按钮居中
    auto buttonSize = m_internelSetting->buttonSize();
    auto titleBarHeightValue = m_internelSetting->titleBarHeight();
    qreal yOffset = (titleBarHeightValue - buttonSize) / 2.0;
    const qreal spacing = m_internelSetting->buttonSpacing();

    if (!m_leftButtons->buttons().isEmpty())
    {
        m_leftButtons->setSpacing(spacing);
        m_leftButtons->setPos(QPointF(spacing, yOffset));
    }

    if (!m_rightButtons->buttons().isEmpty())
    {
        m_rightButtons->setSpacing(spacing);
        qreal rightWidth = m_rightButtons->geometry().width();
        m_rightButtons->setPos(QPointF(size().width() - rightWidth - spacing, yOffset));
    }

    update();
}

void Decoration::updateShadow()
{
    // 根据当前主题的borderRadius获取对应的阴影缓存
    int borderRadius = m_internelSetting->borderRadius();
    auto cachedShadow = s_cachedShadows.value(borderRadius);
    
    if (!cachedShadow.isNull())
    {
        setShadow(cachedShadow);
        return;
    }

    auto withOpacity = [](const QColor &color, qreal opacity) -> QColor
    {
        QColor c(color);
        c.setAlphaF(opacity);
        return c;
    };

    // 形成9Patch机制的阴影图像数据,窗口调整大小时无需重新计算阴影图像数据
    //
    // 为了保证阴影完整显示，需根据最大阴影半径计算绘制区域：
    // shadowSize：取两层阴影中较大的半径，确保所有阴影都能被容纳。
    // box：实际用于绘制阴影的矩形区域，中心为(shadowSize, shadowSize)，宽高为2*shadowSize+1。
    // rect：最终阴影图像区域，比box再向四周扩展shadowSize，保证模糊扩散不会被裁剪。
    const int shadowSize = qMax(s_shadowParams.shape.radius, s_shadowParams.contrast.radius);
    const QRect box(shadowSize, shadowSize, 2 * shadowSize + 1, 2 * shadowSize + 1);
    const QRect rect = box.adjusted(-shadowSize, -shadowSize, shadowSize, shadowSize);

    const int shadowRadius = m_internelSetting->borderRadius();
    CornerRadii radii(shadowRadius);
    QPainterPath roundedBoxPath = Utils::createRoundedRectPath(box, radii, TopLeftCorner | TopRightCorner);

    QImage shadow(rect.size(), QImage::Format_ARGB32_Premultiplied);
    shadow.fill(Qt::transparent);

    QPainter painter(&shadow);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制主体阴影（使用圆角路径）
    Utils::boxShadow(
        &painter,
        roundedBoxPath,
        s_shadowParams.shape.offset,
        s_shadowParams.shape.radius,
        withOpacity(s_shadowColor, s_shadowParams.shape.opacity));

    // 绘制对比阴影（使用圆角路径）
    Utils::boxShadow(
        &painter,
        roundedBoxPath,
        s_shadowParams.contrast.offset,
        s_shadowParams.contrast.radius,
        withOpacity(s_shadowColor, s_shadowParams.contrast.opacity));

    // 去除内圆角矩形，只保留外环阴影
    const QMargins padding = QMargins(
        shadowSize - s_shadowParams.offset.x(),
        shadowSize - s_shadowParams.offset.y(),
        shadowSize + s_shadowParams.offset.x(),
        shadowSize + s_shadowParams.offset.y());
    const QRect innerRect = rect - padding;
    QPainterPath innerRoundedPath = Utils::createRoundedRectPath(innerRect, radii, AllCorners);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.drawPath(innerRoundedPath);
    painter.end();

    // 生成阴影对象并按主题类型缓存
    auto newShadow = QSharedPointer<KDecoration2::DecorationShadow>::create();
    newShadow->setPadding(padding);
    newShadow->setInnerShadowRect(QRect(shadow.rect().center(), QSize(1, 1)));
    newShadow->setShadow(shadow);
    
    // 缓存当前主题类型的阴影
    s_cachedShadows[borderRadius] = newShadow;

    // 安装阴影
    setShadow(newShadow);
}

void Decoration::paintFrameBackground(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)
    const auto decoratedClient = client().toStrongRef();
    const auto frameColor = frameBackgroundColor();
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(frameColor, 1));
    painter->setBrush(frameColor);
    painter->setClipRect(0, borderTop(), size().width(), size().height() - borderTop(), Qt::IntersectClip);

    const bool isMaximized = decoratedClient->isMaximized();
    if (isMaximized)
    {
        painter->drawRect(rect());
    }
    else
    {
        CornerRadii radii(m_internelSetting->borderRadius());
        CornerPositions corners = BottomLeftCorner | BottomRightCorner;
        QPainterPath path = Utils::createRoundedRectPath(rect(), radii, corners, 0);
        painter->drawPath(path);
    }
    painter->restore();
}

QColor Decoration::frameBackgroundColor() const
{
    const auto palette = Theme::Palette::getDefault();
    const auto decoratedClient = client().toStrongRef();
    const auto active = decoratedClient->isActive();
    auto bg = palette->getColor(active ? Theme::Palette::ACTIVE : Theme::Palette::INACTIVE, Theme::Palette::WINDOW);
    return bg;
}

int Decoration::borderWidth() const
{
    const auto decoratedClient = client().toStrongRef();
    const bool isMaximized = decoratedClient->isMaximized();
    return isMaximized ? 0 : m_internelSetting->borderWidth();
}

QColor Decoration::borderColor() const
{
    const auto palette = Theme::Palette::getDefault();
    const auto decoratedClient = client().toStrongRef();
    const auto active = decoratedClient->isActive();
    auto bg = palette->getColor(active ? Theme::Palette::ACTIVE : Theme::Palette::INACTIVE, Theme::Palette::BORDER);
    return bg;
}

QColor Decoration::titleBarBackgroundColor() const
{
    const auto palette = Theme::Palette::getDefault();
    const auto decoratedClient = client().toStrongRef();
    const auto active = decoratedClient->isActive();
    auto bg = palette->getColor(active ? Theme::Palette::ACTIVE : Theme::Palette::INACTIVE, Theme::Palette::WINDOW);
    return bg;
}

QColor Decoration::titleBarForegroundColor() const
{
    auto palette = Theme::Palette::getDefault();
    const auto decoratedClient = client().toStrongRef();
    const auto active = decoratedClient->isActive();
    auto fg = palette->getColor(active ? Theme::Palette::ACTIVE : Theme::Palette::INACTIVE, Theme::Palette::TEXT);
    return fg;
}

void Decoration::paintTitleBarBackground(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)

    const auto decoratedClient = client().toStrongRef();
    const bool isMaximized = decoratedClient->isMaximized();
    auto rect = titleBar();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(titleBarBackgroundColor());

    if (isMaximized)
    {
        painter->fillRect(rect, titleBarBackgroundColor());
    }
    else
    {
        CornerRadii radii(m_internelSetting->borderRadius());
        CornerPositions corners = TopLeftCorner | TopRightCorner;
        QPainterPath path = Utils::createRoundedRectPath(rect, radii, corners, 0.5);
        painter->fillPath(path, titleBarBackgroundColor());
    }
    painter->restore();
}

void Decoration::paintCaption(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)
    const auto decoratedClient = client().toStrongRef();
    QRect availableRect = titleBar();

    const auto s = settings();
    const auto scaleFactor = m_internelSetting->scaleFactor();

    // 去除左右两个ButtonGroup占位
    availableRect.adjust(m_leftButtons->geometry().width() + settings()->largeSpacing(), 0,
                         -(m_rightButtons->geometry().width() + settings()->largeSpacing()), 0);

    // 绘制Icon
    if (!decoratedClient->icon().isNull())
    {
        auto icon = decoratedClient->icon();
        auto iconSize = QSize(20*scaleFactor, 20*scaleFactor);
        auto iconRect = QRect(availableRect.left(),
                              (availableRect.height() - iconSize.height()) / 2,
                              iconSize.width(),
                              iconSize.height());

        icon.paint(painter, iconRect, Qt::AlignLeft | Qt::AlignVCenter,
                   QIcon::Normal,
                   decoratedClient->isActive() ? QIcon::On : QIcon::Off);
        availableRect.adjust(iconRect.height() + settings()->largeSpacing(), 0, 0, 0);
    }

    // 定位文本
    painter->save();
    painter->setFont(s->font());
    const int textWidth = painter->fontMetrics().boundingRect(decoratedClient->caption()).width();
    const QRect textRect(availableRect.topLeft(), QSize(textWidth, m_internelSetting->titleBarHeight()));
    Q_UNUSED(textRect);

    QRect captionRect = availableRect;
    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;

    const QString caption = painter->fontMetrics().elidedText(
        decoratedClient->caption(),
        Qt::ElideMiddle,
        captionRect.width());

    painter->setPen(titleBarForegroundColor());
    painter->drawText(captionRect, alignment, caption);
    painter->restore();
}

void Decoration::paintButtons(QPainter *painter, const QRect &repaintRegion) const
{
    m_leftButtons->paint(painter, repaintRegion);
    m_rightButtons->paint(painter, repaintRegion);
}

void Decoration::paintBorder(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)
    const int width = this->borderWidth();
    if (width <= 0)
    {
        return;
    }

    QPen pen(borderColor(), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    QPainterPath path = Utils::createRoundedRectPath(rect(),
                                                     CornerRadii(m_internelSetting->borderRadius()),
                                                     TopLeftCorner | TopRightCorner);
    painter->strokePath(path, pen);
    painter->restore();
}

ThemeProvider::ThemeProvider(QObject *parent,
                             const KPluginMetaData &data,
                             const QVariantList &args)
    : KDecoration2::DecorationThemeProvider(parent, data, args),
      m_data(data)
{
    init();
}

void ThemeProvider::init()
{
    auto themes = InternelSetting::supportedThemes();
    for (const auto &theme : themes)
    {
        KDecoration2::DecorationThemeMetaData data;
        data.setPluginId(m_data.pluginId());
        data.setThemeName(theme);
        data.setVisibleName(theme);
        data.setHasConfiguration(true);
        m_themes.append(data);
    }
}
}  // namespace KDecoration
}  // namespace Kiran
/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
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
#include "button.h"
#include <kdecoration2/decorationdefines.h>
#include <KDecoration2/DecoratedClient>
#include <QColor>
#include <QIcon>
#include <QMap>
#include <QPainter>
#include <QWeakPointer>

namespace Kiran
{
namespace KDecoration
{
struct ButtonRenderRule
{
    QString icon;
    QColor hoverBackground;
    ButtonRenderRule(const QString &icon,
                     const QColor &hoverBackground)
        : icon(icon), hoverBackground(hoverBackground) {}
};

static QMap<Button::Type, ButtonRenderRule> buttonRenderRuleMap = {
    {KDecoration2::DecorationButtonType::Close, {QStringLiteral("window-close-symbolic"), QColor(255, 0, 0, 200)}},
    {KDecoration2::DecorationButtonType::Maximize, {QStringLiteral("window-maximize-symbolic"), QColor(65, 65, 65, 50)}},
    {KDecoration2::DecorationButtonType::Minimize, {QStringLiteral("window-minimize-symbolic"), QColor(65, 65, 65, 50)}}};
Button::Button(Type type, Decoration *decoration, QObject *parent)
    : KDecoration2::DecorationButton(type, decoration, parent)
{
    auto decoratedClient = decoration->client().toStrongRef();
    connect(decoratedClient.data(), &KDecoration2::DecoratedClient::closeableChanged,
            this, &Button::updateVisible);
    connect(decoratedClient.data(), &KDecoration2::DecoratedClient::maximizeableChanged,
            this, &Button::updateVisible);
    connect(decoratedClient.data(), &KDecoration2::DecoratedClient::minimizeableChanged,
            this, &Button::updateVisible);
    setGeometry(QRect(QPoint(0, 0), QSize(decoration->titleBarHeight(), decoration->titleBarHeight())));
}

Button::~Button()
{
}

bool Button::isSupported(Type type)
{
    if (buttonRenderRuleMap.contains(type))
    {
        return true;
    }

    return false;
}

void Button::paint(QPainter *painter, const QRect &repaintRegion)
{
    auto renderRule = buttonRenderRuleMap.value(type(), ButtonRenderRule(QString(), QColor()));
    auto icon = QIcon::fromTheme(renderRule.icon);

    if (!icon.isNull())
    {
        auto pixmap = icon.pixmap(QSize(16, 16));
        auto pixmapRect = pixmap.rect();

        auto rect = geometry();
        pixmapRect.moveCenter(rect.center().toPoint());

        painter->save();

        // Background
        bool hoverd = isHovered() || isPressed();
        if (hoverd)
        {
            // Draw hover background
            auto background = renderRule.hoverBackground;
            painter->setPen(Qt::NoPen);
            painter->setBrush(background);
            painter->drawRect(rect);
        }

        // Foreground
        painter->setRenderHints(QPainter::Antialiasing, false);
        painter->drawPixmap(pixmapRect, pixmap, pixmap.rect());

        painter->restore();
    }
}

void Button::updateVisible()
{
    auto clientRef = decoration()->client().toStrongRef();
    if (clientRef.isNull())
    {
        return;
    }

    auto visible = false;
    switch (type())
    {
    case Type::Close:
        visible = clientRef->isCloseable();
        break;
    case Type::Minimize:
        visible = clientRef->isMinimizeable();
        break;
    case Type::Maximize:
        visible = clientRef->isMaximizeable();
        break;
    default:
        visible = false;
        break;
    }

    if (isVisible() != visible)
    {
        setVisible(visible);
    }
}

}  // namespace KDecoration
}  // namespace Kiran
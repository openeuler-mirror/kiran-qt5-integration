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
#include "internel-setting.h"

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

// clang-format off
static QMap<Button::Type, ButtonRenderRule> buttonRenderRuleMap = {
    // {type, {icon, hoverBackground}}
    {
        KDecoration2::DecorationButtonType::Close,
        {
            QStringLiteral("window-close-symbolic.svg"),
            QColor(255, 0, 0, 200)
        }
    },
    {
        KDecoration2::DecorationButtonType::Maximize,
        {
            QStringLiteral("window-maximize-symbolic.svg"),
            QColor(65, 65, 65, 50)
        }
    },
    {
        KDecoration2::DecorationButtonType::Minimize,
        {
            QStringLiteral("window-minimize-symbolic.svg"),
            QColor(65, 65, 65, 50)
        }
    }
};
// clang-format on

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

    m_buttonSize = decoration->getInternelSetting()->buttonSize();
    m_decorationTheme = decoration->getInternelSetting()->decorationTheme();
    m_buttonRadius = decoration->getInternelSetting()->buttonRadius();
    m_iconSize = decoration->getInternelSetting()->iconSize();

    setGeometry(QRect(QPoint(0, 0), QSize(m_buttonSize, m_buttonSize)));
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
    Q_UNUSED(repaintRegion)
    
    auto renderRule = buttonRenderRuleMap.value(type(), ButtonRenderRule(QString(), QColor()));

    QIcon icon(QString(":/kdecoration/%1/%2").arg(m_decorationTheme).arg(renderRule.icon));

    if (!icon.isNull())
    {
        auto rect = geometry();
        
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        QPainterPath path;
        path.addRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), m_buttonRadius, m_buttonRadius);

        // 悬浮、点击状态下填充背景色
        bool hoverd = isHovered() || isPressed();
        if (hoverd)
        {
            painter->setPen(Qt::NoPen);
            painter->setBrush(renderRule.hoverBackground);
            painter->drawPath(path);
        }

        // Foreground - 居中绘制图标
        // 使用 iconSize 而不是 pixmap.size()，确保即使 pixmap 实际大小不匹配也能按预期大小绘制
        QRectF iconRect(QPoint(0, 0), QSizeF(m_iconSize, m_iconSize));
        iconRect.moveCenter(rect.center());
        icon.paint(painter, iconRect.toRect());
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
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
#pragma once
#include <KDecoration2/DecoratedClient>
#include <QObject>
#include <QVariantMap>
#include "decoration.h"

namespace Kiran
{
namespace KDecoration
{
class InternelSetting : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int titleBarHeight READ titleBarHeight WRITE setTitleBarHeight)
    Q_PROPERTY(int borderRadius READ borderRadius WRITE setBorderRadius)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
    Q_PROPERTY(int buttonRadius READ buttonRadius WRITE setButtonRadius)
    Q_PROPERTY(int buttonSize READ buttonSize WRITE setButtonSize)
    Q_PROPERTY(int buttonSpacing READ buttonSpacing WRITE setButtonSpacing)
    Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize)
    Q_PROPERTY(qreal scaleFactor READ scaleFactor)
public:
    explicit InternelSetting(QVariantMap settings = QVariantMap(),
                             Decoration *decoration = nullptr,
                             QObject *parent = nullptr);
    ~InternelSetting() override;
    static QStringList supportedThemes();
    QString decorationTheme() const
    {
        return m_decorationTheme;
    };
    int titleBarHeight() const
    {
        return qRound(m_titleBarHeight * m_scaleFactor);
    };
    int borderRadius() const
    {
        return qRound(m_borderRadius * m_scaleFactor);
    };
    int borderWidth() const
    {
        return qRound(m_borderWidth * m_scaleFactor);
    };
    int buttonRadius() const
    {
        return qRound(m_buttonRadius * m_scaleFactor);
    };
    int buttonSize() const
    {
        return qRound(m_buttonSize * m_scaleFactor);
    };
    int buttonSpacing() const
    {
        return m_buttonSpacing;
    };
    qreal scaleFactor() const
    {
        return m_scaleFactor;
    };
    int iconSize() const
    {
        return qRound(m_iconSize * m_scaleFactor);
    };
private:
    void loadSettings(QVariantMap settings);
    void setTitleBarHeight(int height)
    {
        m_titleBarHeight = height;
    }
    void setBorderRadius(int radius)
    {
        m_borderRadius = radius;
    }
    void setBorderWidth(int width)
    {
        m_borderWidth = width;
    }
    void setButtonRadius(int radius)
    {
        m_buttonRadius = radius;
    }
    void setButtonSize(int size)
    {
        m_buttonSize = size;
    }
    void setButtonSpacing(int spacing)
    {
        m_buttonSpacing = spacing;
    }
    void setIconSize(int size)
    {
        m_iconSize = size;
    }
private:
    Decoration *m_decoration = nullptr;
    QString m_decorationTheme;
    int m_titleBarHeight = 35;
    int m_borderRadius = 0;
    int m_borderWidth = 1;
    int m_buttonRadius = 0;
    int m_buttonSize = 35;
    int m_buttonSpacing = 0;
    int m_iconSize = 20;
    qreal m_scaleFactor = 1.0;
};
}  // namespace KDecoration
}  // namespace Kiran
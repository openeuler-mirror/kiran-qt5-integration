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
#include "internel-setting.h"
#include <kiran-session-daemon/settings-i.h>
#include <QDebug>
#include <QGSettings>
#include <QVariantMap>

#define SETTINGS_SCHEMA_WINDOW_SCALING_FACTOR "windowScalingFactor"
#define PREVIEW_CLIENT_CLASS_NAME "Kiran::Decoration::PreviewClient"

static QMap<QString, QMap<QString, QVariant>> themeConfig = {
    {"Kiran", 
        {
            {"titleBarHeight", 35}, 
            {"borderRadius", 0}, 
            {"borderWidth", 1}, 
            {"buttonRadius", 0}, 
            {"buttonSize", 35}, 
            {"buttonSpacing", 0},
            {"iconSize", 20}
        }
    },
    {"Kiran-Rounded", 
        {
            {"titleBarHeight", 40}, 
            {"borderRadius", 8}, 
            {"borderWidth", 1}, 
            {"buttonRadius", 24}, 
            {"buttonSize", 24}, 
            {"buttonSpacing", 10},
            {"iconSize", 20}
        }
    },
};

namespace Kiran
{
namespace KDecoration
{
InternelSetting::InternelSetting(QVariantMap settings,
                                 Decoration *decoration,
                                 QObject *parent)
    : QObject(parent)
{
    m_decoration = decoration;
    loadSettings(settings);
}

InternelSetting::~InternelSetting()
{
}

QStringList InternelSetting::supportedThemes()
{
    return themeConfig.keys();
}

void InternelSetting::loadSettings(QVariantMap settings)
{
    if (settings.contains("theme") && settings.value("theme").canConvert<QString>())
    {
        m_decorationTheme = settings.value("theme").toString();
    }

    if (!themeConfig.contains(m_decorationTheme))
    {
        qWarning() << "InternelSetting loadSettings" << m_decorationTheme
                   << "not found, use default theme";
        m_decorationTheme = themeConfig.firstKey();
    }

    QMap<QString, QVariant> config = themeConfig.value(m_decorationTheme);
    for (auto it = config.constBegin(); it != config.constEnd(); ++it)
    {
        const QByteArray propName = it.key().toLocal8Bit();
        setProperty(propName.constData(), it.value());
    }

    const auto decoratedClient = m_decoration->client().toStrongRef();
    if(decoratedClient.isNull() || QString::compare(decoratedClient->metaObject()->className(),
                                            PREVIEW_CLIENT_CLASS_NAME, Qt::CaseInsensitive) == 0)
    {
        m_scaleFactor = 1.0;
    }
    else
    {
        // KWin内部禁用Qt hidpi功能，内部实现通过读取窗口缩放因子来实现高DPI适配
        // NOTE: 后续需修改为根据字体高度计算标题栏高度，而不是直接放大
        QGSettings settings(SETTINGS_SCHEMA_ID, "", this);
        auto f = settings.get(SETTINGS_SCHEMA_WINDOW_SCALING_FACTOR).toInt();
        if (f == 2)
        {
            m_scaleFactor = 2.0;
        }
        else
        {
            m_scaleFactor = 1.0;
        }
    }
}
}  // namespace KDecoration
}  // namespace Kiran
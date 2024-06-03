/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     tangjie02 <tangjie02@kylinos.com.cn>
 */

#pragma once

#include <qpa/qplatformthemeplugin.h>

namespace Kiran
{
namespace Platformtheme
{
class Plugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "plugin.json")

public:
    explicit Plugin(QObject *parent = nullptr) : QPlatformThemePlugin(parent){};
    virtual ~Plugin() override = default;

    QPlatformTheme *create(const QString &key, const QStringList &paramList) override;
};

}  // namespace Platformtheme
}  // namespace Kiran

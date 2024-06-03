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

#include "plugins/platformtheme/plugin.h"
#include "plugins/platformtheme/theme.h"

#include <QCoreApplication>
#include <QSet>
#include <QDebug>

namespace Kiran
{
namespace Platformtheme
{
QPlatformTheme* Plugin::create(const QString& key, const QStringList& paramList)
{
    static QSet<QString> keySet = {"kiran-next", "kiran", "gnome", "gtk3", "qgnomeplatform"};

    if (keySet.contains(key))
    {
        qDebug() << "create kiran platformtheme for" << QCoreApplication::applicationName()
                 << " with key=" << key
                 << " and paramList=" << paramList;
        return new Theme(paramList);
    }

    return nullptr;
}

}  // namespace Platformtheme
}  // namespace Kiran
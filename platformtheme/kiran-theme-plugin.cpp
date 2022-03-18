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
#include "kiran-theme-plugin.h"
#include "kiran-theme.h"

#include <QDebug>

KiranThemePlugin::KiranThemePlugin(QObject *parent)
    : QPlatformThemePlugin(parent)
{
}

KiranThemePlugin::~KiranThemePlugin()
{
}

QPlatformTheme* KiranThemePlugin::create(const QString& key, const QStringList& paramList)
{
    static QSet<QString> keySet = {"kiran","gnome","gtk3","qgnomeplatform"};
    if(keySet.contains(key))
    {
        return new KiranTheme(paramList);
    }
    return nullptr;
}

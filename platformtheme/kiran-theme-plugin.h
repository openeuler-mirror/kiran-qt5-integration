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
#ifndef KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_PLUGIN_H_
#define KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_PLUGIN_H_

#include <qpa/qplatformthemeplugin.h>

class KiranThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "qkiran.json")
public:
    explicit KiranThemePlugin(QObject *parent=nullptr) :QPlatformThemePlugin(parent){};
    ~KiranThemePlugin() override = default;

    QPlatformTheme *create(const QString &key, const QStringList &paramList) override;

private:
};

#endif  //KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_PLUGIN_H_

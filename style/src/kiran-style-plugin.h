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
#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_PLUGIN_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_PLUGIN_H_

#include <QStylePlugin>

class KiranStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "../kiran-style-plugin.json")
public:
    explicit KiranStylePlugin(QObject* parent = nullptr);
    QStyle* create(const QString&) override;
};

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_PLUGIN_H_

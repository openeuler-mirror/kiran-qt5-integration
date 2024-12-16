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

#include "plugins/style/plugin.h"
#include <QCoreApplication>
#include <QDebug>
#include <QStyleFactory>
#include "lib/common/configuration.h"
#include "lib/theme/style.h"

namespace Kiran
{
namespace Style
{
Plugin::Plugin(QObject *parent) : QStylePlugin(parent)
{
}

QStyle *Plugin::create(const QString &key)
{
    // 对比平台主题名称，若为匹配则应用 Kiran 主题
    if (key.compare("kiran", Qt::CaseInsensitive) == 0)
    {
        auto appName = qAppName();
        if (!Kiran::Configuration::instance()->match(appName))
        {
            qDebug("%s not enable kiran style, create fusion style for it.", appName.toStdString().c_str());
            return QStyleFactory::create("fusion");
        }
        else
        {
            qDebug("create style: %s", key.toStdString().c_str());
            return new Kiran::Theme::Style();
        }
    }
    return nullptr;
}
}  // namespace Style
}  // namespace Kiran
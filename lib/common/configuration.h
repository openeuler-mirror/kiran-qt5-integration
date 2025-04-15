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
 * Author:     tangjie02 <tangjie02@kylinsec.com.cn>
 */

#pragma once

#include <QStringList>

class QSettings;

namespace Kiran
{
// 此类判断是否当前应用名称是否打开主题, 通过调用 match 方法匹配
//      1. 首先检测内置应用列表 m_kiranStyleApps
//      2. 从配置文件中读取应用列表 QSet
class Configuration
{
public:
    static Configuration* instance();
    Configuration(Configuration& other) = delete;
    Configuration& operator=(Configuration& other) = delete;

    ~Configuration() = default;

    // KQI_CONFIGURATION_PATH 示例
    // [Style]
    // #启用kiran style的应用列表,已内置部分kiran桌面环境的组件，eg: kiran-*,kiran-panel
    // enabled-apps=ksl-os-gui,ksl-server-gui,ks-scmc-gui

    // 在 getEnabledApps() 中通过获取到的 m_settings 转为 QSet 实现去重
    QSet<QString> getEnabledApps();
    // 通过应用名称判断是否是打开主题的应用（两种地方判断: m_kiranStyleApps 以及 getEnabledApps 中的）
    bool match(const QString& appName);

private:
    Configuration();

private:
    QStringList m_kiranStyleApps = {
        "kiran-control-panel",
        "kiran-cpanel-*",
        "kiran-polkit-agent",
        "kiran-avatar-editor",
        "kiran-calculator",
        "kiran-power-status-icon",
        "kiran-network-status-icon",
        "kiran-audio-status-icon",
        "kiran-flameshot",
        "kiran-manual"
    };
//    "kiran-integration-example"

    QSettings* m_settings{};
};

}  // namespace Kiran
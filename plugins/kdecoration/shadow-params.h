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
#include <QPoint>

namespace Kiran
{
namespace KDecoration
{
// 阴影参数结构体，描述单层阴影的偏移、半径和透明度
struct ShadowParams
{
    ShadowParams() = default;
    ShadowParams(const QPoint &offset, int radius, qreal opacity)
        : offset(offset), radius(radius), opacity(opacity) {}
    QPoint offset;  // 阴影相较于主体x/y轴偏移
    int radius = 0;
    qreal opacity = 0;
};

// 组合阴影参数结构体，包含主体阴影和对比阴影
struct CompositeShadowParams
{
    CompositeShadowParams() = default;
    CompositeShadowParams(
        const QPoint &_offset,
        const ShadowParams &_shapeShadow,     // 主体阴影
        const ShadowParams &_contrastShadow)  // 对比阴影
        : offset(_offset), shape(_shapeShadow), contrast(_contrastShadow)
    {
    }
    QPoint offset;
    ShadowParams shape;
    ShadowParams contrast;
};

}  // namespace KDecoration
}  // namespace Kiran

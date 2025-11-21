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
#include <QFlags>
#include <QPainterPath>
#include <QRect>
#include <QImage>
#include <QPainter>
#include <QColor>
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

// 圆角位置标志
enum CornerPosition
{
    NoCorner = 0x00,
    TopLeftCorner = 0x01,      // 左上角
    TopRightCorner = 0x02,     // 右上角
    BottomLeftCorner = 0x04,   // 左下角
    BottomRightCorner = 0x08,  // 右下角
    AllCorners = TopLeftCorner | TopRightCorner | BottomLeftCorner | BottomRightCorner
};
Q_DECLARE_FLAGS(CornerPositions, CornerPosition)
Q_DECLARE_OPERATORS_FOR_FLAGS(CornerPositions)

// 圆角大小定义
struct CornerRadii
{
    int topLeft;      // 左上角圆角半径
    int topRight;     // 右上角圆角半径
    int bottomLeft;   // 左下角圆角半径
    int bottomRight;  // 右下角圆角半径

    CornerRadii()
        : topLeft(0), topRight(0), bottomLeft(0), bottomRight(0)
    {
    }

    CornerRadii(int radius)
        : topLeft(radius), topRight(radius), bottomLeft(radius), bottomRight(radius)
    {
    }

    CornerRadii(int topLeft, int topRight, int bottomLeft, int bottomRight)
        : topLeft(topLeft), topRight(topRight), bottomLeft(bottomLeft), bottomRight(bottomRight)
    {
    }
};


namespace Utils
{
/**
 * @brief 创建指定圆角的矩形路径
 * @param rect 矩形区域
 * @param cornerRadii 各角的圆角半径
 * @param corners 需要应用圆角的位置（使用 CornerPositions 枚举组合）
 * @param extension 扩展像素数（用于与相邻区域重叠）
 * @return 圆角矩形路径
 */
QPainterPath createRoundedRectPath(const QRect &rect,
                                    const CornerRadii &cornerRadii,
                                    CornerPositions corners = AllCorners,
                                    qreal extension = 0.0);
/**
 * @brief 在指定区域绘制带有模糊效果的矩形阴影
 * @param p 绘图设备
 * @param box 矩形区域
 * @param offset 阴影偏移
 * @param radius 阴影半径
 * @param color 阴影颜色
 */
void boxShadow(QPainter *p, const QRect &box, const QPoint &offset,
                                        int radius, const QColor &color);

/**
 * @brief 在指定路径绘制带有模糊效果的阴影（支持圆角）
 * @param p 绘图设备
 * @param path 路径
 * @param offset 阴影偏移
 * @param radius 阴影半径
 * @param color 阴影颜色
 */
void boxShadow(QPainter *p, const QPainterPath &path, const QPoint &offset,
                                        int radius, const QColor &color);

}  // namespace Utils
}  // namespace KDecoration
}  // namespace Kiran


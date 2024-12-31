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

#pragma once

#include <QFlags>
#include <QObject>

namespace Kiran
{
Q_NAMESPACE
enum ArrowOrientation
{
    Arrow_Up,
    Arrow_Down,
    Arrow_Left,
    Arrow_Right
};
enum Corner
{
    CornerTopLeft = 0x1,
    CornerTopRight = 0x2,
    CornerBottomLeft = 0x4,
    CornerBottomRight = 0x8,
    CornersTop = CornerTopLeft | CornerTopRight,
    CornersBottom = CornerBottomLeft | CornerBottomRight,
    CornersLeft = CornerTopLeft | CornerBottomLeft,
    CornersRight = CornerTopRight | CornerBottomRight,
    AllCorners = CornerTopLeft | CornerTopRight | CornerBottomLeft | CornerBottomRight,
};
Q_DECLARE_FLAGS(Corners, Corner)

#define KIRAN_STYLE_PROPERTY_BUTTON_TYPE "_kiran_button_type"

}  // namespace Kiran

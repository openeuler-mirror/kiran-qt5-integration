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
#include <QtCore/qobjectdefs.h>
#include <KDecoration2/DecorationButton>
#include "decoration.h"

namespace Kiran
{
namespace KDecoration
{
class Decoration;
class Button : public KDecoration2::DecorationButton
{
    Q_OBJECT
public:
    using Type = KDecoration2::DecorationButtonType;
    Button(Type type, Decoration *decoration, QObject *parent = nullptr);
    ~Button() override;

    static bool isSupported(Type type);
    void paint(QPainter *painter, const QRect &repaintRegion) override;

private slots:
    void updateVisible();
};
}  // namespace KDecoration
}  // namespace Kiran
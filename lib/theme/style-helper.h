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

#define DEFAULT_STYLE_HELPER StyleHelper::getDefault

namespace Kiran
{
namespace Theme
{

enum PaletteType
{
    PALETTE_LIGHT,
    PALETTE_DARK,
    PALETTE_LAST
};

class StyleHelper
{
public:
    explicit StyleHelper();
    ~StyleHelper();

    static StyleHelper* getDefault();

    static PaletteType paletteType();
    static void doChangeTheme(PaletteType paletteType);

private:
    static StyleHelper* m_instance;
};
}  // namespace Theme
}  // namespace Kiran
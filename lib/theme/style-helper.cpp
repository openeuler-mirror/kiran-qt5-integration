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
#include "style-helper.h"
#include "palette.h"

namespace Kiran
{
namespace Theme
{
StyleHelper::StyleHelper() = default;
StyleHelper::~StyleHelper() = default;

StyleHelper* StyleHelper::m_instance = nullptr;
StyleHelper* StyleHelper::getDefault()
{
    if (!StyleHelper::m_instance)
    {
        StyleHelper::m_instance = new StyleHelper();
    }
    return StyleHelper::m_instance;
}

PaletteType StyleHelper::paletteType()
{
    auto baseColors = DEFAULT_PALETTE()->getBaseColors();
    if (baseColors.baseBackground == Kiran::Theme::g_lightBaseColors.baseBackground)
    {
        return PaletteType::PALETTE_LIGHT;
    }
    else if (baseColors.baseBackground == Kiran::Theme::g_darkBaseColors.baseBackground)
    {
        return PaletteType::PALETTE_DARK;
    }
    else
    {
        return PaletteType::PALETTE_DARK;
    }
}

void StyleHelper::doChangeTheme(PaletteType paletteType)
{
    Palette::BaseColors baseColors;
    if (paletteType == PaletteType::PALETTE_LIGHT)
    {
        baseColors = Kiran::Theme::g_lightBaseColors;
    }
    else if (paletteType == PaletteType::PALETTE_DARK)
    {
        baseColors = Kiran::Theme::g_darkBaseColors;
    }
    DEFAULT_PALETTE()->setBaseColors(baseColors);
}
}  // namespace Theme
}  // namespace Kiran

//
// Created by skyzcyou on 2024/4/1.
//

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

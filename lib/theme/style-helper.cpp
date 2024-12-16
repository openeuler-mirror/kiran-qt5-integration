//
// Created by skyzcyou on 2024/4/1.
//

#include "style-helper.h"

//extern Kiran::Theme::Palette::BaseColors g_lightBaseColors;
//extern Kiran::Theme::Palette::BaseColors g_darkBaseColors;

Kiran::Theme::Palette::BaseColors g_lightBaseColors = {.baseBackground = QColor(232, 232, 232),    // #E8E8E8
    .baseForeground = QColor(34, 34, 34),       // #222222
    .widgetBackground = QColor(239, 239, 239),  // #EFEFEF
    .widgetBorder = QColor(204, 204, 204),      // #CCCCCC
    .widgetSelection = QColor(0, 162, 255),     // #00A2FF
    .widgetMain = QColor(46, 179, 255),         // #2EB3FF
    .widgetWarning = QColor(250, 73, 73)};      // #FA4949

Kiran::Theme::Palette::BaseColors g_darkBaseColors = {.baseBackground = QColor(34, 34, 34),     // #222222
    .baseForeground = QColor(255, 255, 255),  // #FFFFFF
    .widgetBackground = QColor(57, 57, 57),   // #393939
    .widgetBorder = QColor(69, 69, 69),       // #454545
    .widgetSelection = QColor(0, 162, 255),   // #00A2FF
    .widgetMain = QColor(46, 179, 255),       // #2EB3FF
    .widgetWarning = QColor(250, 73, 73)};    // #FA4949

namespace Kiran
{
namespace Theme
{
StyleHelper::StyleHelper()
= default;
StyleHelper::~StyleHelper()= default;

void StyleHelper::doChangeTheme(Kiran::PaletteType paletteType)
{
    Palette::BaseColors baseColors;
    if (paletteType == PaletteType::PALETTE_LIGHT) {
        baseColors = g_lightBaseColors;
    } else if (paletteType == PaletteType::PALETTE_DARK) {
        baseColors = g_darkBaseColors;
    }
    DEFAULT_PALETTE()->setBaseColors(baseColors);
}
} // namespace Theme
} // namespace Kiran

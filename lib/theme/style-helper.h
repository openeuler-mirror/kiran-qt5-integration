#pragma once

#include "lib/theme/palette.h"
#include "lib/theme/define.h"


namespace Kiran
{
namespace Theme
{
class StyleHelper
{
public:
    explicit StyleHelper();
    ~StyleHelper();
    static void doChangeTheme(Kiran::PaletteType paletteType);
};
} // namespace Theme
} // namespace Kiran
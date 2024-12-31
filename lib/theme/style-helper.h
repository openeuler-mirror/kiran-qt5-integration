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
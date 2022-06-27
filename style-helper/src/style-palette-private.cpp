#include "style-palette-private.h"
#include <kiran-appearance-monitor.h>

namespace Kiran
{
StylePalettePrivate::StylePalettePrivate(QObject* parent, StylePalette* ptr)
    : QObject(parent),
      q_ptr(ptr),
      m_customSpecialType(false),
      m_type(Kiran::PALETTE_DARK),
      m_schemeLoader(new Kiran::SchemeLoader)
{
    // 监听Kiran桌面主题变化信号
    connect(KiranAppearanceMonitor::instance(), &KiranAppearanceMonitor::gtkThemeChanged,
            this, &StylePalettePrivate::handleGtkThemeChanged);
    handleGtkThemeChanged(KiranAppearanceMonitor::instance()->gtkTheme());
}

StylePalettePrivate::~StylePalettePrivate()
{
    delete m_schemeLoader;
}

void StylePalettePrivate::handleGtkThemeChanged(const QString& gtkTheme)
{
    // 手动指定深浅主题
    if (m_customSpecialType)
    {
        return;
    }
    else
    {
        Kiran::PaletteType type;
        if (gtkTheme.compare("kiran") == 0)
        {
            type = Kiran::PALETTE_LIGHT;
        }
        else
        {
            type = Kiran::PALETTE_DARK;
        }
        updatePaletteType(type, false);
    }
}

QString StylePalettePrivate::getSchemeFile(Kiran::PaletteType type)
{
    // clang-format off
    static QString schemeFile[Kiran::PALETTE_LAST] = {
        QStringLiteral(":/style-helper/colors/light.css"),
        QStringLiteral(":/style-helper/colors/dark.css")
    };
    // clang-format on
    return schemeFile[type];
}

void StylePalettePrivate::updatePaletteType(Kiran::PaletteType type, bool customSpecial)
{
    m_customSpecialType = customSpecial;
    m_type = type;
    QString schemeFile = getSchemeFile(type);
    m_schemeLoader->load(schemeFile);

    emit q_ptr->themeChanged(type);
}
}  // namespace Kiran

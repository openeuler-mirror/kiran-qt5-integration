#include "kiran-palette-private.h"
#include <kiran-appearance-monitor.h>

KiranPalettePrivate::KiranPalettePrivate(QObject* parent, KiranPalette* ptr)
    : QObject(parent),
      q_ptr(ptr),
      m_customSpecialType(false),
      m_type(KiranStyle::PALETTE_DARK),
      m_schemeLoader(new Kiran::Style::SchemeLoader)
{
    //监听Kiran桌面主题变化信号
    connect(KiranAppearanceMonitor::instance(),&KiranAppearanceMonitor::gtkThemeChanged,
            this,&KiranPalettePrivate::handleGtkThemeChanged);
    handleGtkThemeChanged(KiranAppearanceMonitor::instance()->gtkTheme());
}

KiranPalettePrivate::~KiranPalettePrivate()
{
    delete m_schemeLoader;
}

void KiranPalettePrivate::handleGtkThemeChanged(const QString& gtkTheme)
{
    //手动指定深浅主题
    if( m_customSpecialType )
    {
        return;
    }
    else
    {
        KiranStyle::PaletteType type;
        if( gtkTheme.compare("kiran") == 0 )
        {
            type = KiranStyle::PALETTE_LIGHT;
        }
        else
        {
            type = KiranStyle::PALETTE_DARK;
        }
        updatePaletteType(type,false);
    }
}

QString KiranPalettePrivate::getSchemeFile(KiranStyle::PaletteType type)
{
    // clang-format off
    static QString schemeFile[KiranStyle::PALETTE_LAST] = {
        QStringLiteral(":/style-helper/colors/light.css"),
        QStringLiteral(":/style-helper/colors/dark.css")
    };
    // clang-format on
    return schemeFile[type];
}

void KiranPalettePrivate::updatePaletteType(KiranStyle::PaletteType type, bool customSpecial)
{
    m_customSpecialType = customSpecial;
    m_type = type;
    QString schemeFile = getSchemeFile(type);
    m_schemeLoader->load(schemeFile);
}

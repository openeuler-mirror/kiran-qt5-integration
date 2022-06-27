#pragma once
#include <QObject>
#include "scheme-loader.h"
#include "style-palette.h"

namespace Kiran
{
class StylePalettePrivate : public QObject
{
    Q_DECLARE_PUBLIC(StylePalette)
public:
    StylePalettePrivate(QObject* parent, StylePalette* ptr);;
    ~StylePalettePrivate();

    QString getSchemeFile(Kiran::PaletteType type);
    void updatePaletteType(Kiran::PaletteType type,bool customSpecial=false);

private slots:
    void handleGtkThemeChanged(const QString& gtkTheme);

private:
    StylePalette* q_ptr;
    Kiran::SchemeLoader* m_schemeLoader = nullptr;
    bool m_customSpecialType = false;
    Kiran::PaletteType m_type;
};
}

#pragma once
#include "kiran-palette.h"
#include "scheme-loader.h"
#include <QObject>

class KiranPalettePrivate : public QObject
{
    Q_DECLARE_PUBLIC(KiranPalette)
public:
    KiranPalettePrivate(QObject* parent,KiranPalette* ptr);;
    ~KiranPalettePrivate();

    QString getSchemeFile(Kiran::Style::PaletteType type);
    void updatePaletteType(Kiran::Style::PaletteType type,bool customSpecial=false);

private slots:
    void handleGtkThemeChanged(const QString& gtkTheme);

private:
    KiranPalette* q_ptr;
    Kiran::Style::SchemeLoader* m_schemeLoader = nullptr;
    bool m_customSpecialType = false;
    Kiran::Style::PaletteType m_type;
};
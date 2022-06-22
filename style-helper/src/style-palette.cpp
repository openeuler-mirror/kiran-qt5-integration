#include "style-palette.h"
#include "kiran-appearance-monitor.h"
#include "scheme-loader.h"
#include "style-palette-private.h"

#include <QColor>
#include <QDebug>
#include <QMetaEnum>
#include <QPalette>
#include <QWidget>

namespace Kiran
{
static const QMap<StylePalette::ColorState, Kiran::PseudoClassType> pseudoClassMap = {
    {StylePalette::Normal, Kiran::PseudoClass_Unspecified},
    {StylePalette::Active, Kiran::PseudoClass_Active},
    {StylePalette::Checked, Kiran::PseudoClass_Checked},
    {StylePalette::Hover, Kiran::PseudoClass_Hover},
    {StylePalette::Disabled, Kiran::PseudoClass_Disabled}};

StylePalette* StylePalette::instance()
{
    static QMutex mutex;
    static QScopedPointer<StylePalette> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new StylePalette);
        }
    }

    return pInst.data();
}

StylePalette::StylePalette(QObject* parent)
    : QObject(parent),
      d_ptr(new StylePalettePrivate(this, this))
{
}

StylePalette::~StylePalette()
{
}

void StylePalette::polishPalette(QPalette* palette)
{
    d_ptr->m_schemeLoader->polish(palette);
}

Kiran::PaletteType StylePalette::paletteType()
{
    return d_ptr->m_type;
}

void StylePalette::setDesignatedPaletteType(Kiran::PaletteType type)
{
    d_ptr->updatePaletteType(type, true);
}

QColor StylePalette::color(ColorStateFlags states, WidgetType type, WidgetColorRule rule)
{
    QColor defaultColor;

    static QMetaEnum metaEnum = QMetaEnum::fromType<Kiran::SchemeLoader::SchemePropertyName>();
    const char* key = metaEnum.valueToKey(type + rule);
    if (key == nullptr)
    {
        qWarning() << "KiranPalette: can't convert <KiranPalette::WidgetType+KiranPalette::WidgetColorRule> -> SchemeLoader::SchemePropertyName!" << type << rule;
        return defaultColor;
    }
    auto propertyEnum = static_cast<SchemeLoader::SchemePropertyName>(type + rule);

    int64_t pseudoClass = PseudoClass_Unspecified;
    for (auto iter = pseudoClassMap.begin(); iter != pseudoClassMap.end(); iter++)
    {
        if (states & iter.key())
        {
            pseudoClass |= iter.value();
        }
    }

    return d_ptr->m_schemeLoader->getColor(propertyEnum, pseudoClass);
}

QColor StylePalette::color(FlagColorRule rule)
{
    QColor defaultColor;

    static QMetaEnum metaEnum = QMetaEnum::fromType<Kiran::SchemeLoader::SchemePropertyName>();
    const char* key = metaEnum.valueToKey(rule);
    if (key == nullptr)
    {
        qWarning() << "KiranPalette: can't convert <KiranPalette::FlagColorRule> -> SchemeLoader::SchemePropertyName!" << rule;
        return defaultColor;
    }
    auto propertyEnum = static_cast<SchemeLoader::SchemePropertyName>(rule);

    return d_ptr->m_schemeLoader->getColor(propertyEnum, PseudoClass_Unspecified);
}

QColor StylePalette::color(const QWidget* widget,
                           const QStyleOption* option,
                           StylePalette::WidgetType type,
                           StylePalette::WidgetColorRule rule,
                           StylePalette::ColorState specialState)
{
    QColor defaultColor;

    static QMetaEnum metaEnum = QMetaEnum::fromType<Kiran::SchemeLoader::SchemePropertyName>();
    const char* key = metaEnum.valueToKey(type + rule);
    if (key == nullptr)
    {
        qWarning() << "KiranPalette: can't convert <KiranPalette::WidgetType+KiranPalette::WidgetColorRule> -> SchemeLoader::SchemePropertyName!";
        return defaultColor;
    }
    auto propertyEnum = static_cast<SchemeLoader::SchemePropertyName>(type + rule);

    auto pseudoIter = pseudoClassMap.find(specialState);
    if (pseudoIter == pseudoClassMap.end())
    {
        return defaultColor;
    }
    int64_t pseudoClass = pseudoIter.value();

    QColor color = d_ptr->m_schemeLoader->getColor(widget, option, propertyEnum, pseudoClass);
    if (!color.isValid())
    {
        qWarning() << "can't get color for:\n"
                   << "\twidget:       " << widget << "\n"
                   << "\tproperty:     " << type << rule << "-->" << propertyEnum << "\n"
                   << "\tspecial state:" << specialState << "-->" << pseudoClass << "\n";
    }
    return color;
}

void StylePalette::dump()
{
    d_ptr->m_schemeLoader->dump();
}

Kiran::SchemeLoader* StylePalette::getSchemeLoader()
{
    return d_ptr->m_schemeLoader;
}

bool StylePalette::isDesignatedPaletteType()
{
    return d_ptr->m_customSpecialType;
}

}  // namespace Kiran
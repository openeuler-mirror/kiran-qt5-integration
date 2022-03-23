#include "kiran-palette.h"
#include "kiran-palette-private.h"
#include "scheme-loader.h"
#include "kiran-appearance-monitor.h"

#include <QPalette>
#include <QMetaEnum>
#include <QColor>
#include <QDebug>
#include <QWidget>

using namespace Kiran::Style;

static const QMap<KiranPalette::ColorState,Kiran::Style::PseudoClassType> pseudoClassMap = {
    {KiranPalette::Normal,Kiran::Style::PseudoClass_Unspecified},
    {KiranPalette::Active,Kiran::Style::PseudoClass_Active},
    {KiranPalette::Checked,Kiran::Style::PseudoClass_Checked},
    {KiranPalette::Hover,Kiran::Style::PseudoClass_Hover},
    {KiranPalette::Disabled,Kiran::Style::PseudoClass_Disabled}  
};

KiranPalette* KiranPalette::instance()
{
    static QMutex mutex;
    static QScopedPointer<KiranPalette> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new KiranPalette);
        }
    }

    return pInst.data();
}

KiranPalette::KiranPalette(QObject* parent)
    : QObject(parent),
      d_ptr(new KiranPalettePrivate(this,this))
{

}

KiranPalette::~KiranPalette()
{

}

void KiranPalette::polishPalette(QPalette* palette)
{
    d_ptr->m_schemeLoader->polish(palette);
}

Kiran::Style::PaletteType KiranPalette::paletteType()
{
    return d_ptr->m_type;
}

void KiranPalette::setDesignatedPaletteType(Kiran::Style::PaletteType type)
{
    d_ptr->updatePaletteType(type,true);
}

QColor KiranPalette::color(ColorState state,WidgetType type,WidgetColorRule rule)
{
    QColor defaultColor;

    static QMetaEnum metaEnum = QMetaEnum::fromType<Kiran::Style::SchemeLoader::SchemePropertyName>();
    const char*  key = metaEnum.valueToKey(type+rule);
    if( key == nullptr )
    {
        qWarning() << "KiranPalette: can't convert <KiranPalette::WidgetType+KiranPalette::WidgetColorRule> -> SchemeLoader::SchemePropertyName!" << type << rule;
        return defaultColor;
    }
    auto propertyEnum = static_cast<SchemeLoader::SchemePropertyName>(type+rule);

    auto pseudoIter = pseudoClassMap.find(state);
    if( pseudoIter == pseudoClassMap.end() )
    {
        return defaultColor;
    }
    Kiran::Style::PseudoClassType pseudoClass = pseudoIter.value();

    return d_ptr->m_schemeLoader->getColor(propertyEnum,pseudoClass);
}

QColor KiranPalette::color(FlagColorRule rule)
{
    QColor defaultColor;

    static QMetaEnum metaEnum = QMetaEnum::fromType<Kiran::Style::SchemeLoader::SchemePropertyName>();
    const char*  key = metaEnum.valueToKey(rule);
    if( key == nullptr )
    {
        qWarning() << "KiranPalette: can't convert <KiranPalette::FlagColorRule> -> SchemeLoader::SchemePropertyName!" << rule;
        return defaultColor;
    }
    auto propertyEnum = static_cast<SchemeLoader::SchemePropertyName>(rule);

    return d_ptr->m_schemeLoader->getColor(propertyEnum,PseudoClass_Unspecified);
}

QColor KiranPalette::color(const QWidget* widget,
                           const QStyleOption* option,
                           KiranPalette::WidgetType type,
                           KiranPalette::WidgetColorRule rule,
                           KiranPalette::ColorState specialState)
{
    QColor defaultColor;

    static QMetaEnum metaEnum = QMetaEnum::fromType<Kiran::Style::SchemeLoader::SchemePropertyName>();
    const char*  key = metaEnum.valueToKey(type+rule);
    if( key == nullptr )
    {
        qWarning() << "KiranPalette: can't convert <KiranPalette::WidgetType+KiranPalette::WidgetColorRule> -> SchemeLoader::SchemePropertyName!";
        return defaultColor;
    }
    auto propertyEnum = static_cast<SchemeLoader::SchemePropertyName>(type+rule);

    auto pseudoIter = pseudoClassMap.find(specialState);
    if( pseudoIter == pseudoClassMap.end() )
    {
        return defaultColor;
    }
    int64_t pseudoClass = pseudoIter.value();

    QColor color = d_ptr->m_schemeLoader->getColor(widget,option,propertyEnum,pseudoClass);
    if( !color.isValid() )
    {
        qWarning() << "can't get color for:\n"
                   << "\twidget:       " << widget << "\n"
                   << "\tproperty:     " << type << rule << "-->" << propertyEnum << "\n"
                   << "\tspecial state:" << specialState << "-->" << pseudoClass << "\n";
    }
    return color;
}

#include "kiran-palette.h"
#include "scheme-loader.h"
#include "scheme-loader-private.h"

#include <QPalette>
#include <QMetaEnum>
#include <QColor>

using namespace Kiran::Style;

static const QMap<KiranPalette::ColorState,Kiran::Style::PseudoClassType> pseudoClassMap = {
    {KiranPalette::Normal,Kiran::Style::PseudoClass_Unspecified},
    {KiranPalette::Active,Kiran::Style::PseudoClass_Active},
    {KiranPalette::Hover,Kiran::Style::PseudoClass_Hover},
    {KiranPalette::Disabled,Kiran::Style::PseudoClass_Disabled}  
};

KiranPalette::KiranPalette(QObject* parent)
    :QObject(parent)
{

}

KiranPalette::~KiranPalette()
{
    
}

QColor KiranPalette::color(ColorState state,WidgetType type,WidgetColorRule rule)
{
    QColor defaultColor;

    //css selector 选择器
    static const QMetaEnum widgetTypeMetaEnum = QMetaEnum::fromType<WidgetType>();
    const char* typeString = widgetTypeMetaEnum.key(type);
    if( typeString == nullptr )
    {
        return defaultColor;
    }

    //css 属性
    static  const QMetaEnum colorRuleMetaEnum = QMetaEnum::fromType<WidgetColorRule>();
    const char* cssProperty = colorRuleMetaEnum.key(rule);
    if(cssProperty == nullptr )
    {
        return defaultColor;
    }

    //通过css选择器与css属性拼接出SchemeLoader之中预定义的属性
    QString schemePropertyName = QString("%1_%2").arg(typeString).arg(cssProperty);

    //将KiranPalette定义的状态枚举转换成qss之中的伪状态选择器
    auto pseudoIter = pseudoClassMap.find(state);
    if( pseudoIter == pseudoClassMap.end() )
    {
        return defaultColor;
    }
    Kiran::Style::PseudoClassType pseudoClass = pseudoIter.value();

    //通过SchemeLoader拿出颜色表之中所定义的颜色
    return SchemeLoader::instance()->getColor(schemePropertyName,pseudoClass);
}

QColor KiranPalette::color(ColorState state,FlagColorRule rule)
{
    QColor defaultColor;
    static const char* propertyPrefix = "Global";

    static const QMetaEnum flagRuleMetaEnum = QMetaEnum::fromType<FlagColorRule>();
    const char* cssProperty = flagRuleMetaEnum.key(rule);
    if( cssProperty == nullptr )
    {
        return defaultColor;
    }

    //通过css选择器与css属性拼接出SchemeLoader之中预定义的属性
    QString schemePropertyName = QString("%1_%2").arg(propertyPrefix).arg(cssProperty);

    //将KiranPalette定义的状态枚举转换成qss之中的伪状态选择器
    auto pseudoIter = pseudoClassMap.find(state);
    if( pseudoIter == pseudoClassMap.end() )
    {
        return defaultColor;
    }
    Kiran::Style::PseudoClassType pseudoClass = pseudoIter.value();

    return SchemeLoader::instance()->getColor(schemePropertyName,pseudoClass);
}
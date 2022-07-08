/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */
#include "scheme-loader.h"
#include "kiran-appearance-monitor.h"

#include <private/qcssparser_p.h>
#include <QDebug>
#include <QMetaEnum>
#include <QStyle>
#include <QStyleOption>

using namespace QCss;


namespace Kiran
{
static const QMap<quint64, QString> pseudoMap = {
    {QCss::PseudoClass_Unknown, "unknow"},
    {QCss::PseudoClass_Enabled, "enabled"},
    {QCss::PseudoClass_Disabled, "disabled"},
    {QCss::PseudoClass_Pressed, "pressed"},
    {QCss::PseudoClass_Focus, "focus"},
    {QCss::PseudoClass_Hover, "hover"},
    {QCss::PseudoClass_Checked, "checked"},
    {QCss::PseudoClass_Unchecked, "unchecked"},
    {QCss::PseudoClass_Indeterminate, "indeterminate"},
    {QCss::PseudoClass_Unspecified, "unspecifie"},
    {QCss::PseudoClass_Selected, "selected"},
    {QCss::PseudoClass_Horizontal, "horizontal"},
    {QCss::PseudoClass_Vertical, "vertical"},
    {QCss::PseudoClass_Window, "window"},
    {QCss::PseudoClass_Children, "chindren"},
    {QCss::PseudoClass_Sibling, "sibling"},
    {QCss::PseudoClass_Default, "default"},
    {QCss::PseudoClass_First, "first"},
    {QCss::PseudoClass_Last, "last"},
    {QCss::PseudoClass_Middle, "middle"},
    {QCss::PseudoClass_OnlyOne, "only one"},
    {QCss::PseudoClass_PreviousSelected, "previousSelected"},
    {QCss::PseudoClass_NextSelected, "NextSelected"},
    {QCss::PseudoClass_Flat, "flat"},
    {QCss::PseudoClass_Left, "left"},
    {QCss::PseudoClass_Right, "right"},
    {QCss::PseudoClass_Top, "top"},
    {QCss::PseudoClass_Bottom, "bottom"},
    {QCss::PseudoClass_Exclusive, "exclusive"},
    {QCss::PseudoClass_NonExclusive, "nonExclusive"},
    {QCss::PseudoClass_Frameless, "frameless"},
    {QCss::PseudoClass_ReadOnly, "readOnly"},
    {QCss::PseudoClass_Active, "active"},
    {QCss::PseudoClass_Closable, "closable"},
    {QCss::PseudoClass_Movable, "movable"},
    {QCss::PseudoClass_Floatable, "floatable"},
    {QCss::PseudoClass_Minimized, "minimized"},
    {QCss::PseudoClass_Maximized, "maximized"},
    {QCss::PseudoClass_On, "on"},
    {QCss::PseudoClass_Off, "off"},
    {QCss::PseudoClass_Editable, "editable"},
    {QCss::PseudoClass_Item, "item"},
    {QCss::PseudoClass_Closed, "closed"},
    {QCss::PseudoClass_Open, "open"},
    {QCss::PseudoClass_EditFocus, "editFocus"},
    {QCss::PseudoClass_Alternate, "alternate"},
    {QCss::PseudoClass_Any, "any"},
};

bool FuzzyMatch(const QVector<QCss::Selector>& selectors, quint64 pseudoClass);

Kiran::SchemeLoader::SchemeLoader(QObject* parent)
    : QObject(parent),
      m_isValid(false),
      m_styleScheme(new QCss::StyleSheet)
{

}

SchemeLoader::~SchemeLoader()
{
    delete m_styleScheme;
}

QVariant SchemeLoader::fetchPropertyValue(const QWidget* widget,
                                          const QStyleOption* opt,
                                          SchemeLoader::SchemePropertyName name,
                                          SchemeLoader::SchemeValueType valueType,
                                          quint64 specialPseudlClasss)
{
    /// 获取没伪选择器的样式
    QVariant defaultValue = fetchPropertyValue(name, QCss::PseudoClass_Unspecified, valueType);

    /// 获取当前控件的其他状态 用于辅助增加伪状态选择匹配的准确性
    quint64 styleOptionPseudoClass = convertStyleOption2PseudoClass(widget, opt);
    quint64 extendPresudoClass = styleOptionPseudoClass | specialPseudlClasss;

    ///　关键状态,做为匹配的关键因素
    quint64 keyClass = 0;
    if (!(opt->state & QStyle::State_Enabled))
    {  /// disable状态
        keyClass = QCss::PseudoClass_Disabled;
    }
    else if (opt->state & QStyle::State_Sunken)
    {  /// 凹陷或按下
        keyClass = QCss::PseudoClass_Pressed;
    }
    else if (opt->state & QStyle::State_MouseOver)
    {  /// 鼠标悬浮
        keyClass = QCss::PseudoClass_Hover;
    }
    else if (opt->state & QStyle::State_HasFocus)
    {  /// 聚焦　
        keyClass = QCss::PseudoClass_Focus;
    }

#if 0
    QMetaEnum enumInfo = QMetaEnum::fromType<SchemePropertyName>();
    qInfo() << "fetch style detail:" << "\n"
            << "    widget class:  "   << widget->metaObject()->className() << "\n"
            << "    widget:        "   << widget->objectName() << "\n"
            << "    selector:      "   << enumInfo.valueToKey(name) << "\n"
            << "    special pseudo:"   << pseudoClass2String(specialPseudlClasss) << "\n"
            << "    key pseudo:    "   << pseudoClass2String(keyClass) << "\n"
            << "    extend pseudo: "   << pseudoClass2String(extendPresudoClass);
#endif

    if (keyClass != 0)
    {
        QVariant bestFitVar = fetchPropertyValue(name, keyClass | extendPresudoClass, valueType);
        if (bestFitVar.isValid())
        {
            return bestFitVar;
        }
        QVariant keyVar = fetchPropertyValue(name, keyClass, valueType);
        if (keyVar.isValid())
        {
            return keyVar;
        }
    }

    if (extendPresudoClass != 0)
    {
        QVariant extentVar = fetchPropertyValue(name, extendPresudoClass, valueType);
        if (extentVar.isValid())
        {
            return extentVar;
        }
    }

    if (specialPseudlClasss != 0)
    {
        QVariant specialVar = fetchPropertyValue(name, specialPseudlClasss, valueType);
        if (specialVar.isValid())
        {
            return specialVar;
        }
    }

    return defaultValue;
}

QVariant SchemeLoader::fetchPropertyValue(SchemeLoader::SchemePropertyName propertyName,
                                          quint64 pseudoClass,
                                          SchemeLoader::SchemeValueType valueType)
{
    QReadLocker readLocker(&m_rwLock);
    QVariant var = searchCacheEntry(propertyName, pseudoClass);
    if (var.isValid())
    {
        return var;
    }

    //使用元对象中的信息将selector转换成字符串
    QMetaEnum metaEnum = QMetaEnum::fromType<SchemePropertyName>();
    QString strSelector = metaEnum.valueToKey(propertyName);

    //将字符串拆分成类型选择器和属性名进行匹配
    const QStringList selectors = strSelector.split("_");
    const QString type = selectors.at(0);
    const QString property = selectors.at(1);

    auto values = m_styleScheme->nameIndex.values(type);
    for (const StyleRule& rule : values)
    {
        if (!FuzzyMatch(rule.selectors, pseudoClass))
        {
            continue;
        }

        for (const Declaration& declartion : rule.declarations)
        {
            if (declartion.d->property != property)
            {
                continue;
            }
            QVariant propertyValue;
            switch (valueType)
            {
            case SCHEME_VALUE_COLOR:
            {
                propertyValue = declartion.colorValue();
#if 0
                QString pseudoClassStr = pseudoClass2String(pseudoClass);
                qInfo() << "fuzzy match selector:" << type << pseudoClassStr << property << declartion.colorValue();
#endif
                break;
            }
            case SCHEME_VALUE_INT:
            {
                int i = 0;
                declartion.intValue(&i);
                propertyValue = i;
                break;
            }
            case SCHEME_VALUE_URL:
                propertyValue = declartion.uriValue();
                break;
            case SCHEME_VALUE_REAL:
            {
                qreal real = 0.0;
                declartion.realValue(&real);
                propertyValue = real;
                break;
            }
            default:
                qWarning() << "this type is not supported at this time";
                continue;
            }
            readLocker.unlock();
            QWriteLocker writeLocker(&m_rwLock);
            insertCacheEntry(propertyName, pseudoClass, propertyValue);
            return propertyValue;
        }
    }

    readLocker.unlock();
    QWriteLocker writeLocker(&m_rwLock);
    insertCacheEntry(propertyName, pseudoClass, QVariant());
    return QVariant();
}

void SchemeLoader::dump()
{
    QMultiHash<QString, StyleRule>::iterator iter;
    QReadLocker readLocker(&m_rwLock);
    for (iter = m_styleScheme->nameIndex.begin();
         iter != m_styleScheme->nameIndex.end();
         iter++)
    {
        qInfo("//=== %s ====/", iter.key().toStdString().c_str());
        StyleRule rule = iter.value();
        for (auto iter : rule.selectors)
        {
            qInfo() << "\tselector:" << pseudoClass2String(iter.pseudoClass()) << iter.pseudoClass();
        }
        for (const Declaration& declaration : rule.declarations)
        {
            qInfo("\tproperty:%s", declaration.d->property.toStdString().c_str());
            for (const Value& value : declaration.d->values)
            {
                qInfo() << "\t\ttype: " << value.type;
                qInfo() << "\t\tvalue:" << value.variant;
            }
        }
    }
}

bool SchemeLoader::load(const QString& schemeFile)
{
    QWriteLocker writeLocker(&m_rwLock);

    //clean cache
    m_isValid = false;
    m_styleSchemeCache.clear();

    QCss::Parser parser(schemeFile, true);
    if (!parser.parse(m_styleScheme))
    {
        qWarning() << "parse style scheme failed!" << parser.errorSymbol().text;
        return false;
    }

    m_isValid = true;
    qDebug() << "load" << schemeFile << "succeed!";
    return true;
}

QString SchemeLoader::pseudoClass2String(quint64 pseudoClass)
{
    QStringList pseudoClassList;
    for (auto iter = pseudoMap.begin();
         iter != pseudoMap.end();
         iter++)
    {
        if (pseudoClass & iter.key())
        {
            pseudoClassList.append(iter.value());
        }
    }
    return pseudoClassList.join(",");
}

bool SchemeLoader::isValid()
{
    return m_isValid;
}

#define PALETTE_COLOR(palette, colorGroup, colorRole, pseudoClass)                                      \
    {                                                                                                   \
        palette->setColor(colorGroup, QPalette::colorRole, getColor(Palette_##colorRole, pseudoclass)); \
    }

void SchemeLoader::polish(QPalette* palette)
{
    // clang-format off
    static const QVector< QPair<QPalette::ColorGroup,quint64> > states = {
        {QPalette::Active,QCss::PseudoClass_Active},
        {QPalette::Disabled,QCss::PseudoClass_Disabled},
//        {QPalette::Inactive,QCss::PseudoClass_Unspecified} //不给inactive单独做状态
        {QPalette::Inactive,QCss::PseudoClass_Active}
    };
    // clang-format on

    for (const auto& pair : states)
    {
        QPalette::ColorGroup group = pair.first;
        quint64 pseudoclass = pair.second;

        PALETTE_COLOR(palette, group, Window, pseudoclass);
        PALETTE_COLOR(palette, group, WindowText, pseudoclass);

        PALETTE_COLOR(palette, group, Base, pseudoclass);
        PALETTE_COLOR(palette, group, Text, pseudoclass);

        //FIXME:暂时交替色使用同样的颜色
        palette->setColor(group,QPalette::AlternateBase,palette->color(group,QPalette::Window));

        PALETTE_COLOR(palette, group, Button, pseudoclass);
        PALETTE_COLOR(palette, group, ButtonText, pseudoclass);

        PALETTE_COLOR(palette, group, Highlight, pseudoclass);
        PALETTE_COLOR(palette, group, HighlightedText, pseudoclass);

        PALETTE_COLOR(palette, group, ToolTipBase, pseudoclass);
        PALETTE_COLOR(palette, group, ToolTipText, pseudoclass);
    }
}

QColor SchemeLoader::getColor(const QWidget* widget, const QStyleOption* opt, SchemeLoader::SchemePropertyName name, quint64 specialPseudoClass)
{
    QVariant var = fetchPropertyValue(widget, opt, name, SCHEME_VALUE_COLOR, specialPseudoClass);
    if (var.isValid())
    {
        return var.value<QColor>();
    }
    return QColor();
}

QColor SchemeLoader::getColor(SchemeLoader::SchemePropertyName propertyName, quint64 pseudoClass)
{
    QVariant var = fetchPropertyValue(propertyName, pseudoClass, SCHEME_VALUE_COLOR);
    if (var.isValid())
    {
        return var.value<QColor>();
    }
    return QColor();
}

QString SchemeLoader::getUrl(const QWidget* widget, const QStyleOption* opt, SchemeLoader::SchemePropertyName name, quint64 specialPseudoClass)
{
    QVariant var = fetchPropertyValue(widget,opt,name,SCHEME_VALUE_URL);
    if(var.isValid())
    {
        return var.value<QString>();
    }
    return QString();
}

QString SchemeLoader::getUrl(SchemeLoader::SchemePropertyName name, quint64 pseudoClass)
{
    QVariant var = fetchPropertyValue(name, pseudoClass, SCHEME_VALUE_URL);
    if (var.isValid())
    {
        return var.value<QString>();
    }
    return QString();
}


///　模糊匹配,优先完全匹配,再着判断待匹配伪状态包含选择器的伪状态
/// \param selectors            选择器
/// \param pseudoClass          待匹配的伪状态
/// \param fuzzyMatchSelector   模糊匹配到的选择器
/// \return 是否匹配
bool FuzzyMatch(const QVector<QCss::Selector>& selectors,
                quint64 pseudoClass)
{
    bool isMatch = false;

    ///完整匹配
    for (const Selector& selector : selectors)
    {
        if (selector.pseudoClass() == pseudoClass)
        {
            isMatch = true;
            break;
        }
    }

    ///包含
    if (!isMatch)
    {
        for (const Selector& selector : selectors)
        {
            if ((selector.pseudoClass() | pseudoClass) == pseudoClass)
            {
                isMatch = true;
                break;
            }
        }
    }

    return isMatch;
}

quint64 SchemeLoader::convertStyleOption2PseudoClass(const QWidget* widget,
                                                     const QStyleOption* opt)
{
    quint64 pesudoClass = 0;

    if (opt->state & QStyle::State_Enabled)
    {
        pesudoClass |= QCss::PseudoClass_Enabled;
    }

    if (opt->state & QStyle::State_Active)
        pesudoClass |= QCss::PseudoClass_Active;

    if (opt->state & QStyle::State_Window)
        pesudoClass |= QCss::PseudoClass_Window;

    if (opt->state & QStyle::State_On)
        pesudoClass |= (QCss::PseudoClass_On | QCss::PseudoClass_Checked);

    if (opt->state & QStyle::State_Off)
        pesudoClass |= (QCss::PseudoClass_Off | QCss::PseudoClass_Unchecked);

    if (opt->state & QStyle::State_NoChange)
        pesudoClass |= QCss::PseudoClass_Indeterminate;

    if (opt->state & QStyle::State_Selected)
        pesudoClass |= QCss::PseudoClass_Selected;

    if (opt->state & QStyle::State_Horizontal)
        pesudoClass |= QCss::PseudoClass_Horizontal;
    else
        pesudoClass |= QCss::PseudoClass_Vertical;

    if (opt->state & (QStyle::State_Open | QStyle::State_On | QStyle::State_Sunken))
    {
        if( widget && ( widget->inherits("QPushButton") || widget->inherits("QToolButton") ) )
        {
            pesudoClass |= QCss::PseudoClass_Pressed;
        }
        else
        {
            pesudoClass |= QCss::PseudoClass_Open;
        }
    }
    else
    {
        pesudoClass |= QCss::PseudoClass_Closed;
    }

    if (opt->state & QStyle::State_Children)
        pesudoClass |= QCss::PseudoClass_Children;

    if (opt->state & QStyle::State_Sibling)
        pesudoClass |= QCss::PseudoClass_Sibling;

    if (opt->state & QStyle::State_ReadOnly)
        pesudoClass |= QCss::PseudoClass_ReadOnly;

    if (opt->state & QStyle::State_Item)
        pesudoClass |= QCss::PseudoClass_Item;

    if (opt->state & QStyle::State_HasFocus)
        pesudoClass |= QCss::PseudoClass_Focus;

    return pesudoClass;
}

//NOTE:此处未上锁，上层调用上锁
void SchemeLoader::insertCacheEntry(SchemeLoader::SchemePropertyName propertyName,
                                    quint64 pseudoStatus,
                                    QVariant var)
{
    QMetaEnum propertyMateEnum = QMetaEnum::fromType<SchemePropertyName>();
    QString selectorString = propertyMateEnum.valueToKey(propertyName);

    QString key = QString("%1:%2").arg(selectorString).arg(pseudoStatus);

    if (m_styleSchemeCache.find(key) == m_styleSchemeCache.end())
    {
        m_styleSchemeCache.insert(key, var);
    }
}

//NOTE:此处未上锁，上层调用需上锁
QVariant SchemeLoader::searchCacheEntry(SchemeLoader::SchemePropertyName propertyName,
                                        quint64 pseudoStatus)
{
    QMetaEnum propertyMateEnum = QMetaEnum::fromType<SchemePropertyName>();
    QString selectorString = propertyMateEnum.valueToKey(propertyName);

    QString key = QString("%1:%2").arg(selectorString).arg(pseudoStatus);

    QVariant var;
    auto iter = m_styleSchemeCache.find(key);
    if (iter != m_styleSchemeCache.end())
    {
        var = iter.value();
    }
    return var;
}
}
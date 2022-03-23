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
#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_LIB_SRC_SCHEME_LOADER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_LIB_SRC_SCHEME_LOADER_H_

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QVariant>
#include <QUrl>
#include <QReadWriteLock>
#include "kiran-palette-define.h"

class QPalette;
class QWidget;
class QStyleOption;

namespace QCss
{
class StyleSheet;
}

namespace Kiran
{
namespace Style
{
enum PseudoClassType
{
    PseudoClass_Unknown = Q_UINT64_C(0x0000000000000000),
    PseudoClass_Enabled = Q_UINT64_C(0x0000000000000001),
    PseudoClass_Disabled = Q_UINT64_C(0x0000000000000002),
    PseudoClass_Pressed = Q_UINT64_C(0x0000000000000004),
    PseudoClass_Focus = Q_UINT64_C(0x0000000000000008),
    PseudoClass_Hover = Q_UINT64_C(0x0000000000000010),
    PseudoClass_Checked = Q_UINT64_C(0x0000000000000020),
    PseudoClass_Unchecked = Q_UINT64_C(0x0000000000000040),
    PseudoClass_Indeterminate = Q_UINT64_C(0x0000000000000080),
    PseudoClass_Unspecified = Q_UINT64_C(0x0000000000000100),
    PseudoClass_Selected = Q_UINT64_C(0x0000000000000200),
    PseudoClass_Horizontal = Q_UINT64_C(0x0000000000000400),
    PseudoClass_Vertical = Q_UINT64_C(0x0000000000000800),
    PseudoClass_Window = Q_UINT64_C(0x0000000000001000),
    PseudoClass_Children = Q_UINT64_C(0x0000000000002000),
    PseudoClass_Sibling = Q_UINT64_C(0x0000000000004000),
    PseudoClass_Default = Q_UINT64_C(0x0000000000008000),
    PseudoClass_First = Q_UINT64_C(0x0000000000010000),
    PseudoClass_Last = Q_UINT64_C(0x0000000000020000),
    PseudoClass_Middle = Q_UINT64_C(0x0000000000040000),
    PseudoClass_OnlyOne = Q_UINT64_C(0x0000000000080000),
    PseudoClass_PreviousSelected = Q_UINT64_C(0x0000000000100000),
    PseudoClass_NextSelected = Q_UINT64_C(0x0000000000200000),
    PseudoClass_Flat = Q_UINT64_C(0x0000000000400000),
    PseudoClass_Left = Q_UINT64_C(0x0000000000800000),
    PseudoClass_Right = Q_UINT64_C(0x0000000001000000),
    PseudoClass_Top = Q_UINT64_C(0x0000000002000000),
    PseudoClass_Bottom = Q_UINT64_C(0x0000000004000000),
    PseudoClass_Exclusive = Q_UINT64_C(0x0000000008000000),
    PseudoClass_NonExclusive = Q_UINT64_C(0x0000000010000000),
    PseudoClass_Frameless = Q_UINT64_C(0x0000000020000000),
    PseudoClass_ReadOnly = Q_UINT64_C(0x0000000040000000),
    PseudoClass_Active = Q_UINT64_C(0x0000000080000000),
    PseudoClass_Closable = Q_UINT64_C(0x0000000100000000),
    PseudoClass_Movable = Q_UINT64_C(0x0000000200000000),
    PseudoClass_Floatable = Q_UINT64_C(0x0000000400000000),
    PseudoClass_Minimized = Q_UINT64_C(0x0000000800000000),
    PseudoClass_Maximized = Q_UINT64_C(0x0000001000000000),
    PseudoClass_On = Q_UINT64_C(0x0000002000000000),
    PseudoClass_Off = Q_UINT64_C(0x0000004000000000),
    PseudoClass_Editable = Q_UINT64_C(0x0000008000000000),
    PseudoClass_Item = Q_UINT64_C(0x0000010000000000),
    PseudoClass_Closed = Q_UINT64_C(0x0000020000000000),
    PseudoClass_Open = Q_UINT64_C(0x0000040000000000),
    PseudoClass_EditFocus = Q_UINT64_C(0x0000080000000000),
    PseudoClass_Alternate = Q_UINT64_C(0x0000100000000000),
    // The Any specifier is never generated, but can be used as a wildcard in searches.
    PseudoClass_Any = Q_UINT64_C(0x0000ffffffffffff)
};
//提供给KiranStyle以及KiranWidget内部获取颜色表定义
class SchemeLoaderPrivate;
class SchemeLoader : public QObject
{
    Q_OBJECT
public:
    //WARNING:枚举值切勿随意更改
    //该枚举与KiranPalette之中的WidgetType,WidgetColorRule，FlagColorRule相关联
    enum SchemePropertyName
    {
        // === Palette === //
        Palette_Window             = 0x00000000,
        Palette_WindowText         = 0x00000001,
        Palette_Base               = 0x00000002,
        Palette_Text               = 0x00000003,
        Palette_Button             = 0x00000004,
        Palette_ButtonText         = 0x00000005,
        Palette_Highlight          = 0x00000006,
        Palette_HighlightedText    = 0x00000007,
        Palette_ToolTipBase        = 0x00000008,
        Palette_ToolTipText        = 0x00000009,

        // === Flag === ///
        Flag_Warning               = 0x00000010,
        Flag_Error                 = 0x00000011,
        Flag_Success               = 0x00000012,

        // === Window === ///
        Window_Background          = 0x00000020,
        Window_Foreground          = 0x00000021,
        Window_Border              = 0x00000022,

        // === Bare === ///
        Bare_Background            = 0x00000030,
        Bare_Foreground            = 0x00000031,

        // === Widget === ///
        Widget_Background          = 0x00000040,
        Widget_Foreground          = 0x00000041,
        Widget_Border              = 0x00000042,

        // === View === ///
        View_Background            = 0x00000050,
        View_Foreground            = 0x00000051,
        View_Border                = 0x00000052,

        // === Selection === ///
        Selection_Background       = 0x00000060,
        Selection_Foreground       = 0x00000061,

        // === TitleBar === ///
        TitleBar_Background        = 0x00000070,
        TitleBar_Foreground        = 0x00000071,
        TitleBar_Border            = 0x00000072
    };
    Q_ENUM(SchemePropertyName)

private:
    enum SchemeValueType
    {
        SCHEME_VALUE_COLOR,
        SCHEME_VALUE_INT,
        SCHEME_VALUE_URL,
        SCHEME_VALUE_REAL
    };

public:
    explicit SchemeLoader(QObject* parent = nullptr);
    ~SchemeLoader() override;

    bool isValid();
    void dump();

    bool load(const QString& schemeFile);
    void polish(QPalette* palette);

    // color
    QColor getColor(const QWidget* widget,
                    const QStyleOption* opt,
                    SchemePropertyName name,
                    quint64 specialPseudoClass = PseudoClass_Unspecified);

    QColor getColor(SchemePropertyName propertyName,
                    quint64 pseudoClass);
    // url
    QString getUrl(const QWidget* widget,
                const QStyleOption* opt,
                SchemePropertyName name,
                quint64 specialPseudoClass = PseudoClass_Unspecified);

    QString getUrl(SchemePropertyName name,
                quint64 pseudoClass);

private:
    QVariant fetchPropertyValue(const QWidget* widget,
                                const QStyleOption* opt,
                                SchemePropertyName name,
                                SchemeValueType valueType,
                                quint64 specialPseudlClasss = PseudoClass_Unspecified);

    QVariant fetchPropertyValue(SchemePropertyName propertyName,
                                quint64 pseudoClass,
                                SchemeValueType valueType);

    /// 将伪类转成字符串
    /// \param qseudoClass　伪类
    /// \return 字符串描述
    QString pseudoClass2String(quint64 pseudoClass);

    /// 转换QStyleOption成伪状态选择器
    /// \param widget       控件,特别控件需要添加伪状态
    /// \param styleOption  样式选项，判断控件伪状态
    /// \return 转换后的伪状态
    quint64 convertStyleOption2PseudoClass(const QWidget* widget, const QStyleOption* opt);

    /// 存入缓存
    /// \param selector       选择器
    /// \param pseudoStatus   伪状态
    /// \param var            属性值
    void insertCacheEntry(SchemePropertyName propertyName,
                          quint64 pseudoStatus,
                          QVariant var);

    QVariant searchCacheEntry(SchemePropertyName propertyName,
                              quint64 pseudoStatus);

private:
    bool m_isValid = false;
    QCss::StyleSheet* m_styleScheme;
    QReadWriteLock m_rwLock;
    QMap<QString, QVariant> m_styleSchemeCache;
};
}  // namespace Style
}  // namespace Kiran
#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_LIB_SRC_SCHEME_LOADER_H_

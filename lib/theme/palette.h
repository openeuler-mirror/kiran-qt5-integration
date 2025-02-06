/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * kiranwidgets-qt5 is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     tangjie02 <tangjie02@kylinsec.com.cn>
 */

#pragma once

#include <QColor>
#include <QStyle>

class QStyleOption;
class QWidget;
class QPalette;

#define DEFAULT_PALETTE Palette::getDefault

namespace Kiran
{
namespace Theme
{
class PalettePrivate;

// 与QPalette类似，不同之处在于Palette是根据kiran的颜色规范来设计且调色更加细化
class Palette : public QObject
{
    Q_OBJECT

public:
    struct BaseColors
    {
        // 用于窗口背景、输入框背景和图标，即底色
        QColor baseBackground;
        // 用于文字和图标
        QColor baseForeground;
        // 用于大部分控件的默认背景色
        QColor widgetBackground;
        // 用于大部分控件的边框色、Switch关、滑块背景
        QColor widgetBorder;
        // 用于控件选中背景、Switch开、滑块值、check/radio按钮选中
        QColor widgetSelection;
        // 用于主要按钮
        QColor widgetMain;
        // 用于危险提示文字、图标和输入框错误描边
        QColor widgetWarning;
        // 用于大部分容器的默认背景色
        QColor containerBackground;
    };

    struct ColorFactor
    {
        // 一般控件悬浮
        double widgetHover;
        // 一般控件按下
        double widgetSunken;
        // 图标悬浮
        double iconHover;
        // 图标按下
        double iconSunken;
        // 被禁用后的透明度
        double disabledAlpha;
        // 被禁用后的亮度调节
        double disabledLightness;
    };

    // 类似QPalette::ColorGroup
    enum ColorGroup
    {
        // 激活状态
        ACTIVE,
        // 禁用状态
        DISABLED,
        // 未激活状态（即控件所在窗口未激活）
        INACTIVE,
        // 鼠标悬浮状态
        MOUSE_OVER,
        // 按下状态
        SUNKEN,
        // 选中、开启状态
        SELECTED,
        // 总数
        N_COLOR_GROUPS,
        NORMAL = ACTIVE,
        BACKDROP = INACTIVE,
    };
    Q_ENUM(ColorGroup)

    enum ColorRole
    {
        // 窗口背景色
        WINDOW,
        // 文本颜色
        TEXT,
        // 一般（非窗口）控件背景色，例如按钮
        WIDGET,
        // 图标
        ICON,
        // 滑动条
        SCROLL,
        // 边框
        BORDER,
        // 高亮背景色，如选中项颜色
        HIGHLIGHT,
        // 高亮文本颜色
        HIGHLIGHTED_TEXT,
        // 总数
        N_COLOR_ROLES,
    };
    Q_ENUM(ColorRole)

public:
    explicit Palette(QObject* parent = nullptr);
    ~Palette() override = default;

    static Palette* getDefault();

    // 设置基础颜色组，主题颜色都基于基础颜色组衍生
    void setBaseColors(const BaseColors& baseColors);
    // 获取基础颜色组中的颜色
    const BaseColors& getBaseColors();

    // 根据伪状态的优先级来确定实际颜色
    QColor getColor(QStyle::State styleState, Palette::ColorRole colorRole);
    QColor getColor(Palette::ColorGroup colorGroup, Palette::ColorRole colorRole);

    void polish(QPalette& palette);

signals:
    // 当基础颜色发生变化时发送该信号
    void baseColorsChanged();

private:
    uint32_t styleState2ColorGroups(QStyle::State styleState);

private:
    static Palette* m_instance;

    PalettePrivate* d_ptr;
    Q_DECLARE_PRIVATE(Palette)
};

extern Palette::BaseColors g_lightBaseColors;
extern Palette::BaseColors g_darkBaseColors;

}  // namespace Theme

}  // namespace Kiran

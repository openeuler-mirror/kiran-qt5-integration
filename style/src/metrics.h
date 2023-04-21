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
#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_METRICS_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_METRICS_H_

namespace Kiran
{
//* metrics
struct Metrics
{
    // frames
    static constexpr int Frame_FrameWidth = 4;
    static constexpr int Frame_FrameRadius = 4;

    // layout
    static constexpr int Layout_TopLevelMarginWidth = 10;
    static constexpr int Layout_ChildMarginWidth = 6;
    static constexpr int Layout_DefaultSpacing = 6;

    // line editors
    static constexpr int LineEdit_FrameWidth = 7;

    // menu items
    static constexpr int Menu_FrameWidth = 0;
    static constexpr int MenuItem_MarginWidth = 5;
    static constexpr int MenuItem_ItemSpacing = 4;
    static constexpr int MenuItem_AcceleratorSpace = 16;
    static constexpr int MenuButton_IndicatorWidth = 24;

    // combobox
    static constexpr int ComboBox_FrameWidth = 4;
    static constexpr int ComboBox_MarginHeight = 2;
    static constexpr int ComboBox_MarginWidth = 2;
    static constexpr int ComboBox_MinHeight = 34;
    static constexpr int ComboBox_MinWidth = 80;

    // spinbox
    static constexpr int SpinBox_FrameWidth = LineEdit_FrameWidth;
    static constexpr int SpinBox_ArrowButtonWidth = 20;

    // groupbox title margin
    static constexpr int GroupBox_TitleMarginWidth = 4;

    // buttons
    static constexpr int Button_MinHeight = 34;
    static constexpr int Button_MinWidth = 60;
    static constexpr int Button_MarginHeight = 2;
    static constexpr int Button_MarginWidth = 4;
    static constexpr int Button_ItemSpacing = 4;

    // tool buttons
    static constexpr int ToolButton_MarginWidth = 6;
    static constexpr int ToolButton_ItemSpacing = 4;
    static constexpr int ToolButton_InlineIndicatorWidth = 12;

    // checkboxes and radio buttons
    static constexpr int CheckBox_Size = 15;
    static constexpr int CheckBox_FocusMarginWidth = 2;
    static constexpr int CheckBox_ItemSpacing = 4;
    static constexpr int CheckBox_Radius = Frame_FrameRadius - 1;

    // menubar items
    static constexpr int MenuBarItem_MarginWidth = 10;
    static constexpr int MenuBarItem_MarginHeight = 6;

    // scrollbars
    static constexpr int ScrollBar_Extend = 12;
    static constexpr int ScrollBar_SliderWidth = 8;
    static constexpr int ScrollBar_MinSliderHeight = 20;
    static constexpr int ScrollBar_NoButtonHeight = (ScrollBar_Extend - ScrollBar_SliderWidth) / 2;
    static constexpr int ScrollBar_SingleButtonHeight = ScrollBar_Extend;
    static constexpr int ScrollBar_DoubleButtonHeight = 2 * ScrollBar_Extend;

    // toolbars
    static constexpr int ToolBar_FrameWidth = 6;
    static constexpr int ToolBar_HandleExtent = 10;
    static constexpr int ToolBar_HandleWidth = 6;
    static constexpr int ToolBar_SeparatorWidth = 8;
    static constexpr int ToolBar_ExtensionWidth = 20;
    static constexpr int ToolBar_ItemSpacing = 0;

    // progressbars
    static constexpr int ProgressBar_BusyIndicatorSize = 14;
    static constexpr int ProgressBar_Thickness = 8;
    static constexpr int ProgressBar_ItemSpacing = 4;

    // mdi title bar
    static constexpr int TitleBar_MarginWidth = 4;

    // sliders
    static constexpr int Slider_TickLength = 8;
    static constexpr int Slider_TickMarginWidth = 2;
    static constexpr int Slider_GrooveThickness = 6;
    static constexpr int Slider_ControlThickness = 16;

    // tabbar
    static constexpr int TabBar_TabMarginHeight = 4;
    static constexpr int TabBar_TabMarginWidth = 8;
    static constexpr int TabBar_TabMinWidth = 80;
    static constexpr int TabBar_TabMinHeight = 30;
    static constexpr int TabBar_TabItemSpacing = 8;
    static constexpr int TabBar_TabOverlap = 1;
    static constexpr int TabBar_BaseOverlap = 2;

    // tab widget
    static constexpr int TabWidget_MarginWidth = 4;

    // toolbox
    static constexpr int ToolBox_TabMinWidth = 80;
    static constexpr int ToolBox_TabItemSpacing = 4;
    static constexpr int ToolBox_TabMarginWidth = 8;

    // tooltips
    static constexpr int ToolTip_FrameWidth = 3;

    // list headers
    static constexpr int Header_MarginWidth = 6;
    static constexpr int Header_ItemSpacing = 4;
    static constexpr int Header_ArrowSize = 10;

    // tree view
    static constexpr int ItemView_ArrowSize = 10;
    static constexpr int ItemView_ItemMarginWidth = 3;
    static constexpr int SidePanel_ItemMarginWidth = 4;

    // splitter
    static constexpr int Splitter_SplitterWidth = 1;

    // shadow dimensions
    static constexpr int Shadow_Overlap = 2;
};
}  // namespace Kiran

#endif  // KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_METRICS_H_

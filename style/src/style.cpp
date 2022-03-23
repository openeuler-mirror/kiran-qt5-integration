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
#include "style.h"
#include "../../style-helper/src/scheme-loader.h"
#include "metrics.h"
#include "render-helper.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QDebug>
#include <QFormLayout>
#include <QPainter>

#include <kiran-palette.h>
#include <QCheckBox>
#include <QComboBox>
#include <QDial>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSplitterHandle>
#include <QStyleOption>
#include <QSvgGenerator>
#include <QTextEdit>
#include <QToolButton>

//TODO: 后期考虑再度整合到几个大类的源文件之中
#include "draw-helper/draw-button-helper.h"
#include "draw-helper/draw-combo-box-helper.h"
#include "draw-helper/draw-edit-helper.h"
#include "draw-helper/draw-group-box-helper.h"
#include "draw-helper/draw-indicator-helper.h"
#include "draw-helper/draw-item-view-helper.h"
#include "draw-helper/draw-progress-bar-helper.h"
#include "draw-helper/draw-scroll-bar-helper.h"
#include "draw-helper/draw-slider-helper.h"
#include "draw-helper/draw-spin-box-helper.h"
#include "draw-helper/draw-tab-widget-helper.h"
#include "draw-helper/draw-menu-helper.h"

using namespace Kiran::Style;

#if 0
//调试用
QDebug operator<<(QDebug dbg, const QColor &color)
{
    QString format = QString("[rgb(%1,%2,%3) #%4%5%6]")
                         .arg(color.red()).arg(color.green()).arg(color.blue())
                         .arg(QString::number(color.red(),16)).arg(QString::number(color.green(),16)).arg(QString::number(color.blue(),16));
    dbg << format;
    return dbg;
}
#endif

Style::Style()
    : ParentStyle()
{
}

Style::~Style()
{
}

int Style::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    switch (hint)
    {
    //下栏框的下栏列表鼠标追踪
    case SH_ComboBox_ListMouseTracking:
        return true;
    //菜单栏鼠标追踪
    case SH_MenuBar_MouseTracking:
        return true;
    //菜单鼠标追踪
    case SH_Menu_MouseTracking:
        return true;
    //菜单之中子菜单弹出延时时间
    case SH_Menu_SubMenuPopupDelay:
        return 150;
    //弹出菜单是否支持用户在跨越菜单的其他项时将鼠标光标移动到子菜单
    case SH_Menu_SloppySubMenus:
        return true;
    //该值目前亦废弃，改用SH_Widget_Animation_Duration
    case SH_Widget_Animate:
        return true;
    //动画持续时间ms,0代表禁用
    case SH_Widget_Animation_Duration:
        return 100;
    //确定样式是在菜单中显示节，还是将其视为普通分隔符。节是带有文本和图标提示的分隔符
    case SH_Menu_SupportsSections:
        return true;
    //指示QDialogButtonBox中的标准按钮是否应具有图标
    case SH_DialogButtonBox_ButtonsHaveIcons:
        return false;
    //GroupBox文本标签垂直对齐选项
    case SH_GroupBox_TextLabelVerticalAlignment:
        return Qt::AlignVCenter;
    //TabBar的对齐方式
    case SH_TabBar_Alignment:
        return Qt::AlignLeft | Qt::AlignVCenter;
    //ToolBox中所选页面标题是否显示粗体
    case SH_ToolBox_SelectedPageTitleBold:
        return false;
    //滚动条上单击鼠标中键滑块是否跳转到该绝对位置
    case SH_ScrollBar_MiddleClickAbsolutePosition:
        return true;
    //ScrollView是仅围绕内容（如Motif）还是围绕内容、滚动条和角部件（如窗口）绘制框架。
    case SH_ScrollView_FrameOnlyAroundContents:
        return false;
    //FormLayout对齐其内容的默认方式
    case SH_FormLayoutFormAlignment:
        return Qt::AlignLeft | Qt::AlignVCenter;
    //FormatLayout对齐标签的默认方式
    case SH_FormLayoutLabelAlignment:
        return Qt::AlignRight;
    //QFormLayout中字段增长方式的默认值。返回QFormLayout:：FieldGrowthPolicy枚举
    case SH_FormLayoutFieldGrowthPolicy:
        return QFormLayout::ExpandingFieldsGrow;
    //QFormLayout中如何换行的默认方式。返回一个QFormLayout::RowWrapPolicy enum。
    case SH_FormLayoutWrapPolicy:
        return QFormLayout::DontWrapRows;
    //消息框中的文本是否允许用户交互(如选择)
    case SH_MessageBox_TextInteractionFlags:
        return Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse;
    //进度对话框按钮取消按钮是否居中对齐，否则右对齐
    case SH_ProgressDialog_CenterCancelButton:
        return false;
    //消息框是否按钮居中
    case SH_MessageBox_CenterButtons:
        return false;
    //输入控件请求输入面板的时间点,返回类型为QStyle::RequestSoftwareInputPanel。
    case SH_RequestSoftwareInputPanel:
        return RSIP_OnMouseClick;
    //标题栏无边框
    case SH_TitleBar_NoBorder:
        return true;
    //DockWidget的按钮是否有Frame
    case SH_DockWidget_ButtonsHaveFrame:
        return false;
    //ToolTip透明文本标签的透明度
    case SH_ToolTipLabel_Opacity:
        return 204;
    //Table里网格线的颜色
    case SH_Table_GridLineColor:
        return KiranPalette::instance()->color(KiranPalette::Normal,KiranPalette::Widget,KiranPalette::Border).rgb();
    default:
        return ParentStyle::styleHint(hint, option, widget, returnData);
    }
}

int Style::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric)
    {
    case PM_DefaultFrameWidth:
        return 1;
    case PM_SpinBoxFrameWidth:
        return 1;
    case PM_ToolBarFrameWidth:
        return 6;
    case PM_ToolTipLabelFrameWidth:
        return 3;

    case PM_FocusFrameVMargin:
    case PM_FocusFrameHMargin:
        return 2;

    //布局默认边距
    case PM_LayoutLeftMargin:
    case PM_LayoutTopMargin:
    case PM_LayoutRightMargin:
    case PM_LayoutBottomMargin:
    {
        //使用子控件边距或顶层窗口边距，取决于部件类型
        if ((option && (option->state & QStyle::State_Window)) || (widget && widget->isWindow()))
        {
            return 10;
        }
        else
        {
            return 6;
        }
    }

    //布局默认间距
    case PM_LayoutHorizontalSpacing:
    case PM_LayoutVerticalSpacing:
        return 6;

    //按钮边距
    case PM_ButtonMargin:
        return 12;

    //按钮默认指示器边框宽度
    case PM_ButtonDefaultIndicator:
        return 0;
    //当按钮按下时，按钮的内容水平移动偏移量。
    case PM_ButtonShiftHorizontal:
        return 0;
    //当按钮按下时，按钮的内容垂直移动偏移量。
    case PM_ButtonShiftVertical:
        return 0;

    //菜单栏默认边框宽度
    case PM_MenuBarPanelWidth:
        return 0;
    //菜单栏水平边距
    case PM_MenuBarHMargin:
        return 0;
    //菜单栏垂直边距
    case PM_MenuBarVMargin:
        return 3;
    //菜单项间距
    case PM_MenuBarItemSpacing:
        return 10;
    //桌面菜单边框宽度
    case PM_MenuDesktopFrameWidth:
        return 0;

    //菜单按钮指示器宽度
    case PM_MenuButtonIndicator:
        return 24;

    // toolbars
    case PM_ToolBarHandleExtent:
        return 10;
    case PM_ToolBarSeparatorExtent:
        return 8;
    case PM_ToolBarExtensionExtent:
        return pixelMetric(PM_SmallIconSize, option, widget) + 2 * 6;

    case PM_ToolBarItemMargin:
        return 0;
    case PM_ToolBarItemSpacing:
        return 0;

    // tabbars
    case PM_TabBarTabShiftVertical:
        return 0;
    case PM_TabBarTabShiftHorizontal:
        return 0;
    case PM_TabBarTabOverlap:
        return 1;
    case PM_TabBarBaseOverlap:
        return 2;
    case PM_TabBarTabHSpace:
        return 2 * 8;
    case PM_TabBarTabVSpace:
        return 2 * 4;
    case PM_TabCloseIndicatorWidth:
    case PM_TabCloseIndicatorHeight:
        return pixelMetric(PM_SmallIconSize, option, widget);

    // scrollbars
    case PM_ScrollBarExtent:
        return 12;
    case PM_ScrollBarSliderMin:
        return 20;

    // title bar
    case PM_TitleBarHeight:
        return 2 * 4 + pixelMetric(PM_SmallIconSize, option, widget);

    // sliders
    case PM_SliderThickness:
        return 16;
    case PM_SliderControlThickness:
        return 16;
    case PM_SliderLength:
        return 16;

    // checkboxes and radio buttons
    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
        return 15;

    // list headers
    case PM_HeaderMarkSize:
        return 10;
    case PM_HeaderMargin:
        return 6;

    // dock widget
    // return 0 here, since frame is handled directly in polish
    case PM_DockWidgetFrameWidth:
        return 0;
    case PM_DockWidgetTitleMargin:
        return 4;
    case PM_DockWidgetTitleBarButtonMargin:
        return 6;

    case PM_SplitterWidth:
        return 1;
    case PM_DockWidgetSeparatorExtent:
        return 1;

    default:  //fallback
        break;
    }
    return ParentStyle::pixelMetric(metric, option, widget);
}

void Style::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    bool (*func)(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget);
    func = nullptr;

    // clang-format off
    switch (element)
    {
    case PE_Frame:                   func = &drawPEFrame; break;
    case PE_FrameFocusRect:          func = &drawPEFrameFocusRect; break;
    case PE_FrameGroupBox:           func = &drawPEFrameGroupBox; break;

    case PE_PanelButtonCommand:      func = &drawPEPanelButtonCommand; break;
    case PE_PanelButtonTool:         func = &drawPEPanelButtonTool; break;

    case PE_FrameLineEdit:           func = &drawPEFrameLineEdit; break;
    case PE_PanelLineEdit:
    {
        if( widget && widget->parentWidget() )
        {
            /*作为QComoBox和QAbstractSpinBox的子控件时不进行绘制*/
            if( qobject_cast<const QComboBox*>(widget->parentWidget()) ||
                qobject_cast<const QAbstractSpinBox*>(widget->parentWidget()) )
            {
                return;
            }
        }
        break;
    }
    case PE_IndicatorButtonDropDown:    func = &drawPEIndicatorButtonDropDown; break;
    case PE_IndicatorArrowUp:           func = &drawPEIndicatorArrowUp; break;
    case PE_IndicatorArrowDown:         func = &drawPEIndicatorArrowDown; break;
    case PE_IndicatorArrowLeft:         func = &drawPEIndicatorArrowLeft; break;
    case PE_IndicatorArrowRight:        func = &drawPEIndicatorArrowRight; break;
    case PE_IndicatorRadioButton:       func = &drawPEIndicatorRadioButton; break;
    case PE_IndicatorCheckBox:          func = &drawPEIndicatorCheckBox; break;
    case PE_IndicatorBranch:            func = &drawPEIndicatorBranch; break;
    case PE_FrameTabWidget:             func = &drawPEFrameTabWidget; break;
    case PE_FrameTabBarBase:            func = &drawPEFrameTabBarBase; break;

    case PE_IndicatorToolBarSeparator:  func = &drawPEIndicatorToolBarSeparator; break;
    case PE_IndicatorToolBarHandle:     func = &drawPEIndicatorToolBarHandle; break;
    default: break;
    }
    // clang-format on

    PainterSaver painterSaver(painter);
    if (!(func && (*func)(this, option, painter, widget)))
    {
        ParentStyle::drawPrimitive(element, option, painter, widget);
    }
}

void Style::drawComplexControl(QStyle::ComplexControl control,
                               const QStyleOptionComplex *option,
                               QPainter *painter,
                               const QWidget *widget) const
{
    bool (*func)(const QStyle *style, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget);
    func = nullptr;

    // clang-format off
    switch (control)
    {
    case QStyle::CC_ToolButton: func = &drawCCToolButton; break;
    case QStyle::CC_ComboBox: func = &drawCCComboBox; break;
    case QStyle::CC_ScrollBar: func = &drawCCScrollBar; break;
    case QStyle::CC_SpinBox: func = &drawCCSpinBox; break;
    case QStyle::CC_GroupBox: func = &drawCCGroupBox; break;
    case QStyle::CC_Slider: func = &drawCCSlider; break;
    default: break;
    }
    // clang-format on

    painter->save();
    if (!(func && (*func)(this, option, painter, widget)))
    {
        ParentStyle::drawComplexControl(control, option, painter, widget);
    }
    painter->restore();
}

QRect Style::subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    QRect rect;

    switch (element)
    {
    case QStyle::SE_TabBarTearIndicator:
    case QStyle::SE_TabBarTabLeftButton:
    case QStyle::SE_TabBarTabRightButton:
    case QStyle::SE_TabBarTabText:
    case QStyle::SE_TabBarScrollLeftButton:
    case QStyle::SE_TabBarScrollRightButton:
    case QStyle::SE_TabBarTearIndicatorRight:
        rect = tabBarSubElementRect(this, element, option, widget);
        return rect;

    case SE_ProgressBarGroove:
    case SE_ProgressBarContents:
    case SE_ProgressBarLabel:
        rect = progressBarElementRect(this, element, option, widget);
        return rect;

    case QStyle::SE_SliderFocusRect:
        rect = sliderElementRect(this, element, option, widget);
        return rect;

    default:
        break;
    }

    return ParentStyle::subElementRect(element, option, widget);
}

QRect Style::subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *widget) const
{
    typedef bool (*SubControlRectFunc)(const QStyle *, const QStyleOptionComplex *, QStyle::SubControl, const QWidget *, QRect &);
    static const QMap<QStyle::ComplexControl, SubControlRectFunc> SubControlRectMap = {
        {QStyle::CC_ToolButton, &toolButtonSubControlRect},
        {QStyle::CC_ComboBox, &comboBoxSubControlRect},
        {QStyle::CC_ScrollBar, &scrollBarSubControlRect},
        {QStyle::CC_SpinBox, &spinBoxSubControlRect},
        {QStyle::CC_GroupBox, &groupBoxSubControlRect},
        {QStyle::CC_Slider, &sliderSubControlRect}};

    QRect controlRect;
    if ((SubControlRectMap.find(cc) != SubControlRectMap.end()) &&
        (*SubControlRectMap.find(cc))(this, opt, sc, widget, controlRect))
    {
        return controlRect;
    }
    else
    {
        return ParentStyle::subControlRect(cc, opt, sc, widget);
    }
}

QSize Style::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{

    switch (type)
    {
    case CT_PushButton:
        return pushButtonSizeFromContents(this, option, contentSize, widget);
    case CT_ToolButton:
        return toolButtonSizeFromContents(this, option, contentSize, widget);
    case CT_TabBarTab:
        return tabBarTabSizeFromContents(this, option, contentSize, widget);
    case CT_ComboBox:
        return comboBoxSizeFromContents(this, option, contentSize, widget);
    case CT_SpinBox:
        return spinBoxSizeFromContents(this, option, contentSize, widget);
    case CT_GroupBox:
        return groupBoxSizeFromContents(this, option, contentSize, widget);
    case CT_ProgressBar:
        return progressBarSizeFromContents(this, option, contentSize, widget);
    case CT_Slider:
        return sliderSizeFromContents(this, option, contentSize, widget);
    case CT_CheckBox:
        break;
    case CT_RadioButton:
        break;
    case CT_Splitter:
        break;
    case CT_MenuItem:
        break;
    case CT_MenuBarItem:
        break;
    case CT_MenuBar:
        break;
    case CT_Menu:
        break;
    case CT_LineEdit:
    {
        int buttuonMargin = pixelMetric(QStyle::PM_ButtonMargin,option,widget);
        QSize size = contentSize + QSize(buttuonMargin,buttuonMargin);
        return size;
    }
    case CT_SizeGrip:
        break;
    case CT_TabWidget:
        break;
    case CT_DialogButtons:
        break;
    case CT_HeaderSection:
        break;
    case CT_MdiControls:
        break;
    case CT_ItemViewItem:
        break;
    default:
        break;
    }
    return ParentStyle::sizeFromContents(type, option, contentSize, widget);
}

void Style::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    bool (*func)(const QStyle *style, const QStyleOption *option, QPainter *painter, const QWidget *widget);
    func = nullptr;

    //不绘制的控件元素集合
    static QSet<QStyle::ControlElement> emptyControlSet = {CE_ToolBar};

    // clang-format off
    switch (element)
    {
    case QStyle::CE_ShapedFrame:      func = &drawControlShapedFrame; break;

    case QStyle::CE_TabBarTabLabel:   func = &drawControlTabBarTabLabel; break;
    case QStyle::CE_TabBarTabShape:   func = &drawControlTabBarTabShape; break;

    case QStyle::CE_ToolBoxTabLabel:  func = &drawControlToolButtonLabel; break;
    case QStyle::CE_ComboBoxLabel:    func = &drawControlComboBoxLabel; break;

    case QStyle::CE_HeaderLabel:      func = &drawControlHeaderLabel; break;
    case QStyle::CE_HeaderSection:    func = &drawControlHeaderSection; break;
    case QStyle::CE_HeaderEmptyArea:  func = &drawControlHeaderEmptyArea; break;

    case QStyle::CE_ScrollBarSlider:  func = &drawControlScrollBarSlider; break;

    case QStyle::CE_ProgressBar:      func = &drawControlProgressBar; break;
    case CE_ProgressBarGroove:        func = &drawControlProgressBarGroove; break;
    case CE_ProgressBarContents:      func = &drawControlProgressBarContents; break;
    case CE_ProgressBarLabel:         func = &drawControlProgressBarLabel; break;

    case QStyle::CE_MenuBarItem:      func = &drawControlMenuBarItem; break;
    case QStyle::CE_MenuBarEmptyArea: func = &drawControlMenuBarEmptyArea; break;
    default: break;
    }
    // clang-format on

    painter->save();
    if (!(func && (*func)(this, option, painter, widget)))
    {
        if( emptyControlSet.find(element) == emptyControlSet.end() )
        {
            ParentStyle::drawControl(element, option, painter, widget);
        }
    }
    painter->restore();
}

void Style::polish(QWidget *widget)
{
    if (!widget)
        return;

    // enable mouse over effects for all necessary widgets
    if (qobject_cast<QAbstractItemView *>(widget) ||
        qobject_cast<QAbstractSpinBox *>(widget) ||
        qobject_cast<QCheckBox *>(widget) ||
        qobject_cast<QComboBox *>(widget) ||
        qobject_cast<QDial *>(widget) ||
        qobject_cast<QLineEdit *>(widget) ||
        qobject_cast<QPushButton *>(widget) ||
        qobject_cast<QRadioButton *>(widget) ||
        qobject_cast<QScrollBar *>(widget) ||
        qobject_cast<QSlider *>(widget) ||
        qobject_cast<QSplitterHandle *>(widget) ||
        qobject_cast<QTabBar *>(widget) ||
        qobject_cast<QTextEdit *>(widget) ||
        qobject_cast<QToolButton *>(widget))
    {
        widget->setAttribute(Qt::WA_Hover);
    }

    if( qobject_cast<QAbstractScrollArea*>(widget) )
    {
        auto scrollArea = qobject_cast<QAbstractScrollArea*>(widget);
        if( scrollArea->frameShadow() == QFrame::Sunken && scrollArea->focusPolicy()&Qt::StrongFocus )
        {
            scrollArea->setAttribute( Qt::WA_Hover );
        }
//        scrollArea->viewport()->setAutoFillBackground(false);
//        qInfo() << "viewport:" << scrollArea->viewport()->geometry();
//        qInfo() << "scroll area:" << scrollArea->geometry();
    }

    ParentStyle::polish(widget);
}

void Style::polish(QApplication *app)
{
    ParentStyle::polish(app);

    QPalette palette;
    KiranPalette::instance()->polishPalette(&palette);
    QApplication::setPalette(palette);
}

void Style::polish(QPalette &palette)
{
    ParentStyle::polish(palette);
    KiranPalette::instance()->polishPalette(&palette);
}
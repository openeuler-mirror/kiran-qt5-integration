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

#include "lib/theme/style.h"
#include <declarations.h>
#include <QAbstractItemView>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDial>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSplitterHandle>
#include <QStyleFactory>
#include <QStyleOption>
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <QTextEdit>
#include <QToolButton>
#include "lib/theme/metrics.h"
#include "lib/theme/palette.h"
#include "lib/theme/style-private.h"
#include "render-helper.h"

#if 0
// 调试用
QDebug operator<<(QDebug dbg, const QColor &color)
{
    QString format = QString("[rgb(%1,%2,%3) #%4%5%6]")
                         .arg(color.red())
                         .arg(color.green())
                         .arg(color.blue())
                         .arg(QString::number(color.red(), 16))
                         .arg(QString::number(color.green(), 16))
                         .arg(QString::number(color.blue(), 16));
    dbg << format;
    return dbg;
}
#endif

namespace Kiran
{
namespace Theme
{
static const int groupBoxTopMargin = 3;

StylePrivate::StylePrivate(Style *ptr) : q_ptr(ptr)
{
}

void StylePrivate::tabLayout(const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect, QRect *iconRect) const
{
    auto q = q_func();

    QRect tr = opt->rect;
    bool verticalTabs = RenderHelper::isVerticalTab(opt->shape);

    if (verticalTabs)
        tr.setRect(0, 0, tr.height(), tr.width());  // 0, 0 as we will have a translate transform

    /// 垂直和水平偏移量,以突出当前选中的Tab标签
    int verticalShift = q->pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt, widget);
    int horizontalShift = q->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt, widget);

    int hpadding = q->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2;
    int vpadding = q->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;

    if (opt->shape == QTabBar::RoundedSouth || opt->shape == QTabBar::TriangularSouth)
        verticalShift = -verticalShift;

    tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);
    bool selected = opt->state & QStyle::State_Selected;
    if (selected)
    {
        tr.setTop(tr.top() - verticalShift);
        tr.setRight(tr.right() - horizontalShift);
    }

    // left widget
    if (!opt->leftButtonSize.isEmpty())
    {
        tr.setLeft(tr.left() + 4 +
                   (verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
    }
    // right widget
    if (!opt->rightButtonSize.isEmpty())
    {
        tr.setRight(tr.right() - 4 -
                    (verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));
    }

    // icon
    if (!opt->icon.isNull())
    {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid())
        {
            int iconExtent = q->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize = opt->icon.actualSize(iconSize,
                                                 (opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                                                 (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);
        // High-dpi icons do not need adjustment; make sure tabIconSize is not larger than iconSize
        tabIconSize = QSize(qMin(tabIconSize.width(), iconSize.width()), qMin(tabIconSize.height(), iconSize.height()));

        *iconRect = QRect(tr.left(), tr.center().y() - tabIconSize.height() / 2,
                          tabIconSize.width(), tabIconSize.height());
        if (!verticalTabs)
            *iconRect = q->visualRect(opt->direction, opt->rect, *iconRect);
        tr.setLeft(tr.left() + tabIconSize.width() + 4);
    }

    if (!verticalTabs)
        tr = q->visualRect(opt->direction, opt->rect, tr);

    *textRect = tr;
}

void StylePrivate::renderSpinBoxArrow(const QStyle::SubControl &subControl, const QStyleOptionSpinBox *option, QPainter *painter, const QWidget *widget) const
{
    auto q = q_func();

    const QStyle::State &state(option->state);

    QRectF arrowRect(q->subControlRect(QStyle::CC_SpinBox, option, subControl, widget));

    bool enabled(state & QStyle::State_Enabled);
    const bool atLimit = (subControl == QStyle::SC_SpinBoxUp && !(option->stepEnabled & QAbstractSpinBox::StepUpEnabled)) ||
                         (subControl == QStyle::SC_SpinBoxDown && !(option->stepEnabled & QAbstractSpinBox::StepDownEnabled));
    enabled &= !atLimit;

    const bool mouseOver(enabled && (state & QStyle::State_MouseOver));
    bool sunken(state & QStyle::State_Sunken && option->activeSubControls & subControl);
    bool hasFocus(state & QStyle::State_HasFocus);

    const bool subControlHover(enabled && mouseOver && (option->activeSubControls & subControl));
    const bool subControlSunken(enabled && (sunken) && (option->activeSubControls & subControl));

    const QMap<QStyle::State, bool> subControlFlagMap = {
        {QStyle::State_MouseOver, subControlHover},
        {QStyle::State_Sunken, subControlSunken},
        {QStyle::State_Enabled, enabled}};

    // 复制一个子控件StyleOption,更新其中的状态
    QStyleOption tempOption(*option);
    for (auto iter = subControlFlagMap.begin(); iter != subControlFlagMap.end(); iter++)
    {
        if (iter.value())
        {
            tempOption.state |= iter.key();
        }
        else
        {
            tempOption.state &= ~iter.key();
        }
    }

    auto backgroundColor = Palette::getDefault()->getColor(tempOption.state, Palette::WIDGET);
    auto borderColor = Palette::getDefault()->getColor(tempOption.state, Palette::BORDER);
    auto signColor = Palette::getDefault()->getColor(tempOption.state, Palette::ICON);

    // 绘制按钮与输入区域之间的分割线
    if (subControl == QStyle::SC_SpinBoxDown)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(borderColor);
        int highlight = hasFocus ? 1 : 0;
        painter->drawLine(arrowRect.left(), arrowRect.top() + 2 + highlight, arrowRect.left(), arrowRect.bottom() - 1 - highlight);
    }

    if (subControl == QStyle::SC_SpinBoxUp)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(borderColor);
        int highlight = hasFocus ? 1 : 0;
        painter->drawLine(arrowRect.left(), arrowRect.top() + 2 + highlight, arrowRect.left(), arrowRect.bottom() - 1 - highlight);
    }

    // 绘制按钮背景色
    painter->setPen(Qt::NoPen);
    QColor background = backgroundColor;
    painter->setBrush(background);
    painter->drawRect(arrowRect.adjusted(0.5, 0.5, -0.5, -0.5));

    // 绘制按钮之上的符号 “+”，“-”
    QPen pen(signColor, 2);
    pen.setCapStyle(Qt::FlatCap);
    QRect r = arrowRect.adjusted(1, 2, 0, 0).toRect();
    painter->setPen(pen);
    painter->drawLine(r.center() - QPointF(5, 0), r.center() + QPointF(5, 0));
    if (subControl == QStyle::SC_SpinBoxUp)
        painter->drawLine(r.center() - QPointF(0, 5), r.center() + QPointF(0, 5));
}

//* returns height for scrollbar buttons depending of button types
int StylePrivate::scrollBarButtonHeight(ScrollBarButtonType type) const
{
    switch (type)
    {
    case NoButton:
        return Metrics::ScrollBar_NoButtonHeight;
    case SingleButton:
        return Metrics::ScrollBar_SingleButtonHeight;
    case DoubleButton:
        return Metrics::ScrollBar_DoubleButtonHeight;
    default:
        return 0;
    }
}

QRect StylePrivate::scrollBarInternalSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl) const
{
    const QRect &rect = option->rect;
    const QStyle::State &state(option->state);
    bool horizontal(state & QStyle::State_Horizontal);

    switch (subControl)
    {
    case QStyle::SC_ScrollBarSubLine:
    {
        int majorSize(this->scrollBarButtonHeight(_subLineButtons));
        if (horizontal)
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(rect.left(), rect.top(), majorSize, rect.height()));
        }
        else
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(rect.left(), rect.top(), rect.width(), majorSize));
        }
    }
    case QStyle::SC_ScrollBarAddLine:
    {
        int majorSize(this->scrollBarButtonHeight(_addLineButtons));
        if (horizontal)
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(rect.right() - majorSize + 1, rect.top(), majorSize, rect.height()));
        }
        else
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(rect.left(), rect.bottom() - majorSize + 1, rect.width(), majorSize));
        }
    }

    default:
        return QRect();
    }
}

void StylePrivate::drawPEIndicatorArrow(ArrowOrientation orientation, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto palettes = Palette::getDefault();
    auto arrowColor = Palette::getDefault()->getColor(option->state, Palette::ICON);

    RenderHelper::renderArrow(painter, option->rect, orientation, arrowColor);
}

StylePrivate::ButtonType StylePrivate::getButtonType(const QPushButton *btn)
{
    ButtonType buttonType = BUTTON_Normal;

    if (btn)
    {
        QVariant var = btn->property(KIRAN_STYLE_PROPERTY_BUTTON_TYPE);
        if (var.isValid())
        {
            bool toInt = false;
            auto temp = static_cast<ButtonType>(var.toInt(&toInt));
            if (toInt)
            {
                buttonType = temp;
            }
        }
    }

    return buttonType;
}

/* 这里应该需要给QProxyStyle指定一个非Kiran::Theme::Style类型的Style作为fallback，而不是使用QProxyStyle自动生成Style，
   因为自动生成的Style可能是Kiran::Theme::Style。最终导致死循环。整个顺序为Kiran::Theme::Style -> QProxyStyle -> QFusionStyle，
   如果QFusionStyle使用了proxy()函数，则又会回到Kiran::Theme::Style*/
Style::Style() : QProxyStyle(QStyleFactory::create("fusion")),
                 d_ptr(new StylePrivate(this))
{
}

Style::~Style()
{
    delete d_ptr;
}

int Style::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    switch (hint)
    {
    // 下栏框的下栏列表鼠标追踪
    case SH_ComboBox_ListMouseTracking:
        return true;
    // 菜单栏鼠标追踪
    case SH_MenuBar_MouseTracking:
        return true;
    // 菜单鼠标追踪
    case SH_Menu_MouseTracking:
        return true;
    // 菜单之中子菜单弹出延时时间
    case SH_Menu_SubMenuPopupDelay:
        return 150;
    // 弹出菜单是否支持用户在跨越菜单的其他项时将鼠标光标移动到子菜单
    case SH_Menu_SloppySubMenus:
        return true;
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
    // 该值目前已废弃，改用SH_Widget_Animation_Duration
    case SH_Widget_Animate:
        return true;
#else
    // 动画持续时间ms,0代表禁用
    case SH_Widget_Animation_Duration:
        return 100;
#endif
    // 确定样式是在菜单中显示节，还是将其视为普通分隔符。节是带有文本和图标提示的分隔符
    case SH_Menu_SupportsSections:
        return true;
    // 指示QDialogButtonBox中的标准按钮是否应具有图标
    case SH_DialogButtonBox_ButtonsHaveIcons:
        return false;
    // GroupBox文本标签垂直对齐选项
    case SH_GroupBox_TextLabelVerticalAlignment:
        return Qt::AlignVCenter;
    // TabBar的对齐方式
    case SH_TabBar_Alignment:
        return Qt::AlignLeft | Qt::AlignVCenter;
    // ToolBox中所选页面标题是否显示粗体
    case SH_ToolBox_SelectedPageTitleBold:
        return false;
    // 滚动条上单击鼠标中键滑块是否跳转到该绝对位置
    case SH_ScrollBar_MiddleClickAbsolutePosition:
        return true;
    // ScrollView是仅围绕内容（如Motif）还是围绕内容、滚动条和角部件（如窗口）绘制框架。
    case SH_ScrollView_FrameOnlyAroundContents:
        return false;
    // FormLayout对齐其内容的默认方式
    case SH_FormLayoutFormAlignment:
        return Qt::AlignLeft | Qt::AlignVCenter;
    // FormatLayout对齐标签的默认方式
    case SH_FormLayoutLabelAlignment:
        return Qt::AlignRight;
    // QFormLayout中字段增长方式的默认值。返回QFormLayout:：FieldGrowthPolicy枚举
    case SH_FormLayoutFieldGrowthPolicy:
        return QFormLayout::ExpandingFieldsGrow;
    // QFormLayout中如何换行的默认方式。返回一个QFormLayout::RowWrapPolicy enum。
    case SH_FormLayoutWrapPolicy:
        return QFormLayout::DontWrapRows;
    // 消息框中的文本是否允许用户交互(如选择)
    case SH_MessageBox_TextInteractionFlags:
        return Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse;
    // 进度对话框按钮取消按钮是否居中对齐，否则右对齐
    case SH_ProgressDialog_CenterCancelButton:
        return false;
    // 消息框是否按钮居中
    case SH_MessageBox_CenterButtons:
        return false;
    // 输入控件请求输入面板的时间点,返回类型为QStyle::RequestSoftwareInputPanel。
    case SH_RequestSoftwareInputPanel:
        return RSIP_OnMouseClick;
    // 标题栏无边框
    case SH_TitleBar_NoBorder:
        return true;
    // DockWidget的按钮是否有Frame
    case SH_DockWidget_ButtonsHaveFrame:
        return false;
    // ToolTip透明文本标签的透明度
    case SH_ToolTipLabel_Opacity:
        return 204;
    // Table里网格线的颜色
    case SH_Table_GridLineColor:
        return Palette::getDefault()->getBaseColors().widgetBorder.rgb();
    default:
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}

int Style::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric)
    {
    case PM_DefaultFrameWidth:
        if (qobject_cast<const QAbstractScrollArea *>(widget))
        {
            return 2;
        }
        return 6;
    case PM_SpinBoxFrameWidth:
        return 2;
    case PM_ToolBarFrameWidth:
        return 6;
    case PM_ToolTipLabelFrameWidth:
        return 3;
    case PM_ComboBoxFrameWidth:
        return 3;
    case PM_FocusFrameVMargin:
    case PM_FocusFrameHMargin:
        return 2;

    // 布局默认边距
    case PM_LayoutLeftMargin:
    case PM_LayoutTopMargin:
    case PM_LayoutRightMargin:
    case PM_LayoutBottomMargin:
    {
        // 使用子控件边距或顶层窗口边距，取决于部件类型
        if ((option && (option->state & QStyle::State_Window)) || (widget && widget->isWindow()))
        {
            return 10;
        }
        else
        {
            return 6;
        }
    }

    // 布局默认间距
    case PM_LayoutHorizontalSpacing:
    case PM_LayoutVerticalSpacing:
        return 6;

    // 按钮边距
    case PM_ButtonMargin:
        return 12;

    // 按钮默认指示器边框宽度
    case PM_ButtonDefaultIndicator:
        return 0;
    // 当按钮按下时，按钮的内容水平移动偏移量。
    case PM_ButtonShiftHorizontal:
        return 0;
    // 当按钮按下时，按钮的内容垂直移动偏移量。
    case PM_ButtonShiftVertical:
        return 0;

    // 菜单栏默认边框宽度
    case PM_MenuBarPanelWidth:
        return 0;
    // 菜单栏水平边距
    case PM_MenuBarHMargin:
        return 0;
    // 菜单栏垂直边距
    case PM_MenuBarVMargin:
        return 3;
    // 菜单项间距
    case PM_MenuBarItemSpacing:
        return 10;
    // 桌面菜单边框宽度
    case PM_MenuDesktopFrameWidth:
        return 0;

    // 菜单按钮指示器宽度
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
        return 5;
    case PM_ScrollBarSliderMin:
        return 20;

        // scrollview
    case PM_ScrollView_ScrollBarOverlap:
        return 1;

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

    default:  // fallback
        break;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

void Style::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    PainterSaver painterSaver(painter);

    switch (element)
    {
    case PE_Frame:
        if (widget && widget->inherits("QComboBoxPrivateContainer"))
        {
            QStyleOption copy = *option;
            copy.state |= State_Raised;
            this->drawPrimitive(PE_PanelMenu, &copy, painter, widget);
        }
        else
        {
            this->drawPEFrame(option, painter, widget);
        }
        break;
        //    case PE_FrameFocusRect:
        //        this->drawPEFrameFocusRect(option, painter, widget);
        //        break;
    case PE_FrameGroupBox:
        this->drawPEFrameGroupBox(option, painter, widget);
        break;
    case PE_PanelButtonCommand:
        this->drawPEPanelButtonCommand(option, painter, widget);
        break;
    case PE_PanelButtonTool:
        this->drawPEPanelButtonTool(option, painter, widget);
        break;
    case PE_FrameLineEdit:
        this->drawPEFrameLineEdit(option, painter, widget);
        break;
    case PE_PanelLineEdit:
    {
        /*作为QComoBox和QAbstractSpinBox的子控件时不进行绘制*/
        if (widget && widget->parentWidget())
        {
            if (qobject_cast<const QComboBox *>(widget->parentWidget()) ||
                qobject_cast<const QAbstractSpinBox *>(widget->parentWidget()))
            {
                return;
            }
        }

        if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(option))
        {
            if (panel->lineWidth > 0)
            {
                return Style::drawPrimitive(PE_FrameLineEdit, panel, painter, widget);
            }
            return;
        }

        break;
    }
    case PE_IndicatorButtonDropDown:
        this->drawPEIndicatorButtonDropDown(option, painter, widget);
        break;
    case PE_IndicatorArrowUp:
        this->drawPEIndicatorArrowUp(option, painter, widget);
        break;
    case PE_IndicatorArrowDown:
        this->drawPEIndicatorArrowDown(option, painter, widget);
        break;
    case PE_IndicatorArrowLeft:
        this->drawPEIndicatorArrowLeft(option, painter, widget);
        break;
    case PE_IndicatorArrowRight:
        this->drawPEIndicatorArrowRight(option, painter, widget);
        break;
    case PE_IndicatorRadioButton:
        this->drawPEIndicatorRadioButton(option, painter, widget);
        break;
    case PE_IndicatorCheckBox:
        this->drawPEIndicatorCheckBox(option, painter, widget);
        break;
    case PE_IndicatorBranch:
        this->drawPEIndicatorBranch(option, painter, widget);
        break;
    case PE_FrameTabWidget:
        this->drawPEFrameTabWidget(option, painter, widget);
        break;
    case PE_IndicatorToolBarSeparator:
        this->drawPEIndicatorToolBarSeparator(option, painter, widget);
        break;
    case PE_IndicatorToolBarHandle:
        this->drawPEIndicatorToolBarHandle(option, painter, widget);
        break;
    case PE_PanelMenu:
        this->drawPEPanelMenu(option, painter, widget);
        break;
    //    case PE_PanelItemViewRow:
    //        this->drawPanelItemViewRow(option, painter, widget);
    //        break;
    //    case PE_PanelItemViewItem:
    //        this->drawPanelItemViewItem(option, painter, widget);
    //        break;
    default:
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        break;
    }
}

void Style::drawComplexControl(QStyle::ComplexControl control,
                               const QStyleOptionComplex *option,
                               QPainter *painter,
                               const QWidget *widget) const
{
    PainterSaver painterSaver(painter);

    switch (control)
    {
    case QStyle::CC_ToolButton:
        this->drawCCToolButton(option, painter, widget);
        break;
    case QStyle::CC_ComboBox:
        this->drawCCComboBox(option, painter, widget);
        break;
    case QStyle::CC_ScrollBar:
        this->drawCCScrollBar(option, painter, widget);
        break;
    case QStyle::CC_SpinBox:
        this->drawCCSpinBox(option, painter, widget);
        break;
    case QStyle::CC_Slider:
        this->drawCCSlider(option, painter, widget);
        break;
    default:
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        break;
    }
}

QRect Style::subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    switch (element)
    {
    case QStyle::SE_TabBarTearIndicator:
    case QStyle::SE_TabBarTabLeftButton:
    case QStyle::SE_TabBarTabRightButton:
    case QStyle::SE_TabBarTabText:
    case QStyle::SE_TabBarScrollLeftButton:
    case QStyle::SE_TabBarScrollRightButton:
    case QStyle::SE_TabBarTearIndicatorRight:
        return this->tabBarSubElementRect(element, option, widget);
    case SE_ProgressBarGroove:
    case SE_ProgressBarContents:
    case SE_ProgressBarLabel:
        return this->progressBarElementRect(element, option, widget);
    case QStyle::SE_SliderFocusRect:
        return this->sliderElementRect(element, option, widget);
    default:
        break;
    }

    return QProxyStyle::subElementRect(element, option, widget);
}

QRect Style::subControlRect(QStyle::ComplexControl complexControl,
                            const QStyleOptionComplex *option,
                            QStyle::SubControl subControl,
                            const QWidget *widget) const
{
    switch (complexControl)
    {
    case QStyle::CC_ToolButton:
        return this->toolButtonSubControlRect(option, subControl, widget);
    case QStyle::CC_ComboBox:
        return this->comboBoxSubControlRect(option, subControl, widget);
    case QStyle::CC_ScrollBar:
        return this->scrollBarSubControlRect(option, subControl, widget);
    case QStyle::CC_SpinBox:
        return this->spinBoxSubControlRect(option, subControl, widget);
    // TODO: 先注释
    // case QStyle::CC_GroupBox:
    //     return this->groupBoxSubControlRect(option, subControl, widget);
    case QStyle::CC_Slider:
        return this->sliderSubControlRect(option, subControl, widget);
    default:
        return QProxyStyle::subControlRect(complexControl, option, subControl, widget);
    }
}

QSize Style::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    switch (type)
    {
    case CT_PushButton:
        return this->pushButtonSizeFromContents(option, contentSize, widget);
    case CT_ToolButton:
        return this->toolButtonSizeFromContents(option, contentSize, widget);
    case CT_TabBarTab:
        return this->tabBarTabSizeFromContents(option, contentSize, widget);
    case CT_ComboBox:
        return this->comboBoxSizeFromContents(option, contentSize, widget);
    case CT_SpinBox:
        return this->spinBoxSizeFromContents(option, contentSize, widget);
    case CT_GroupBox:
        return this->groupBoxSizeFromContents(option, contentSize, widget);
    case CT_ProgressBar:
        return this->progressBarSizeFromContents(option, contentSize, widget);
    case CT_Slider:
        return this->sliderSizeFromContents(option, contentSize, widget);
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
        int buttuonMargin = pixelMetric(QStyle::PM_ButtonMargin, option, widget);
        QSize size = contentSize + QSize(buttuonMargin, buttuonMargin);
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
    return QProxyStyle::sizeFromContents(type, option, contentSize, widget);
}

void Style::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    PainterSaver painterSaver(painter);

    switch (element)
    {
    case CE_PushButton:
        this->drawPEPushButton(element, option, painter, widget);
        break;
    case CE_ShapedFrame:
        this->drawControlShapedFrame(option, painter, widget);
        break;
    case CE_TabBarTabShape:
        this->drawControlTabBarTabShape(option, painter, widget);
        break;
    case CE_ToolBoxTabLabel:
        this->drawControlToolButtonLabel(option, painter, widget);
        break;
    case CE_ComboBoxLabel:
        this->drawControlComboBoxLabel(option, painter, widget);
        break;
    case CE_HeaderLabel:
        this->drawControlHeaderLabel(option, painter, widget);
        break;
    case CE_HeaderSection:
        this->drawControlHeaderSection(option, painter, widget);
        break;
    case CE_HeaderEmptyArea:
        this->drawControlHeaderEmptyArea(option, painter, widget);
        break;
    case CE_ScrollBarSlider:
        this->drawControlScrollBarSlider(option, painter, widget);
        break;
    case CE_ProgressBar:
        this->drawControlProgressBar(option, painter, widget);
        break;
    case CE_ProgressBarGroove:
        this->drawControlProgressBarGroove(option, painter, widget);
        break;
    case CE_ProgressBarContents:
        this->drawControlProgressBarContents(option, painter, widget);
        break;
    case CE_ProgressBarLabel:
        this->drawControlProgressBarLabel(option, painter, widget);
        break;
    case CE_MenuBarItem:
        this->drawControlMenuBarItem(option, painter, widget);
        break;
    case CE_MenuBarEmptyArea:
        this->drawControlMenuBarEmptyArea(option, painter, widget);
        break;
    // 不绘制的控件元素集合
    case CE_ToolBar:
        break;
    case CE_ItemViewItem:
        this->drawItemViewItem(element, option, painter, widget);
        break;
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
        break;
    }
}

QPixmap Style::standardPixmap(QStyle::StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget) const
{
    switch (standardPixmap)
    {
    case SP_ArrowUp:
    case SP_ArrowDown:
    case SP_ArrowLeft:
    case SP_ArrowRight:
    {
        QPalette palette = widget ? widget->palette() : qApp->palette();
        QPalette::ColorRole colorRole = widget ? widget->foregroundRole() : QPalette::ButtonText;
        QPixmap arrowPixmap(16, 16);
        arrowPixmap.fill(Qt::transparent);
        QPainter painter(&arrowPixmap);
        RenderHelper::renderArrow(&painter, arrowPixmap.rect(), (Kiran::ArrowOrientation)((int)standardPixmap - SP_ArrowUp), palette.color(colorRole));
        return arrowPixmap;
    }
    default:
        break;
    }
    return QProxyStyle::standardPixmap(standardPixmap, opt, widget);
}

QIcon Style::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
{
    static const QMap<QStyle::StandardPixmap, QString> titleBarIconMap = {
        {SP_TitleBarMinButton, ":/style/images/window-minimum-symbolic.svg"},
        {SP_TitleBarMaxButton, ":/style/images/window-maximum-symbolic.svg"},
        {SP_TitleBarCloseButton, ":/style/images/window-close-symbolic.svg"},
        {SP_TitleBarNormalButton, ":/style/images/window-unmaximum-symbolic.svg"},

    };
    switch (standardIcon)
    {
    case SP_TitleBarMinButton:
    case SP_TitleBarMaxButton:
    case SP_TitleBarCloseButton:
    case SP_TitleBarNormalButton:
    {
        QSize defaultTitleBarIconSize(16, 16);

        QIcon res;
        auto iter = titleBarIconMap.find(standardIcon);
        QString iconPath = iter.value();
        QPalette palette = qApp->palette();

        QPixmap normal = RenderHelper::changeSVGFillColor(iconPath, palette.color(QPalette::Normal, QPalette::Foreground), defaultTitleBarIconSize);
        res.addPixmap(normal, QIcon::Normal);

        QPixmap disable = RenderHelper::changeSVGFillColor(iconPath, palette.color(QPalette::Disabled, QPalette::Foreground), defaultTitleBarIconSize);
        res.addPixmap(normal, QIcon::Disabled);
        return res;
    }
    default:
        break;
    }
    return QProxyStyle::standardIcon(standardIcon, option, widget);
}

void Style::polishScrollArea(QAbstractScrollArea *scrollArea)
{
    if (!scrollArea)
        return;

    // enable mouse over effect in sunken scrollareas that support focus
    if (scrollArea->frameShadow() == QFrame::Sunken && scrollArea->focusPolicy() & Qt::StrongFocus)
    {
        scrollArea->setAttribute(Qt::WA_Hover);
    }

    // disable autofill background for flat (== NoFrame) scrollareas, with QPalette::Window as a background
    // this fixes flat scrollareas placed in a tinted widget, such as groupboxes, tabwidgets or framed dock-widgets
    if (!(scrollArea->frameShape() == QFrame::NoFrame || scrollArea->backgroundRole() == QPalette::Window))
    {
        return;
    }

    // get viewport and check background role
    QWidget *viewport(scrollArea->viewport());
    if (!(viewport && viewport->backgroundRole() == QPalette::Window))
        return;

    // change viewport autoFill background.
    // do the same for all children if the background role is QPalette::Window
    viewport->setAutoFillBackground(false);
    QList<QWidget *> children(viewport->findChildren<QWidget *>());
    foreach (QWidget *child, children)
    {
        if (child->parent() == viewport && child->backgroundRole() == QPalette::Window)
        {
            child->setAutoFillBackground(false);
        }
    }
}

void Style::polish(QWidget *widget)
{
    RETURN_IF_FALSE(widget);

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

    polishScrollArea(qobject_cast<QAbstractScrollArea *>(widget));

    if (QAbstractItemView *itemView = qobject_cast<QAbstractItemView *>(widget))
    {
        // enable mouse over effects in itemviews' viewport
        itemView->viewport()->setAttribute(Qt::WA_Hover);
    }

    QProxyStyle::polish(widget);
}

void Style::polish(QApplication *application)
{
    QProxyStyle::polish(application);
}

void Style::polish(QPalette &palette)
{
    // 用于初始化部件的外观，会在部件创建完成之后，在第一次显示之前被调用，默认实现什么也不做。
    // 子类化 QStyle 时，可利用该函数的调用时机，对部件的一些属性进行初始化。
    // 通常在此函数内指定配色方案，也即配置调色板，改变调色板为样式指定的颜色调色板。
    DEFAULT_PALETTE()->polish(palette);
}

void Style::drawPEFrame(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto palettes = Palette::getDefault();
    auto background = Palette::getDefault()->getColor(option->state, Palette::WINDOW);
    auto border = Palette::getDefault()->getColor(option->state, Palette::BORDER);

    if (qobject_cast<const QAbstractScrollArea *>(widget))
    {
        RenderHelper::renderFrame(painter, option->rect, 1, 0, Qt::transparent, border);
    }
    else
    {
        RenderHelper::renderFrame(painter, option->rect, 1, 0, background, border);
    }
}

void Style::drawPEFrameFocusRect(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    RETURN_IF_FALSE(qstyleoption_cast<const QStyleOptionFocusRect *>(option));

    auto background = Palette::getDefault()->getColor(option->state, Palette::WIDGET);
    const QStyle::State &state(option->state);
    bool hasFocus(state & QStyle::State_HasFocus);

    if (hasFocus)
    {
        background = Qt::transparent;
        QColor borderColor = DEFAULT_PALETTE()->getColor(Palette::SELECTED, Palette::WIDGET);
        RenderHelper::renderFrame(painter, option->rect, 1, 4, background, borderColor);
    }
}

void Style::drawPEFrameGroupBox(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    RETURN_IF_FALSE(qstyleoption_cast<const QStyleOptionFrame *>(option));

    auto topMargin = qMax(pixelMetric(PM_ExclusiveIndicatorHeight), option->fontMetrics.height()) + groupBoxTopMargin;
    auto frame = option->rect.adjusted(0, topMargin, 0, 0);
    auto background = DEFAULT_PALETTE()->getBaseColors().baseBackground;
    auto border = DEFAULT_PALETTE()->getBaseColors().widgetBorder;
    RenderHelper::renderFrame(painter, frame, 1, 0, background, border);
}

void Style::drawControlShapedFrame(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto frameOption = qstyleoption_cast<const QStyleOptionFrame *>(option);
    RETURN_IF_FALSE(frameOption);

    switch (frameOption->frameShape)
    {
    case QFrame::Box:
        break;
    case QFrame::HLine:
    case QFrame::VLine:
    {
        const QRect &rect(option->rect);
        bool isVertical(frameOption->frameShape == QFrame::VLine);
        auto color = Palette::getDefault()->getColor(option->state, Palette::BORDER);
        RenderHelper::renderSeparator(painter, option->rect, isVertical, color);
        break;
    }
    case QFrame::StyledPanel:
    {
        if (RenderHelper::isQtQuickControl(option, widget))
        {
            this->drawPrimitive(QStyle::PE_FrameMenu, option, painter, widget);
        }
        break;
    }
    default:
        break;
    }
}

QSize Style::defaultSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    return contentSize;
}

// 根据按钮内容计算按钮尺寸
QSize Style::pushButtonSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    const auto buttonOption(qstyleoption_cast<const QStyleOptionButton *>(option));
    if (!buttonOption) return contentSize;

    QSize size;

    const bool hasText(!buttonOption->text.isEmpty());
    const bool flat(buttonOption->features & QStyleOptionButton::Flat);
    bool hasIcon(!buttonOption->icon.isNull());

    if (!(hasText || hasIcon))
    {
        // 没有文本以及图标，采用自定义按钮作为内容大小的起点
        size = contentSize;
    }
    else
    {
        // 不管Qt在内容大小如何计算，完全根据按钮选项重新计算按钮大小,保证渲染阶段的一致性
        hasIcon &= (RenderHelper::showIconsOnPushButtons() || flat || !hasText);

        // 文本大小
        if (hasText)
            size = buttonOption->fontMetrics.size(Qt::TextShowMnemonic, buttonOption->text);

        // 图标大小
        if (hasIcon)
        {
            QSize iconSize = buttonOption->iconSize;
            if (!iconSize.isValid())
            {
                iconSize = QSize(this->pixelMetric(QStyle::PM_SmallIconSize, option, widget),
                                 this->pixelMetric(QStyle::PM_SmallIconSize, option, widget));
            }

            size.setHeight(qMax(size.height(), iconSize.height()));
            size.rwidth() += iconSize.width();

            if (hasText) size.rwidth() += Metrics::Button_ItemSpacing;
        }
    }

    // 菜单
    const bool hasMenu(buttonOption->features & QStyleOptionButton::HasMenu);
    if (hasMenu)
    {
        size.rwidth() += Metrics::MenuButton_IndicatorWidth;
        if (hasText || hasIcon) size.rwidth() += Metrics::Button_ItemSpacing;
    }

    // 扩展按钮内边距
    size = RenderHelper::expandSize(size, Metrics::Button_MarginWidth, Metrics::Button_MarginHeight);
    // 扩张按钮边框宽度
    size = RenderHelper::expandSize(size, Metrics::Frame_FrameWidth);

    // 确保按钮有最小的宽度
    if (hasText)
    {
        size.setWidth(qMax(size.width(), int(Metrics::Button_MinWidth)));
    }

    // 确保按钮有最小高度
    size.setHeight(qMax(size.height(), int(Metrics::Button_MinHeight)));

    return size;
}

// 根据ToolButton内容大小获取尺寸
QSize Style::toolButtonSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    const auto toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton *>(option);
    if (!toolButtonOption) return contentSize;

    QSize size = contentSize;

    const QStyle::State &state(option->state);

    // Auto Raise标志： true表示自动突出，false表示与父窗口齐平
    const bool autoRaise(state & QStyle::State_AutoRaise);
    const bool hasPopupMenu(toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup);
    const bool hasInlineIndicator(toolButtonOption->features & QStyleOptionToolButton::HasMenu && toolButtonOption->features & QStyleOptionToolButton::PopupDelay && !hasPopupMenu);

    // 若存在内部指示器，宽度加上一个内部指示器的宽度
    if (hasInlineIndicator) size.rwidth() += Metrics::ToolButton_InlineIndicatorWidth;

    // 总边距宽度=按钮边距宽度+Frame宽度
    int marginWidth = Metrics::Button_MarginWidth + Metrics::Frame_FrameWidth;
    // 四周扩展边距宽度
    size = RenderHelper::expandSize(size, marginWidth);

    return size;
}

QRect Style::toolButtonSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    const auto toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton *>(option);
    RETURN_VAL_IF_FALSE(toolButtonOption, QRect());

    bool hasPopupMenu(toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup);
    const bool hasInlineIndicator(toolButtonOption->features & QStyleOptionToolButton::HasMenu &&
                                  toolButtonOption->features & QStyleOptionToolButton::PopupDelay &&
                                  !hasPopupMenu);

    const QRect &rect(option->rect);
    int menuButtonWidth(Metrics::MenuButton_IndicatorWidth);

    switch (subControl)
    {
    case QStyle::SC_ToolButtonMenu:
    {
        RETURN_VAL_IF_TRUE(!(hasPopupMenu || hasInlineIndicator), QRect());

        QRect menuRect(rect);
        // 定位菜单按钮为ToolButton右侧
        menuRect.setLeft(rect.right() - menuButtonWidth + 1);
        if (hasInlineIndicator)
        {
            menuRect.setTop(menuRect.bottom() - menuButtonWidth);
        }
        return QStyle::visualRect(option->direction, option->rect, menuRect);
    }
    case QStyle::SC_ToolButton:
    {
        if (hasPopupMenu)
        {
            QRect contentsRect(rect);
            contentsRect.setRight(rect.right() - menuButtonWidth);
            return QStyle::visualRect(option->direction, option->rect, contentsRect);
        }
        else
        {
            return rect;
        }
        break;
    }
    default:
        return QProxyStyle::subControlRect(QStyle::CC_ToolButton, option, subControl, widget);
    }
}

void Style::drawPEPushButton(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
    RETURN_IF_FALSE(buttonOption);

    const QRect &rect(buttonOption->rect);
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));
    bool hasFocus((enabled && (state & QStyle::State_HasFocus)) && !(widget && widget->focusProxy()));
    bool sunken = (state & (QStyle::State_On | QStyle::State_Sunken));
    bool flat(buttonOption->features & QStyleOptionButton::Flat);

    auto palette = Palette::getDefault();
    auto background = palette->getColor(option->state, Palette::WIDGET);
    auto border = palette->getColor(option->state, Palette::BORDER);

    StylePrivate::ButtonType buttonType = StylePrivate::getButtonType(qobject_cast<const QPushButton *>(widget));
    // 若为 Button_Default 按钮中的文字为白色
    if (enabled && buttonType == StylePrivate::BUTTON_Default)
    {
        QColor textColor = Qt::white;  // 主按钮文字总为白色
        QStyleOptionButton modifiedOption(*buttonOption);
        modifiedOption.palette.setColor(QPalette::ButtonText, textColor);
        QProxyStyle::drawControl(element, &modifiedOption, painter, widget);
        return;
    }
    if (enabled && qobject_cast<const QPushButton *>(widget) && StylePrivate::getButtonType(qobject_cast<const QPushButton *>(widget)) != StylePrivate::BUTTON_Normal)
    {
        if (buttonType == StylePrivate::BUTTON_Warning)
        {
            QColor textColor = DEFAULT_PALETTE()->getBaseColors().widgetWarning;
            QStyleOptionButton modifiedOption(*buttonOption);
            modifiedOption.palette.setColor(QPalette::ButtonText, textColor);
            QProxyStyle::drawControl(element, &modifiedOption, painter, widget);
            return;
        }
    }
    QProxyStyle::drawControl(element, buttonOption, painter, widget);
}

void Style::drawItemViewItem(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto itemOption = qstyleoption_cast<const QStyleOptionViewItem *>(option);
    if (itemOption && itemOption->state & QStyle::State_Selected)
    {
        QStyleOptionViewItem newOption(*itemOption);
        //        newOption.palette.setColor(QPalette::HighlightedText, Qt::red);
        //        newOption.palette.setColor(QPalette::Highlight, Qt::transparent);
        QProxyStyle::drawControl(element, &newOption, painter, widget);
        return;
    }

    QProxyStyle::drawControl(element, option, painter, widget);
}

void Style::drawPEPanelButtonCommand(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
    RETURN_IF_FALSE(buttonOption);

    const QRect &rect(option->rect);
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));
    bool hasFocus((enabled && (state & QStyle::State_HasFocus)) && !(widget && widget->focusProxy()));
    bool hasKeyboardFocusChange((option->state & State_KeyboardFocusChange) && hasFocus);  // 加入 State_KeyboardFocusChange 的目的：确保是使用 TAB 键切换焦点才显示选中边框
    bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    bool flat(buttonOption->features & QStyleOptionButton::Flat);

    auto palettes = Palette::getDefault();
    auto background = palettes->getColor(option->state, Palette::WIDGET);
    auto border = palettes->getColor(option->state, Palette::BORDER);

    StylePrivate::ButtonType buttonType = StylePrivate::getButtonType(qobject_cast<const QPushButton *>(widget));
    if (enabled && qobject_cast<const QPushButton *>(widget) && buttonType != StylePrivate::BUTTON_Normal)
    {
        if (buttonType == StylePrivate::BUTTON_Default)
        {
            background = DEFAULT_PALETTE()->getBaseColors().widgetMain;
            if (mouseOver)
            {
                background = DEFAULT_PALETTE()->getColor(Palette::MOUSE_OVER, Palette::WIDGET);
                background = QColor(112, 203, 255);  // FIXME: 颜色较特殊，暂时指定
            }
            if (sunken)
            {
                background = DEFAULT_PALETTE()->getColor(Palette::SUNKEN, Palette::WIDGET);
                background = QColor(0, 112, 176);  // FIXME: 颜色较特殊，暂时指定
            }
        }
        if (buttonType == StylePrivate::BUTTON_Warning)
        {
            if (sunken)
            {
                background = DEFAULT_PALETTE()->getColor(Palette::SELECTED, Palette::WINDOW);
            }
        }
    }

    if (flat && !sunken)
    {
        background = Qt::transparent;
    }

    if (hasKeyboardFocusChange && !sunken)
    {
        border = DEFAULT_PALETTE()->getColor(Palette::SELECTED, Palette::WIDGET);
    }

    RenderHelper::renderFrame(painter, option->rect, 1, 4, background, border);
}

void Style::drawPEPanelButtonTool(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const QPalette &palette(option->palette);
    QRect rect(option->rect);

    const QStyle::State &state(option->state);
    bool autoRaise(state & QStyle::State_AutoRaise);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    bool mouseOver((state & QStyle::State_Active) && enabled && (option->state & QStyle::State_MouseOver));
    bool hasFocus(enabled && (option->state & (QStyle::State_HasFocus)));

    if (!autoRaise || mouseOver || sunken)
    {
        // need to check widget for popup mode, because option is not set properly
        const auto *toolButton(qobject_cast<const QToolButton *>(widget));
        bool hasPopupMenu(toolButton && toolButton->popupMode() == QToolButton::MenuButtonPopup);

        // adjust frame in case of menu
        if (hasPopupMenu)
        {
            painter->setClipRect(rect);
            rect.adjust(0, 0, Metrics::Frame_FrameRadius + 2, 0);
            rect = QStyle::visualRect(option->direction, option->rect, rect);
        }

        auto background = Palette::getDefault()->getColor(option->state, Palette::WIDGET);
        auto border = Palette::getDefault()->getColor(option->state, Palette::BORDER);
        RenderHelper::renderFrame(painter, rect, 1, 4, background, border);
    }
    else if (hasFocus)
    {
        auto background = Palette::getDefault()->getBaseColors().widgetWarning;
        //        auto border = DEFAULT_PALETTE()->getColor(Palette::SELECTED,Palette::WIDGET);
        RenderHelper::renderFrame(painter, rect, 1, 4, background);
    }
    else
    {
        RenderHelper::renderFrame(painter, rect, 1, 4, Qt::transparent);
    }
}

void Style::drawControlToolButtonLabel(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto toolButtonOption(qstyleoption_cast<const QStyleOptionToolButton *>(option));
    RETURN_IF_FALSE(toolButtonOption);

    const QRect &rect = option->rect;
    const QPalette &palette = option->palette;

    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    bool mouseOver((state & QStyle::State_Active) && enabled && (option->state & QStyle::State_MouseOver));
    bool flat(state & QStyle::State_AutoRaise);

    bool hasFocus(false);
    if (flat)
        hasFocus = enabled && !mouseOver && (option->state & QStyle::State_HasFocus);
    else
        hasFocus = enabled && !mouseOver && (option->state & (QStyle::State_HasFocus | QStyle::State_Sunken));

    bool hasArrow = toolButtonOption->features & QStyleOptionToolButton::Arrow;
    bool hasIcon(!(hasArrow || toolButtonOption->icon.isNull()));
    bool hasText(!toolButtonOption->text.isEmpty());

    QRect contentsRect(rect);
    QSize iconSize(toolButtonOption->iconSize);
    int textFlags(RenderHelper::mnemonicsTextFlags());
    QSize textSize(option->fontMetrics.size(textFlags, toolButtonOption->text));

    QRect iconRect;
    QRect textRect;

    if (hasText && (!(hasArrow || hasIcon) || toolButtonOption->toolButtonStyle == Qt::ToolButtonTextOnly))  // 只显示文字
    {
        textRect = contentsRect;
        textFlags |= Qt::AlignCenter;
    }
    else if ((hasArrow || hasIcon) && (!hasText || toolButtonOption->toolButtonStyle == Qt::ToolButtonIconOnly))  // 只显示图标
    {
        iconRect = contentsRect;
    }
    else if (toolButtonOption->toolButtonStyle == Qt::ToolButtonTextUnderIcon)  // 文字位于图标之下
    {
        int contentsHeight(iconSize.height() + textSize.height() + Metrics::ToolButton_ItemSpacing);
        iconRect = QRect(QPoint(contentsRect.left() + (contentsRect.width() - iconSize.width()) / 2, contentsRect.top() + (contentsRect.height() - contentsHeight) / 2), iconSize);
        textRect = QRect(QPoint(contentsRect.left() + (contentsRect.width() - textSize.width()) / 2, iconRect.bottom() + Metrics::ToolButton_ItemSpacing + 1), textSize);
        textFlags |= Qt::AlignCenter;
    }
    else
    {
        //        bool leftAlign(widget && widget->property(PropertyNames::toolButtonAlignment).toInt() == Qt::AlignLeft);
        //        if (leftAlign)
        //            iconRect = QRect(QPoint(contentsRect.left(), contentsRect.top() + (contentsRect.height() - iconSize.height()) / 2), iconSize);
        //        else
        {
            int contentsWidth(iconSize.width() + textSize.width() + Metrics::ToolButton_ItemSpacing);
            iconRect = QRect(QPoint(contentsRect.left() + (contentsRect.width() - contentsWidth) / 2, contentsRect.top() + (contentsRect.height() - iconSize.height()) / 2), iconSize);
        }

        textRect = QRect(QPoint(iconRect.right() + Metrics::ToolButton_ItemSpacing + 1, contentsRect.top() + (contentsRect.height() - textSize.height()) / 2), textSize);

        // handle right to left layouts
        iconRect = QStyle::visualRect(option->direction, option->rect, iconRect);
        textRect = QStyle::visualRect(option->direction, option->rect, textRect);

        textFlags |= Qt::AlignLeft | Qt::AlignVCenter;
    }

    // make sure there is enough room for icon
    if (iconRect.isValid()) iconRect = RenderHelper::centerRect(iconRect, iconSize);

    // render arrow or icon
    if (hasArrow && iconRect.isValid())
    {
        QStyleOptionToolButton copy(*toolButtonOption);
        copy.rect = iconRect;
        switch (toolButtonOption->arrowType)
        {
        case Qt::LeftArrow:
            this->drawPrimitive(QStyle::PE_IndicatorArrowLeft, &copy, painter, widget);
            break;
        case Qt::RightArrow:
            this->drawPrimitive(QStyle::PE_IndicatorArrowRight, &copy, painter, widget);
            break;
        case Qt::UpArrow:
            this->drawPrimitive(QStyle::PE_IndicatorArrowUp, &copy, painter, widget);
            break;
        case Qt::DownArrow:
            this->drawPrimitive(QStyle::PE_IndicatorArrowDown, &copy, painter, widget);
            break;
        default:
            break;
        }
    }
    else if (hasIcon && iconRect.isValid())
    {
        // icon state and mode
        const QIcon::State iconState(sunken ? QIcon::On : QIcon::Off);
        QIcon::Mode iconMode;
        if (!enabled)
            iconMode = QIcon::Disabled;
        else if (!flat && hasFocus)
            iconMode = QIcon::Selected;
        else if (mouseOver && flat)
            iconMode = QIcon::Active;
        else
            iconMode = QIcon::Normal;

        QPixmap pixmap = toolButtonOption->icon.pixmap(iconSize, iconMode, iconState);
        this->drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);
    }

    // render text
    if (hasText && textRect.isValid())
    {
        QPalette::ColorRole textRole(QPalette::ButtonText);
        if (flat)
            textRole = (hasFocus && sunken && !mouseOver) ? QPalette::HighlightedText : QPalette::WindowText;
        else if (hasFocus && !mouseOver)
            textRole = QPalette::HighlightedText;

        painter->setFont(toolButtonOption->font);
        this->drawItemText(painter, textRect, textFlags, palette, enabled, toolButtonOption->text, textRole);
    }
}

void Style::drawCCToolButton(const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    const auto *toolButtonOption(qstyleoption_cast<const QStyleOptionToolButton *>(option));
    RETURN_IF_FALSE(toolButtonOption);

    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool mouseOver((state & QStyle::State_Active) && enabled && (option->state & QStyle::State_MouseOver));
    bool hasFocus(enabled && (option->state & QStyle::State_HasFocus));
    bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    bool flat(state & QStyle::State_AutoRaise);

    bool isDockWidgetTitleButton(widget && widget->inherits("QDockWidgetTitleButton"));
    bool inTabBar(widget && qobject_cast<const QTabBar *>(widget->parentWidget()));
    bool isMenuTitle(RenderHelper::isMenuTitle(widget));

    if (isMenuTitle)
    {
        QStyleOptionToolButton copy(*toolButtonOption);
        copy.font.setBold(false);
        copy.state = QStyle::State_Enabled;

        return;
    }

    // copy option and alter palette
    QStyleOptionToolButton copy(*toolButtonOption);

    if (isDockWidgetTitleButton)
    {
        const QAbstractButton *button(qobject_cast<const QAbstractButton *>(widget));
        if (button->isChecked() || button->isDown())
        {
            copy.state |= QStyle::State_Enabled | QStyle::State_On | QStyle::State_Sunken;
        }
        if (button->underMouse())
        {
            copy.state |= QStyle::State_Enabled | QStyle::State_MouseOver | QStyle::State_Active;
        }
    }

    bool hasPopupMenu(toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup);
    const bool hasInlineIndicator(
        toolButtonOption->features & QStyleOptionToolButton::HasMenu && toolButtonOption->features & QStyleOptionToolButton::PopupDelay && !hasPopupMenu);

    QRect buttonRect(this->subControlRect(QStyle::CC_ToolButton, option, QStyle::SC_ToolButton, widget));
    QRect menuRect(this->subControlRect(QStyle::CC_ToolButton, option, QStyle::SC_ToolButtonMenu, widget));

    // frame
    if (toolButtonOption->subControls & QStyle::SC_ToolButton || isDockWidgetTitleButton)
    {
        copy.rect = buttonRect;
        if (inTabBar)
        {
            QRect rect(option->rect);

            auto background = Palette::getDefault()->getColor(option->state, Palette::WIDGET);

            painter->setPen(background);
            painter->setBrush(background);

            switch (toolButtonOption->arrowType)
            {
            case Qt::UpArrow:
                painter->drawRect(rect.adjusted(1, 1, -2, -1));
                break;
            case Qt::DownArrow:
                painter->drawRect(rect.adjusted(1, 0, -2, -2));
                break;
            case Qt::LeftArrow:
                painter->drawRect(rect.adjusted(1, 1, -1, -2));
                break;
            case Qt::RightArrow:
                painter->drawRect(rect.adjusted(0, 1, -2, -2));
                break;
            }

#if 0
            //位于QTabBar之上的QToolButton不绘制边框
            painter->setPen(border);
            switch (toolButtonOption->arrowType)
            {
            case Qt::DownArrow:
                painter->drawLine(rect.bottomLeft(), rect.bottomRight());
                break;
            case Qt::RightArrow:
                painter->drawLine(rect.topRight(), rect.bottomRight());
                break;
            }
            switch (toolButtonOption->arrowType)
            {
            case Qt::UpArrow:
            case Qt::DownArrow:
                painter->drawLine(rect.topLeft(), rect.bottomLeft());
                painter->drawLine(rect.topLeft(), rect.bottomLeft());
                break;
            case Qt::LeftArrow:
            case Qt::RightArrow:
                painter->drawLine(rect.topLeft(), rect.topRight());
                painter->drawLine(rect.bottomLeft(), rect.bottomRight());
                break;
            }
#endif
        }
        else if (sunken && hasPopupMenu && !(toolButtonOption->activeSubControls & QStyle::SC_ToolButton))
        {
            // Only menu button is active. so draw left hand side od button raised
            QStyleOptionToolButton btn(copy);
            btn.state |= QStyle::State_Raised;
            btn.state &= ~QStyle::State_Sunken;
            btn.state &= ~QStyle::State_AutoRaise;
            this->drawPrimitive(QStyle::PE_PanelButtonTool, &btn, painter, widget);
        }
        else
        {
            this->drawPrimitive(QStyle::PE_PanelButtonTool, &copy, painter, widget);
        }
    }

    // arrow
    if (hasPopupMenu)
    {
        copy.rect = menuRect;

        if (!flat || mouseOver || sunken)
            this->drawPrimitive(QStyle::PE_IndicatorButtonDropDown, &copy, painter, widget);

        this->drawPrimitive(QStyle::PE_IndicatorArrowDown, &copy, painter, widget);
    }

    else if (hasInlineIndicator)
    {
        copy.rect = menuRect;

        this->drawPrimitive(QStyle::PE_IndicatorArrowDown, &copy, painter, widget);
    }

    // contents
    {
        // restore state
        copy.state = state;

        // define contents rect
        QRect contentsRect(buttonRect);

        // detect dock widget title button
        // for dockwidget title buttons, do not take out margins, so that icon do not get scaled down
        if (isDockWidgetTitleButton)
        {
            // cast to abstract button
            // adjust state to have correct icon rendered
            const QAbstractButton *button(qobject_cast<const QAbstractButton *>(widget));
            if (button->isChecked() || button->isDown())
            {
                copy.state |= QStyle::State_Enabled | QStyle::State_On | QStyle::State_Sunken;
            }
            if (button->underMouse())
            {
                copy.state |= QStyle::State_Enabled | QStyle::State_MouseOver | QStyle::State_Active;
            }
        }
        else if (!inTabBar && hasInlineIndicator)
        {
            int marginWidth(flat ? Metrics::ToolButton_MarginWidth : Metrics::Button_MarginWidth + Metrics::Frame_FrameWidth);
            contentsRect = RenderHelper::insideMargin(contentsRect, marginWidth, 0);
            contentsRect.setRight(contentsRect.right() - Metrics::ToolButton_InlineIndicatorWidth);
            contentsRect = QStyle::visualRect(option->direction, option->rect, contentsRect);
        }

        copy.rect = contentsRect;

        // render
        this->drawControl(QStyle::CE_ToolButtonLabel, &copy, painter, widget);
    }
}

QSize Style::comboBoxSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    const auto comboBoxOption(qstyleoption_cast<const QStyleOptionComboBox *>(option));
    if (!comboBoxOption) return contentSize;

    QSize size(contentSize);

    // 若存在Frame绘制，大小扩展开  FrameWidth
    const bool flat(!comboBoxOption->frame);
    const int frameWidth(this->pixelMetric(QStyle::PM_ComboBoxFrameWidth, option, widget));
    if (!flat)
    {
        size = RenderHelper::expandSize(size, frameWidth);
    }

    size.rwidth() += Metrics::MenuButton_IndicatorWidth;
    size.rwidth() += Metrics::Button_ItemSpacing;

    size.rwidth() += size.height();

    // 确保有足够的高度绘制下拉按钮指示器
    size.setHeight(qMax(size.height(), int(Metrics::MenuButton_IndicatorWidth)));

    //    size = RenderHelper::expandSize(size, Metrics::ComboBox_MarginWidth, Metrics::ComboBox_MarginHeight);

    // 确保至少有最小大小
    size.setHeight(qMax(size.height(), int(Metrics::ComboBox_MinHeight)));
    size.setWidth(qMax(size.width(), int(Metrics::ComboBox_MinWidth)));

    return size;
}

QRect Style::comboBoxSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    const auto comboBoxOption(qstyleoption_cast<const QStyleOptionComboBox *>(option));
    if (!comboBoxOption) return QRect();

    const bool editable(comboBoxOption->editable);
    const bool flat(editable && !comboBoxOption->frame);
    auto rect(option->rect);
    const int frameWidth(this->pixelMetric(QStyle::PM_ComboBoxFrameWidth, option, widget));

    switch (subControl)
    {
    case QStyle::SC_ComboBoxFrame:
        return flat ? rect : QRect();
    case QStyle::SC_ComboBoxListBoxPopup:
        return rect;
    case QStyle::SC_ComboBoxArrow:
        return QRect(rect.right() - rect.height() + 1,
                     rect.top(),
                     rect.height(), rect.height());
        break;
    case QStyle::SC_ComboBoxEditField:
    {
        auto rect2 = QRect(rect.left(), rect.top(), rect.width() - rect.height() - 4, rect.height());
        rect2.adjust(frameWidth, frameWidth, 0, -frameWidth);
        return QStyle::visualRect(option->direction, option->rect, rect2);
    }
    default:
        return QProxyStyle::subControlRect(QStyle::CC_ComboBox, option, subControl, widget);
    }
}

void Style::drawCCComboBox(const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    const auto comboBoxOption(qstyleoption_cast<const QStyleOptionComboBox *>(option));
    RETURN_IF_FALSE(comboBoxOption);

    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool editable(comboBoxOption->editable);
    bool arrowActive(comboBoxOption->activeSubControls & QStyle::SC_ComboBoxArrow);
    bool flat(!comboBoxOption->frame);
    bool sunken;

    if (editable)
    {
        sunken = arrowActive && enabled && (state & (QStyle::State_On | QStyle::State_Sunken));
    }
    else
    {
        sunken = enabled && (state & (QStyle::State_On | QStyle::State_Sunken));
    }

    const auto comboBox = qobject_cast<const QComboBox *>(widget);
    if (!comboBox)
        return;

    const bool empty(comboBox && !comboBox->count());

    // TODO: 当ComboBox为空时，应当特别绘制
    if (empty)
    {
        const auto &background = option->palette.color(QPalette::Base);
        painter->fillRect(option->rect, background);
    }

    // frame
    if (option->subControls & QStyle::SC_ComboBoxFrame)
    {
        if (editable)
        {
            // FIXME:下拉框编辑状态悬浮判断没有区域
            flat |= (option->rect.height() <= 2 * Metrics::Frame_FrameWidth + Metrics::MenuButton_IndicatorWidth);
            if (flat)
            {
                const auto &background = option->palette.color(QPalette::Base);
                painter->setBrush(background);
                painter->setPen(Qt::NoPen);
                painter->drawRect(option->rect);
            }
            else
            {
                QRectF arrowRect = this->subControlRect(QStyle::CC_ComboBox, comboBoxOption, QStyle::SC_ComboBoxArrow, widget);

                // ComboBox 箭头按钮
                arrowRect.adjust(1.5, 1.5, -1.5, -1.5);
                QPainterPath indicatorPainterPath = RenderHelper::roundedPath(arrowRect, CornersRight, 4);

                auto background = Palette::getDefault()->getColor(option->state, Palette::WIDGET);
                auto border = Palette::getDefault()->getColor(option->state, Palette::BORDER);

                QPen pen = painter->pen();
                painter->setRenderHints(QPainter::Antialiasing);
                painter->setPen(border);
                painter->setBrush(background);
                painter->drawPath(indicatorPainterPath);

                // 输入框
                QStyleOptionComplex tmpOpt(*option);
                // 让输入框长度加上一个宽度进行绘制用来去掉输入框的圆角
                tmpOpt.rect.setWidth(tmpOpt.rect.width() - arrowRect.width() + 3);
                this->drawPrimitive(QStyle::PE_FrameLineEdit, &tmpOpt, painter, widget);
            }
        }
        else
        {
            if (flat)
            {
                auto background = Palette::getDefault()->getColor(option->state, Palette::WINDOW);
                RenderHelper::renderFrame(painter, option->rect, 1, 0, background);
            }
            else
            {
                auto backgroundColor = Palette::getDefault()->getColor(option->state, Palette::WIDGET);
                // combobox在选中状态时，背景颜色需设置为ACTIVE状态下的颜色
                if (option->state & QStyle::State_Selected)
                {
                    backgroundColor = Palette::getDefault()->getColor(Palette::ACTIVE, Palette::WIDGET);
                }

                auto borderColor = Palette::getDefault()->getColor(option->state, Palette::BORDER);
                RenderHelper::renderFrame(painter, option->rect, 1, 4, backgroundColor, borderColor);
            }
        }
    }

    // arrow
    if (option->subControls & QStyle::SC_ComboBoxArrow)
    {
        auto arrowRect(this->subControlRect(QStyle::CC_ComboBox, option, QStyle::SC_ComboBoxArrow, widget));
        auto arrowColor = Palette::getDefault()->getColor(option->state, Palette::ICON);
        RenderHelper::renderArrow(painter, arrowRect, Arrow_Down, arrowColor);
    }
}

void Style::drawControlComboBoxLabel(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto comboBoxOption(qstyleoption_cast<const QStyleOptionComboBox *>(option));
    if (!comboBoxOption) return;
    // TODO: 是否给基类处理？
    if (comboBoxOption->editable) return;

    const QStyle::State &state(option->state);
    const bool enabled(state & QStyle::State_Enabled);
    const bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));
    const bool mouseOver(enabled && (option->state & QStyle::State_MouseOver));
    const bool hasFocus(enabled && !mouseOver && (option->state & QStyle::State_HasFocus));
    const bool flat(!comboBoxOption->frame);

    QPalette::ColorRole textRole;
    if (flat)
    {
        textRole = QPalette::WindowText;
    }
    else if (hasFocus)
    {
        textRole = QPalette::ButtonText;
    }
    else
    {
        textRole = QPalette::ButtonText;
    }

    painter->setPen(QPen(option->palette.color(textRole), 1));

    // 获取文本框位置
    auto editRect = this->subControlRect(QStyle::CC_ComboBox, comboBoxOption, QStyle::SC_ComboBoxEditField, widget);

    painter->save();
    painter->setClipRect(editRect);
    // 绘制图标
    if (!comboBoxOption->currentIcon.isNull() && qobject_cast<const QComboBox *>(widget))
    {
        QIcon::Mode mode;

        if ((comboBoxOption->state & QStyle::State_Selected) && (comboBoxOption->state & QStyle::State_Active))
        {
            mode = QIcon::Selected;
        }
        else if (comboBoxOption->state & QStyle::State_Enabled)
        {
            mode = QIcon::Normal;
        }
        else
        {
            mode = QIcon::Disabled;
        }

        QPixmap pixmap = comboBoxOption->currentIcon.pixmap(comboBoxOption->iconSize, mode);
        QRect iconRect(editRect);
        iconRect.setWidth(comboBoxOption->iconSize.width() + 4);
        iconRect = QStyle::alignedRect(comboBoxOption->direction,
                                       Qt::AlignLeft | Qt::AlignVCenter,
                                       iconRect.size(), editRect);

        if (comboBoxOption->editable)
        {
            painter->fillRect(iconRect, option->palette.brush(QPalette::Base));
        }

        this->drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);

        if (comboBoxOption->direction == Qt::RightToLeft)
        {
            editRect.translate(-4 - comboBoxOption->iconSize.width(), 0);
        }
        else
        {
            editRect.translate(comboBoxOption->iconSize.width() + 4, 0);
        }
    }

    // 绘制文本
    if (!comboBoxOption->currentText.isEmpty() && !comboBoxOption->editable)
    {
        QRect itemTextRect = editRect.adjusted(Metrics::ComboBox_FrameWidth, 0, -1, 0);
        int itemTextFlags = QStyle::visualAlignment(comboBoxOption->direction, Qt::AlignLeft | Qt::AlignVCenter);
        bool itemTextEnable = comboBoxOption->state & QStyle::State_Enabled;
        this->drawItemText(painter, itemTextRect, itemTextFlags, comboBoxOption->palette, itemTextEnable, comboBoxOption->currentText);
    }

    painter->restore();
}

void Style::drawPEFrameLineEdit(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto &rect = option->rect;

    // QLineEdit::paintEvent中设置option->state |= QStyle::State_Sunken，导致获取的获取颜色为Sunken状态下的控件颜色，与预期不一致
    // 指定获取ACTIVE状态下的控件颜色，与QCombobox颜色保持一致
    auto background = DEFAULT_PALETTE()->getColor(Palette::ACTIVE, Palette::WIDGET);
    auto border = DEFAULT_PALETTE()->getColor(option->state, Palette::BORDER);

    // 控件高度不足够绘制边框
    if (rect.height() < 2 + option->fontMetrics.height())
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRect(rect);
        painter->restore();
    }
    else
    {
        if (qobject_cast<const QComboBox *>(widget))
        {
            RenderHelper::renderFlatFrame(painter, rect, 4, background, border);
        }
        else
        {
            RenderHelper::renderFrame(painter, rect, 1, 4, background, border);
        }
    }

    // 输入框聚焦时候高亮边框
    const QStyle::State &state(option->state);
    bool hasFocus(state & QStyle::State_HasFocus);

    if (hasFocus)
    {
        const auto frameOption = qstyleoption_cast<const QStyleOptionFrame *>(option);
        if (frameOption)
        {
            QColor borderColor = DEFAULT_PALETTE()->getColor(Palette::SELECTED, Palette::WIDGET);
            RenderHelper::renderFrame(painter, frameOption->rect, 1, 4, background, borderColor);
        }
    }
}

// copy from QCommonStyle
QSize Style::groupBoxSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    if (!widget || !widget->inherits("QGroupBox"))
    {
        return {};
    }

    auto groupBox = qobject_cast<const QGroupBox *>(widget);
    QSize size = contentSize + QSize(groupBox->isFlat() ? 16 : 0, 0);

    return size;
}

// copy from QCommonStyle
QRect Style::groupBoxSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    const auto groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option);
    RETURN_VAL_IF_FALSE(groupBox, QRect());

    switch (subControl)
    {
    case QStyle::SC_GroupBoxFrame:
    case QStyle::SC_GroupBoxContents:
    {
        int topMargin = 0;
        int topHeight = 0;
        int verticalAlignment = this->styleHint(QStyle::SH_GroupBox_TextLabelVerticalAlignment, groupBox, widget);
        if (groupBox->text.size() || (groupBox->subControls & QStyle::SC_GroupBoxCheckBox))
        {
            topHeight = groupBox->fontMetrics.height();
            if (verticalAlignment & Qt::AlignVCenter)
                topMargin = topHeight / 2;
            else if (verticalAlignment & Qt::AlignTop)
                topMargin = topHeight;
        }

        QRect frameRect = groupBox->rect;
        frameRect.setTop(topMargin);

        if (subControl == QStyle::SC_GroupBoxFrame)
        {
            return frameRect;
        }

        int frameWidth = 0;
        if ((groupBox->features & QStyleOptionFrame::Flat) == 0)
            frameWidth = this->pixelMetric(QStyle::PM_DefaultFrameWidth, groupBox, widget);
        return frameRect.adjusted(frameWidth, frameWidth + topHeight - topMargin, -frameWidth, -frameWidth);
    }
    case QStyle::SC_GroupBoxCheckBox:
    case QStyle::SC_GroupBoxLabel:
    {
        QRect controlRect;
        QFontMetrics fontMetrics = groupBox->fontMetrics;
        int h = fontMetrics.height();
        int tw = fontMetrics.size(Qt::TextShowMnemonic, groupBox->text + QLatin1Char(' ')).width();
        int marg = (groupBox->features & QStyleOptionFrame::Flat) ? 0 : 8;
        controlRect = groupBox->rect.adjusted(marg, 0, -marg, 0);
        controlRect.setHeight(h);

        int indicatorWidth = this->pixelMetric(QStyle::PM_IndicatorWidth, option, widget);
        int indicatorSpace = this->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, option, widget) - 1;
        bool hasCheckBox = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;
        int checkBoxSize = hasCheckBox ? (indicatorWidth + indicatorSpace) : 0;

        // Adjusted rect for label + indicatorWidth + indicatorSpace
        QRect totalRect = QStyle::alignedRect(groupBox->direction, groupBox->textAlignment,
                                              QSize(tw + checkBoxSize, h), controlRect);

        // Adjust totalRect if checkbox is set
        if (hasCheckBox)
        {
            bool ltr = groupBox->direction == Qt::LeftToRight;
            int left = 0;
            // Adjust for check box
            if (subControl == QStyle::SC_GroupBoxCheckBox)
            {
                int indicatorHeight = this->pixelMetric(QStyle::PM_IndicatorHeight, option, widget);
                left = ltr ? totalRect.left() : (totalRect.right() - indicatorWidth);
                int top = totalRect.top() + qMax(0, fontMetrics.height() - indicatorHeight) / 2;
                totalRect.setRect(left, top, indicatorWidth, indicatorHeight);
                // Adjust for label
            }
            else
            {
                left = ltr ? (totalRect.left() + checkBoxSize - 2) : totalRect.left();
                totalRect.setRect(left, totalRect.top(),
                                  totalRect.width() - checkBoxSize, totalRect.height());
            }
        }
        return totalRect;
    }
    default:
        return QProxyStyle::subControlRect(QStyle::CC_GroupBox, option, subControl, widget);
    }
}

void Style::drawPEIndicatorButtonDropDown(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto toolButtonOption(qstyleoption_cast<const QStyleOptionToolButton *>(option));
    RETURN_IF_FALSE(toolButtonOption);

    const QStyle::State &state(option->state);
    bool autoRaise(state & QStyle::State_AutoRaise);
    bool enabled(state & QStyle::State_Enabled);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));
    bool sunken(enabled && (state & QStyle::State_Sunken));

    // do nothing for autoraise buttons
    RETURN_IF_TRUE((autoRaise && !sunken && !mouseOver) || !(toolButtonOption->subControls & QStyle::SC_ToolButtonMenu));

    const QPalette &palette(option->palette);
    const QRect &rect(option->rect);

    auto backgroundColor = Palette::getDefault()->getColor(option->state, Palette::WIDGET);
    auto borderColor = Palette::getDefault()->getColor(option->state, Palette::BORDER);
    auto separatorColor = borderColor;

    QRect frameRect(rect);
    painter->setClipRect(rect);
    frameRect.adjust(-Metrics::Frame_FrameRadius - 1, 0, 0, 0);
    frameRect = QStyle::visualRect(option->direction, option->rect, frameRect);

    // render
    RenderHelper::renderFrame(painter, frameRect, 1, 4, backgroundColor, borderColor);

    // also render separator
    QRect separatorRect(rect.adjusted(0, 2, -2, -2));
    separatorRect.setWidth(1);
    separatorRect = QStyle::visualRect(option->direction, option->rect, separatorRect);
    RenderHelper::renderSeparator(painter, separatorRect, true, separatorColor);
}

void Style::drawPEIndicatorArrowUp(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto d = d_func();

    d->drawPEIndicatorArrow(Arrow_Up, option, painter, widget);
}

void Style::drawPEIndicatorArrowDown(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto d = d_func();

    d->drawPEIndicatorArrow(Arrow_Down, option, painter, widget);
}

void Style::drawPEIndicatorArrowLeft(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto d = d_func();

    d->drawPEIndicatorArrow(Arrow_Left, option, painter, widget);
}

void Style::drawPEIndicatorArrowRight(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto d = d_func();

    d->drawPEIndicatorArrow(Arrow_Right, option, painter, widget);
}

void Style::drawPEIndicatorRadioButton(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto &rect(option->rect);

    bool checked = option->state & QStyle::State_On;
    QString state = "normal";
    if (!(option->state & QStyle::State_Enabled))
    {
        state = "disabled";
    }
    else if ((option->state & QStyle::State_Sunken))
    {
        state = "active";
    }
    else if ((option->state & QStyle::State_MouseOver))
    {
        state = "hover";
    }
    QString iconUrl = QString(":/style/images/radio-%1-%2.svg").arg(checked ? "checked" : "unchecked").arg(state);
    // 参考 UI　设计文稿　6.1 单选框 & 复选框 设置　Hover　状态背景色
    if (option->state & State_MouseOver)
    {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(DEFAULT_PALETTE()->getColor(Palette::MOUSE_OVER, Palette::WIDGET));  // # 454545
        painter->drawEllipse(rect);
        painter->restore();
    }

    // 参考 UI　设计文稿　6.1 单选框 & 复选框 设置　Focus　状态背景色
    if (option->state & State_HasFocus)
    {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(DEFAULT_PALETTE()->getColor(option->state, Palette::WINDOW));  // # 222222
        painter->drawEllipse(rect);
        painter->restore();
    }

    //    if (!(option->state & State_Enabled)) {
    //        painter->save();
    //        painter->setPen(QPen(QColor(145, 145, 145))); // # 919191 //TODO: 取消手动指定颜色
    //        painter->setPen(Qt::NoPen);
    //        painter->setBrush(QColor(145, 145, 145));
    //        painter->drawEllipse(option->rect);
    //        painter->restore();
    //    }

    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    QSvgRenderer renderer(iconUrl);
    renderer.render(painter, rect);
    painter->restore();
}

void Style::drawPEIndicatorCheckBox(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto &rect(option->rect);

    bool checked = option->state & QStyle::State_On;
    QString state = "normal";
    if (!(option->state & QStyle::State_Enabled))
    {
        state = "disabled";
    }
    else if ((option->state & QStyle::State_Sunken))
    {
        state = "active";
    }
    else if ((option->state & QStyle::State_MouseOver))
    {
        state = "hover";
    }
    QString iconUrl = QString(":/style/images/check-%1-%2.svg").arg(checked ? "checked" : "unchecked").arg(state);

    // 参考 UI　设计文稿　6.1 单选框 & 复选框 设置　Hover　状态背景色
    if (option->state & State_MouseOver)
    {
        auto lineColor = Palette::getDefault()->getColor(option->state, Palette::BORDER);

        painter->save();
        painter->setPen(QPen(lineColor, 1));
        painter->setBrush(DEFAULT_PALETTE()->getColor(Palette::MOUSE_OVER, Palette::WIDGET));  // # 454545
        painter->drawRect(rect);
        painter->restore();
    }

    // 参考 UI　设计文稿　6.1 单选框 & 复选框 设置　Focus　状态背景色
    if (option->state & State_HasFocus)
    {
        // 自定义获取焦点时的背景色
        painter->save();
        painter->setPen(QPen());
        painter->setBrush(DEFAULT_PALETTE()->getColor(option->state, Palette::WINDOW));  // # 222222
        painter->drawRect(rect);
        painter->restore();
    }

    //    if (!(option->state & State_Enabled)) {
    //        painter->save();
    //        painter->setPen(QPen(QColor(145, 145, 145))); // # 919191 //TODO: 取消手动指定颜色
    //        painter->setPen(Qt::NoPen);
    //        painter->setBrush(QColor(145, 145, 145));
    //        painter->drawRect(option->rect);
    //        painter->restore();
    //    }

    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    QSvgRenderer renderer(iconUrl);
    renderer.render(painter, rect);
    painter->restore();
}

void Style::drawPEIndicatorBranch(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const QRect &rect(option->rect);
    const QPalette &palette(option->palette);

    const QStyle::State &state(option->state);
    bool reverseLayout(option->direction == Qt::RightToLeft);

    // 绘制扩展的箭头
    int expanderAdjust = 0;
    if (state & QStyle::State_Children)
    {
        // state
        bool expanderOpen(state & QStyle::State_Open);
        bool enabled(state & QStyle::State_Enabled);
        bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));

        // expander rect
        int expanderSize = qMin(rect.width(), rect.height());
        expanderSize = qMin(expanderSize, int(Metrics::ItemView_ArrowSize));
        expanderAdjust = expanderSize / 2 + 1;
        QRect arrowRect = RenderHelper::centerRect(rect, expanderSize, expanderSize);

        // get orientation from option
        ArrowOrientation orientation;
        if (expanderOpen)
        {
            orientation = Arrow_Down;
        }
        else if (reverseLayout)
        {
            orientation = Arrow_Left;
        }
        else
        {
            orientation = Arrow_Right;
        }
        auto arrowColor = Palette::getDefault()->getColor(option->state, Palette::ICON);
        RenderHelper::renderArrow(painter, arrowRect, orientation, arrowColor, QSize(12, 12));
    }

    RETURN_IF_FALSE(RenderHelper::drawTreeBranches());

    QPoint center(rect.center());
    auto lineColor = Palette::getDefault()->getColor(option->state, Palette::BORDER);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(0.5, 0.5);
    painter->setPen(QPen(lineColor, 1));
    if (state & (QStyle::State_Item | QStyle::State_Children | QStyle::State_Sibling))
    {
        QLineF line(QPointF(center.x(), rect.top()), QPointF(center.x(), center.y() - expanderAdjust - 1));
        painter->drawLine(line);
    }

    // 如果存在Item则绘制左/右取决与方向的线
    if (state & QStyle::State_Item)
    {
        const QLineF line = reverseLayout ? QLineF(QPointF(rect.left(), center.y()), QPointF(center.x() - expanderAdjust, center.y())) : QLineF(QPointF(center.x() + expanderAdjust, center.y()), QPointF(rect.right(), center.y()));
        painter->drawLine(line);
    }

    // 如果存在同级节点的话，则绘制上下的线
    if (state & QStyle::State_Sibling)
    {
        QLineF line(QPointF(center.x(), center.y() + expanderAdjust), QPointF(center.x(), rect.bottom()));
        painter->drawLine(line);
    }
    painter->restore();
}

void Style::drawPEIndicatorToolBarSeparator(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const QStyle::State &state = option->state;
    bool enable = (state & QStyle::State_Enabled);
    bool separatorIsVertical(state & QStyle::State_Horizontal);

    auto separatorColor = Palette::getDefault()->getColor(option->state, Palette::BORDER);

    RenderHelper::renderSeparator(painter, option->rect, separatorIsVertical, separatorColor);
}

void Style::drawPEIndicatorToolBarHandle(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const QStyle::State &state = option->state;
    QRect rect(option->rect);
    bool separatorIsVertical(state & QStyle::State_Horizontal);
    bool enabled(state & QStyle::State_Enabled);

    auto separatorColor = Palette::getDefault()->getColor(option->state, Palette::BORDER);

    if (separatorIsVertical)
    {
        rect.setWidth(Metrics::ToolBar_HandleWidth);
        rect = RenderHelper::centerRect(option->rect, rect.size());
        rect.setWidth(3);
        RenderHelper::renderSeparator(painter, rect, separatorIsVertical, separatorColor);

        rect.translate(2, 0);
        RenderHelper::renderSeparator(painter, rect, separatorIsVertical, separatorColor);
    }
    else
    {
        rect.setHeight(Metrics::ToolBar_HandleWidth);
        rect = RenderHelper::centerRect(option->rect, rect.size());
        rect.setHeight(3);
        RenderHelper::renderSeparator(painter, rect, separatorIsVertical, separatorColor);

        rect.translate(0, 2);
        RenderHelper::renderSeparator(painter, rect, separatorIsVertical, separatorColor);
    }
}

void Style::drawControlHeaderSection(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const QRect &rect(option->rect);
    const QPalette &palette(option->palette);
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));

    const auto *headerOption(qstyleoption_cast<const QStyleOptionHeader *>(option));
    RETURN_IF_FALSE(headerOption);

    bool horizontal(headerOption->orientation == Qt::Horizontal);
    bool isFirst(horizontal && (headerOption->position == QStyleOptionHeader::Beginning));
    bool isCorner(widget && widget->inherits("QTableCornerButton"));
    bool reverseLayout(option->direction == Qt::RightToLeft);

    // outline
    painter->setBrush(Qt::NoBrush);
    // TODO: 需要确认该颜色是否合适
    // 该元素主要为表头框线条，颜色为边框颜色合适
    auto outlineColor = Palette::getDefault()->getColor(option->state, Palette::BORDER);
    painter->setPen(outlineColor);

    if (isCorner)
    {
        if (reverseLayout)
        {
            painter->drawPoint(rect.bottomLeft());
        }
        else
        {
            painter->drawPoint(rect.bottomRight());
        }
    }
    else if (horizontal)
    {
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    }
    else
    {
        if (reverseLayout)
        {
            painter->drawLine(rect.topLeft(), rect.bottomLeft());
        }
        else
        {
            painter->drawLine(rect.topRight(), rect.bottomRight());
        }
    }

    // separators
    if (horizontal)
    {
        if (headerOption->section != 0 || isFirst)
        {
            if (reverseLayout)
            {
                painter->drawLine(rect.topLeft(), rect.bottomLeft() - QPoint(0, 1));
            }
            else
            {
                painter->drawLine(rect.topRight(), rect.bottomRight() - QPoint(0, 1));
            }
        }
    }
    else
    {
        if (reverseLayout)
        {
            painter->drawLine(rect.bottomLeft() + QPoint(1, 0), rect.bottomRight());
        }
        else
        {
            painter->drawLine(rect.bottomLeft(), rect.bottomRight() - QPoint(1, 0));
        }
    }
}

void Style::drawControlHeaderLabel(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option))
    {
        QRect rect = header->rect;
        if (!header->icon.isNull())
        {
            QPixmap pixmap = header->icon.pixmap(this->pixelMetric(QStyle::PM_SmallIconSize), (header->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled);
            int pixw = pixmap.width();

            QRect aligned = QStyle::alignedRect(header->direction, QFlag(header->iconAlignment), pixmap.size(), rect);
            QRect inter = aligned.intersected(rect);
            painter->drawPixmap(inter.x(), inter.y(), pixmap, inter.x() - aligned.x(), inter.y() - aligned.y(), inter.width(), inter.height());

            if (header->direction == Qt::LeftToRight)
                rect.setLeft(rect.left() + pixw + 2);
            else
                rect.setRight(rect.right() - pixw - 2);
        }

        QFont fnt = painter->font();
        painter->setFont(fnt);
        QPalette palette(header->palette);
        this->drawItemText(painter, rect, header->textAlignment, palette, (header->state & QStyle::State_Active), header->text, QPalette::Text);
    }
}

void Style::drawControlHeaderEmptyArea(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // use the same background as in drawHeaderPrimitive
    const QRect &rect(option->rect);
    QPalette palette(option->palette);

    bool horizontal(option->state & QStyle::State_Horizontal);
    bool reverseLayout(option->direction == Qt::RightToLeft);

    // fill
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setBrush(palette.color(QPalette::Base));
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);

    // outline
    // TODO: 需要确认该颜色是否合适
    // 该元素用于表格空内容区域分割线， 颜色合适
    auto outlineColor = Palette::getDefault()->getColor(option->state, Palette::BORDER);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(outlineColor);

    if (horizontal)
    {
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    }
    else
    {
        if (reverseLayout)
        {
            painter->drawLine(rect.topLeft(), rect.bottomLeft());
        }
        else
        {
            painter->drawLine(rect.topRight(), rect.bottomRight());
        }
    }
}

void Style::drawControlMenuBarItem(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
    const QStyle::State &state = option->state;
    const bool enabled(option->state & QStyle::State_Enabled);
    const bool mouseOver((option->state & QStyle::State_MouseOver) && enabled);
    const bool sunken((option->state & QStyle::State_Sunken) && enabled);

    PainterSaver painterSaver(painter);

#if 0
    if(mouseOver && sunken)
    {
        auto background = schemeLoader->getColor(widget,option,SchemeLoader::MenuBar_ItemBackground);
        painter->setRenderHints(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRoundedRect(option->rect,4,4);
    }
#endif
    // TODO: menubaritem 和 menubar背景存在差异
    if (menuItemOption)
    {
        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

        if (!this->styleHint(QStyle::SH_UnderlineShortcut, option, widget))
        {
            alignment |= Qt::TextHideMnemonic;
        }

        int iconExtent = this->pixelMetric(QStyle::PM_SmallIconSize);

        QPixmap pix = menuItemOption->icon.pixmap(widget ? widget->window()->windowHandle() : nullptr, QSize(iconExtent, iconExtent), enabled ? (mouseOver ? QIcon::Active : QIcon::Normal) : QIcon::Disabled);

        if (!pix.isNull())
        {
            this->drawItemPixmap(painter, option->rect, alignment, pix);
        }
        else
        {
            QStyleOptionMenuItem itemOption = *menuItemOption;
            QColor bgColor;
            if (mouseOver || sunken)
            {
                // TODO: 需要确认
                bgColor = Palette::getDefault()->getColor(option->state, Palette::WINDOW);
                painter->fillRect(option->rect, bgColor);
            }
            this->drawItemText(painter, itemOption.rect, alignment, itemOption.palette, enabled, itemOption.text, QPalette::ButtonText);
        }
    }
}

void Style::drawControlMenuBarEmptyArea(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // TODO: 需要确认
    auto background = Palette::getDefault()->getColor(option->state, Palette::WINDOW);
    painter->fillRect(option->rect, background);
}

void Style::drawPEPanelMenu(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto panelMenuOption(qstyleoption_cast<const QStyleOptionFrame *>(option));
    RETURN_IF_FALSE(panelMenuOption);

    if (widget && widget->inherits("QComboBoxPrivateContainer"))
    {
        painter->save();
        auto bgColor = Palette::getDefault()->getBaseColors().widgetMain;  // 与combobox菜单项颜色一致
        painter->fillRect(option->rect, bgColor);
        painter->restore();
    }
}

QSize Style::progressBarSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    if (!progressBarOption) return contentSize;

    bool horizontal(progressBarOption->orientation == Qt::Horizontal);

    QSize size(contentSize);

    if (horizontal)
    {
        bool textVisible(progressBarOption->textVisible);

        size.setWidth(qMax(size.width(), int(Metrics::ProgressBar_Thickness)));
        size.setHeight(qMax(size.height(), int(Metrics::ProgressBar_Thickness)));
        if (textVisible) size.setHeight(qMax(size.height(), option->fontMetrics.height()));
    }
    else
    {
        size.setHeight(qMax(size.height(), int(Metrics::ProgressBar_Thickness)));
        size.setWidth(qMax(size.width(), int(Metrics::ProgressBar_Thickness)));
    }

    return size;
}

QRect Style::progressBarElementRect(QStyle::SubElement subElement, const QStyleOption *option, const QWidget *widget) const
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    if (!progressBarOption) return option->rect;

    bool textVisible(progressBarOption->textVisible);
    bool horizontal(progressBarOption->orientation == Qt::Horizontal);
    bool inverted(progressBarOption->invertedAppearance);

    QRect rect(option->rect);
    int frameWidth(this->pixelMetric(QStyle::PM_DefaultFrameWidth, option, widget));

    if (horizontal)
        rect = RenderHelper::insideMargin(rect, frameWidth, 0);
    else
        rect = RenderHelper::insideMargin(rect, 0, frameWidth);

    switch (subElement)
    {
    case QStyle::SE_ProgressBarContents:
    {
        rect = this->subElementRect(QStyle::SE_ProgressBarGroove, option, widget);

        qreal progress(progressBarOption->progress - progressBarOption->minimum);
        int steps(qMax(progressBarOption->maximum - progressBarOption->minimum, 1));
        qreal widthFrac = qMin(qreal(1), progress / steps);
        int indicatorSize(widthFrac * (horizontal ? rect.width() : rect.height()));

        if (horizontal)
        {
            rect = QRect(inverted ? (rect.right() - indicatorSize + 1) : rect.left(), rect.y(), indicatorSize, rect.height());
            rect = QStyle::visualRect(option->direction, rect, rect);
        }
        else
        {
            rect = QRect(rect.x(), inverted ? rect.top() : (rect.bottom() - indicatorSize + 1), rect.width(), indicatorSize);
        }
        break;
    }
    case QStyle::SE_ProgressBarGroove:
    {
        if (textVisible && horizontal)
        {
            QRect textRect(this->subElementRect(QStyle::SE_ProgressBarLabel, option, widget));
            textRect = QStyle::visualRect(option->direction, option->rect, textRect);
            rect.setRight(textRect.left() - Metrics::ProgressBar_ItemSpacing - 1);
            rect = QStyle::visualRect(option->direction, option->rect, rect);
            rect = RenderHelper::centerRect(rect, rect.width(), Metrics::ProgressBar_Thickness);
        }
        else if (horizontal)
        {
            rect = RenderHelper::centerRect(rect, rect.width(), Metrics::ProgressBar_Thickness);
        }
        else
        {
            rect = RenderHelper::centerRect(rect, Metrics::ProgressBar_Thickness, rect.height());
        }
        break;
    }
    case QStyle::SE_ProgressBarLabel:
    {
        if (!textVisible)
            return QRect();

        int textWidth = qMax(
            option->fontMetrics.size(RenderHelper::mnemonicsTextFlags(), progressBarOption->text).width(),
            option->fontMetrics.size(RenderHelper::mnemonicsTextFlags(), QStringLiteral("100%")).width());

        rect = RenderHelper::insideMargin(option->rect, Metrics::Frame_FrameWidth, 0);
        rect.setLeft(rect.right() - textWidth + 1);
        rect = QStyle::visualRect(option->direction, option->rect, rect);
        break;
    }
    default:
        break;
    }

    return rect;
}

void Style::drawControlProgressBar(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    RETURN_IF_FALSE(progressBarOption);

    QStyleOptionProgressBar tempOption(*progressBarOption);

    QRect grooveRect = this->subElementRect(QStyle::SE_ProgressBarGroove, option, widget);
    QRect contentsRect = this->subElementRect(QStyle::SE_ProgressBarContents, option, widget);
    QRect labelRect = this->subElementRect(QStyle::SE_ProgressBarLabel, option, widget);

    tempOption.rect = grooveRect;
    this->drawControl(QStyle::CE_ProgressBarGroove, &tempOption, painter, widget);

    tempOption.rect = contentsRect;
    this->drawControl(QStyle::CE_ProgressBarContents, &tempOption, painter, widget);

    bool textVisible(progressBarOption->textVisible);
    bool busy(progressBarOption->minimum == 0 && progressBarOption->maximum == 0);
    if (textVisible && !busy)
    {
        tempOption.rect = labelRect;
        this->drawControl(QStyle::CE_ProgressBarLabel, &tempOption, painter, widget);
    }

    // TODO: BUSY动画
    //    if (busy) {

    //    }
}

void Style::drawControlProgressBarGroove(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    RETURN_IF_FALSE(progressBarOption);

    auto grooveColor = Palette::getDefault()->getColor(option->state & (QStyle::State_Enabled), Palette::SCROLL);

    RenderHelper::renderFrame(painter, option->rect, 1, 4, grooveColor);
}

void Style::drawControlProgressBarContents(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    RETURN_IF_FALSE(progressBarOption);

    // TODO: 进度条滑动区域左侧颜色，需要确认是否合理
    // 为主要基本色，合理
    auto palettes = Palette::getDefault();
    auto contentsColor = palettes->getBaseColors().widgetMain;

    RenderHelper::renderFrame(painter, option->rect, 1, 4, contentsColor);
}

void Style::drawControlProgressBarLabel(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto progressBarOption(qstyleoption_cast<const QStyleOptionProgressBar *>(option));
    RETURN_IF_FALSE(progressBarOption);

    bool horizontal = progressBarOption->orientation == Qt::Horizontal;
    RETURN_IF_FALSE(horizontal);

    const QRect &rect(option->rect);
    const QPalette &palette(option->palette);

    // store state and direction
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);

    // define text rect
    Qt::Alignment hAlign((progressBarOption->textAlignment == Qt::AlignLeft) ? Qt::AlignHCenter : progressBarOption->textAlignment);
    this->drawItemText(painter, rect, Qt::AlignVCenter | hAlign, palette, enabled, progressBarOption->text, QPalette::WindowText);
}

QRect Style::scrollBarSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    auto d = d_func();

    const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
    RETURN_VAL_IF_FALSE(sliderOption, QRect());

    const QStyle::State &state(option->state);
    bool horizontal(state & QStyle::State_Horizontal);

    switch (subControl)
    {
    case QStyle::SC_ScrollBarAddLine:
    case QStyle::SC_ScrollBarSubLine:
    case QStyle::SC_ScrollBarAddPage:
    case QStyle::SC_ScrollBarSubPage:
        return QRect();
    case QStyle::SC_ScrollBarSlider:
    {
        // handle RTL here to unreflect things if need be
        QRect groove = QStyle::visualRect(option->direction, option->rect, this->subControlRect(QStyle::CC_ScrollBar, option, QStyle::SC_ScrollBarGroove, widget));
        groove.adjust(0, 0, 1, 1);

        if (sliderOption->minimum == sliderOption->maximum)
        {
            return groove;
        }

        // Figure out how much room there is
        int space(horizontal ? groove.width() : groove.height());

        // Calculate the portion of this space that the slider should occupy
        int sliderSize = space * qreal(sliderOption->pageStep) / (sliderOption->maximum - sliderOption->minimum + sliderOption->pageStep);
        sliderSize = qMax(sliderSize, static_cast<int>(Metrics::ScrollBar_MinSliderHeight));
        sliderSize = qMin(sliderSize, space);

        space -= sliderSize;
        if (space <= 0)
        {
            return groove;
        }

        int pos = qRound(qreal(sliderOption->sliderPosition - sliderOption->minimum) / (sliderOption->maximum - sliderOption->minimum) * space);
        if (sliderOption->upsideDown)
        {
            pos = space - pos;
        }

        if (horizontal)
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(groove.left() + pos, groove.top(), sliderSize, groove.height()));
        }
        else
        {
            return QStyle::visualRect(option->direction, option->rect, QRect(groove.left(), groove.top() + pos, groove.width(), sliderSize));
        }
        break;
    }
    case QStyle::SC_ScrollBarGroove:
    {
        QRect topRect = QStyle::visualRect(option->direction, option->rect, d->scrollBarInternalSubControlRect(option, QStyle::SC_ScrollBarSubLine));
        QRect bottomRect = QStyle::visualRect(option->direction, option->rect, d->scrollBarInternalSubControlRect(option, QStyle::SC_ScrollBarAddLine));

        QPoint topLeftCorner;
        QPoint botRightCorner;

        if (horizontal)
        {
            topLeftCorner = QPoint(topRect.right() + 1, topRect.top());
            botRightCorner = QPoint(bottomRect.left() - 1, topRect.bottom());
        }
        else
        {
            topLeftCorner = QPoint(topRect.left(), topRect.bottom() + 1);
            botRightCorner = QPoint(topRect.right(), bottomRect.top() - 1);
        }

        // define rect
        return QStyle::visualRect(option->direction, option->rect, QRect(topLeftCorner, botRightCorner));
    }
    default:
        return QProxyStyle::subControlRect(QStyle::CC_ScrollBar, option, subControl, widget);
    }
}

void Style::drawCCScrollBar(const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    const auto scrollbarOption = qstyleoption_cast<const QStyleOptionSlider *>(option);
    RETURN_IF_FALSE(scrollbarOption);

    bool enabled(option->state & QStyle::State_Enabled);
    bool mouseOver((option->state & QStyle::State_Active) && option->state & QStyle::State_MouseOver);

    if (option->subControls & QStyle::SC_ScrollBarGroove)
    {
        auto grooveRect = this->subControlRect(QStyle::CC_ScrollBar, option, QStyle::SC_ScrollBarGroove, widget);
// 不绘制滑动槽
#if 0
        if( mouseOver )
        {
            QColor grooveColor = KiranPalette::instance()->color(widget,option,KiranPalette::Window,KiranPalette::Background);
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(grooveColor);
            painter->drawRect(grooveRect);
            painter->restore();
        }
#endif
    }

    if (scrollbarOption->subControls & QStyle::SC_ScrollBarSlider)
    {
        QStyleOptionSlider sliderOption = *scrollbarOption;
        sliderOption.state = scrollbarOption->state;
        sliderOption.rect = this->subControlRect(QStyle::CC_ScrollBar, &sliderOption, QStyle::SC_ScrollBarSlider, widget);
        if (sliderOption.rect.isValid())
        {
            this->drawControl(QStyle::CE_ScrollBarSlider, &sliderOption, painter, widget);
        }
    }
}

void Style::drawControlScrollBarSlider(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto sliderOption = qstyleoption_cast<const QStyleOptionSlider *>(option);
    RETURN_IF_FALSE(sliderOption);

    const QStyle::State &state(option->state);
    bool horizontal(state & QStyle::State_Horizontal);
    const QRect &rect(option->rect);
    QRect handleRect;

    bool enabled(state & QStyle::State_Enabled);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));

    const QWidget *parent = nullptr;
    if ((widget && widget->parentWidget()))
    {
        auto scrollBar = qobject_cast<const QScrollBar *>(widget);

        QAbstractScrollArea *scrollArea = nullptr;
        if (!(scrollArea = qobject_cast<QAbstractScrollArea *>(widget->parentWidget())))
        {
            scrollArea = qobject_cast<QAbstractScrollArea *>(widget->parentWidget()->parentWidget());
        }

        if (scrollArea && (scrollBar == scrollArea->verticalScrollBar() || scrollBar == scrollArea->horizontalScrollBar()))
        {
            parent = scrollArea;
        }
    }

    handleRect = rect;
    if (!mouseOver && horizontal)
    {
        handleRect = rect.adjusted(0, 1, 0, -1);
    }
    else if (!mouseOver && !horizontal)
    {
        handleRect = rect.adjusted(1, 0, -1, 0);
    }

    auto sliderColor = Palette::getDefault()->getColor(option->state, Palette::SCROLL);
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPainterPath painterPath = RenderHelper::roundedPath(handleRect, AllCorners, 2);
    painter->fillPath(painterPath, sliderColor);
}

// TODO: 之后整合所有的绘制过程中的是否启用的开关
static const bool sliderDrawTickMarks = true;
enum Side
{
    SideNone = 0x0,
    SideLeft = 0x1,
    SideTop = 0x2,
    SideRight = 0x4,
    SideBottom = 0x8,
    AllSides = SideLeft | SideTop | SideRight | SideBottom
};

QSize Style::sliderSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    // cast option and check
    const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
    if (!sliderOption) return contentSize;

    // store tick position and orientation
    const QSlider::TickPosition &tickPosition(sliderOption->tickPosition);
    bool horizontal(sliderOption->orientation == Qt::Horizontal);
    bool disableTicks = !sliderDrawTickMarks;

    // do nothing if no ticks are requested
    if (tickPosition == QSlider::NoTicks) return contentSize;

    /*
     * Qt adds its own tick length directly inside QSlider.
     * Take it out and replace by ours, if needed
     */
    const int tickLength(disableTicks ? 0 : (Metrics::Slider_TickLength + Metrics::Slider_TickMarginWidth + (Metrics::Slider_GrooveThickness - Metrics::Slider_ControlThickness) / 2));

    int builtInTickLength(2);

    QSize size(contentSize);
    if (horizontal)
    {
        if (tickPosition & QSlider::TicksAbove) size.rheight() += tickLength - builtInTickLength;
        if (tickPosition & QSlider::TicksBelow) size.rheight() += tickLength - builtInTickLength;
    }
    else
    {
        if (tickPosition & QSlider::TicksAbove) size.rwidth() += tickLength - builtInTickLength;
        if (tickPosition & QSlider::TicksBelow) size.rwidth() += tickLength - builtInTickLength;
    }

    return size;
}

void Style::drawCCSlider(const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    // cast option and check
    const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
    RETURN_IF_FALSE(sliderOption);

    // copy rect and palette
    const QRect &rect(option->rect);
    const QPalette &palette(option->palette);

    // copy state
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool windowActive(state & QStyle::State_Active);
    bool mouseOver((state & QStyle::State_Active) && enabled && (state & QStyle::State_MouseOver));
    bool hasFocus(enabled && (state & QStyle::State_HasFocus));
    bool horizontal(sliderOption->orientation == Qt::Horizontal);
    Side tickSide{SideNone};
    if (horizontal && sliderOption->tickPosition == QSlider::TicksAbove) tickSide = (Side)((int)tickSide | (int)SideTop);
    if (horizontal && sliderOption->tickPosition == QSlider::TicksBelow) tickSide = (Side)((int)tickSide | (int)SideBottom);
    if (!horizontal && sliderOption->tickPosition == QSlider::TicksLeft) tickSide = (Side)((int)tickSide | (int)SideLeft);
    if (!horizontal && sliderOption->tickPosition == QSlider::TicksRight) tickSide = (Side)((int)tickSide | (int)SideRight);

    auto palettes = Palette::getDefault();
    // 背景只考虑启用和禁用状态，悬浮和点击不用变颜色
    auto grooveColor = Palette::getDefault()->getColor(option->state & (QStyle::State_Enabled), Palette::SCROLL);
    auto contentColor = Palette::getDefault()->getBaseColors().widgetSelection;

    // tickmarks
    if (sliderDrawTickMarks && (sliderOption->subControls & QStyle::SC_SliderTickmarks))
    {
        bool upsideDown(sliderOption->upsideDown);
        int tickPosition(sliderOption->tickPosition);
        int available(this->pixelMetric(QStyle::PM_SliderSpaceAvailable, option, widget));
        int interval = sliderOption->tickInterval;
        if (interval < 1) interval = sliderOption->pageStep;
        if (interval >= 1)
        {
            int fudge(this->pixelMetric(QStyle::PM_SliderLength, option, widget) / 2);
            int current(sliderOption->minimum);

            // store tick lines
            QRect grooveRect(this->subControlRect(QStyle::CC_Slider, sliderOption, QStyle::SC_SliderGroove, widget));
            QList<QLine> tickLines;
            if (horizontal)
            {
                if (tickPosition & QSlider::TicksAbove) tickLines.append(QLine(rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength));
                if (tickPosition & QSlider::TicksBelow) tickLines.append(QLine(rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength));
            }
            else
            {
                if (tickPosition & QSlider::TicksAbove) tickLines.append(QLine(grooveRect.left() - Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.left() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength, rect.top()));
                if (tickPosition & QSlider::TicksBelow) tickLines.append(QLine(grooveRect.right() + Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.right() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength, rect.top()));
            }

            while (current <= sliderOption->maximum)
            {
                // adjust color
                QColor color((enabled && current <= sliderOption->sliderPosition) ? contentColor : grooveColor);
                painter->setPen(color);

                // calculate positions and draw lines
                int position(QStyle::sliderPositionFromValue(sliderOption->minimum, sliderOption->maximum, current, available) + fudge);
                foreach (const QLine &tickLine, tickLines)
                {
                    if (horizontal)
                        painter->drawLine(tickLine.translated(upsideDown ? (rect.width() - position) : position, 0));
                    else
                        painter->drawLine(tickLine.translated(0, upsideDown ? (rect.height() - position) : position));
                }

                // go to next position
                current += interval;
            }
        }
    }

    // groove
    if (sliderOption->subControls & QStyle::SC_SliderGroove)
    {
        if (hasFocus)
        {
            QRect focusRect = this->subElementRect(QStyle::SE_SliderFocusRect, option, widget);

            QStyleOptionFocusRect fropt;
            fropt.QStyleOption::operator=(*option);
            fropt.rect = focusRect;
            this->drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, painter, widget);
        }

        // retrieve groove rect
        QRect grooveRect(this->subControlRect(QStyle::CC_Slider, sliderOption, QStyle::SC_SliderGroove, widget));

        if (!enabled)
        {
            RenderHelper::renderFrame(painter, grooveRect, 1, 0, grooveColor);
        }
        else
        {
            bool upsideDown(sliderOption->upsideDown);

            // handle rect
            QRect handleRect(this->subControlRect(QStyle::CC_Slider, sliderOption, QStyle::SC_SliderHandle, widget));

            if (sliderOption->orientation == Qt::Horizontal)
            {
                QRect leftRect(grooveRect);
                QRect rightRect(grooveRect);
                leftRect.setRight(handleRect.right() - Metrics::Slider_ControlThickness / 2);
                rightRect.setLeft(handleRect.left() + Metrics::Slider_ControlThickness / 2);

                if (upsideDown)
                {
                    RenderHelper::renderFrame(painter, leftRect, 1, 0, grooveColor);
                    RenderHelper::renderFrame(painter, rightRect, 1, 0, contentColor);
                }
                else
                {
                    RenderHelper::renderFrame(painter, leftRect, 1, 0, contentColor);
                    RenderHelper::renderFrame(painter, rightRect, 1, 0, grooveColor);
                }
            }
            else
            {
                QRect topRect(grooveRect);
                topRect.setBottom(handleRect.bottom() - Metrics::Slider_ControlThickness / 2);
                QRect bottomRect(grooveRect);
                bottomRect.setTop(handleRect.top() + Metrics::Slider_ControlThickness / 2);

                if (upsideDown)
                {
                    RenderHelper::renderFrame(painter, topRect, 1, 0, grooveColor);
                    RenderHelper::renderFrame(painter, bottomRect, 1, 0, contentColor);
                }
                else
                {
                    RenderHelper::renderFrame(painter, topRect, 1, 0, contentColor);
                    RenderHelper::renderFrame(painter, bottomRect, 1, 0, grooveColor);
                }
            }
        }
    }

    // handle
    if (sliderOption->subControls & QStyle::SC_SliderHandle)
    {
        // get rect and center
        QRectF handleRect(this->subControlRect(QStyle::CC_Slider, sliderOption, QStyle::SC_SliderHandle, widget));

        // handle state
        bool handleActive(sliderOption->activeSubControls & QStyle::SC_SliderHandle);
        bool sunken(state & (QStyle::State_On | QStyle::State_Sunken));

        auto handleBorder = QColor(255, 255, 255);
        auto handleBackground = palettes->getColor(option->state, Palette::SCROLL);

        // draw handle
        painter->setRenderHint(QPainter::Antialiasing);
        handleRect.adjust(1, 1, -1, -1);

        if (handleBorder.isValid())
        {
            QPen pen;
            pen.setCapStyle(Qt::RoundCap);
            pen.setColor(handleBorder);
            pen.setWidth(2);
            painter->setPen(pen);
            handleRect = RenderHelper::strokedRect(handleRect, 2);
        }
        else
        {
            painter->setPen(Qt::NoPen);
        }
        painter->setBrush(handleBackground);
        painter->drawRoundedRect(handleRect, handleRect.width() / 2, handleRect.width());
    }
}

QRect Style::sliderElementRect(QStyle::SubElement subElement, const QStyleOption *option, const QWidget *widget) const
{
    QRect rect(option->rect);

    switch (subElement)
    {
    case QStyle::SE_SliderFocusRect:
    {
        const auto sliderOption(qstyleoption_cast<const QStyleOptionSlider *>(option));
        QRect r(option->rect);
        if (sliderOption->orientation == Qt::Vertical)
        {
            int thickness = Metrics::Slider_GrooveThickness + 8;
            return QRect(r.center().x() - thickness / 2, r.top() + 1, thickness + 1, r.height() - 1);
        }
        else
        {
            int thickness = Metrics::Slider_GrooveThickness + 6;
            return QRect(r.left() + 1, r.center().y() - thickness / 2, r.width() - 1, thickness + 1);
        }
    }
    default:
        break;
    }

    return rect;
}

QRect Style::sliderSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    // cast option and check
    auto sliderOption = qstyleoption_cast<const QStyleOptionSlider *>(option);
    RETURN_VAL_IF_FALSE(sliderOption, QRect());

    switch (subControl)
    {
    case QStyle::SC_SliderGroove:
    {
        // direction
        bool horizontal(sliderOption->orientation == Qt::Horizontal);

        // get base class rect
        // NOTE:该处直接调用父类的方法
        QRect grooveRect = qobject_cast<const QProxyStyle *>(this)->QProxyStyle::subControlRect(QStyle::CC_Slider, option, subControl, widget);
        grooveRect = RenderHelper::insideMargin(grooveRect, this->pixelMetric(QStyle::PM_DefaultFrameWidth, option, widget));

        // centering
        if (horizontal)
            grooveRect = RenderHelper::centerRect(grooveRect, grooveRect.width(), Metrics::Slider_GrooveThickness);
        else
            grooveRect = RenderHelper::centerRect(grooveRect, Metrics::Slider_GrooveThickness, grooveRect.height());

        return grooveRect;
    }
    default:
        return QProxyStyle::subControlRect(QStyle::CC_Slider, option, subControl, widget);
    }
}

QSize Style::spinBoxSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    const auto spinBoxOption(qstyleoption_cast<const QStyleOptionSpinBox *>(option));
    if (!spinBoxOption) return contentSize;

    bool flat(!spinBoxOption->frame);

    // copy size
    QSize size(contentSize);

    // add editor margins
    int frameWidth(this->pixelMetric(QStyle::PM_SpinBoxFrameWidth, option, widget));
    if (!flat) size = RenderHelper::expandSize(size, frameWidth);

    // add button width and spacing
    size.rwidth() += 2 * size.height() - 1;

    return size;
}

QRect Style::spinBoxSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    const auto spinBoxOption(qstyleoption_cast<const QStyleOptionSpinBox *>(option));
    RETURN_VAL_IF_FALSE(spinBoxOption, QRect());

    bool flat(!spinBoxOption->frame);

    QRect rect(option->rect);
    switch (subControl)
    {
    case QStyle::SC_SpinBoxFrame:
        return flat ? QRect() : rect;
        break;
    case QStyle::SC_SpinBoxUp:
        if (rect.width() > 2 * rect.height() + 24)
        {
            return QRect(rect.right() - rect.height(), rect.top(), rect.height(), rect.height() - 1);
        }
        else
        {
            return QRect(rect.right() - 0.6 * rect.height() + 1, rect.top(), rect.height() * 0.6, rect.height() / 2 + 3);
        }
        break;
    case QStyle::SC_SpinBoxDown:
        if (rect.width() > 2 * rect.height() + 24)
        {
            return QRect(rect.right() - 2 * rect.height() + 1, rect.top(), rect.height(), rect.height() - 1);
        }
        else
        {
            return QRect(rect.right() - 0.6 * rect.height() + 1, rect.top() + rect.height() / 2 - 2, rect.height() * 0.6, rect.height() / 2 + 2);
        }
        break;
    case QStyle::SC_SpinBoxEditField:
    {
        QRect rect2;
        int frameWidth(this->pixelMetric(QStyle::PM_SpinBoxFrameWidth, option, widget));
        if (rect.width() > 2 * rect.height() + 24)
        {
            rect2 = QRect(rect.left(), rect.top(),
                          rect.width() - 2 * rect.height() - frameWidth,
                          rect.height());
        }
        else
        {
            rect2 = QRect(
                rect.left(), rect.top(),
                rect.width() - 0.6 * rect.height() - frameWidth,
                rect.height());
        }
        // remove right side line editor margins
        if (!flat && rect2.height() >= option->fontMetrics.height() + 2 * frameWidth)
        {
            rect2.adjust(frameWidth, frameWidth, 0, -frameWidth);
        }

        return QStyle::visualRect(option->direction, option->rect, rect2);
    }
    default:
        return QProxyStyle::subControlRect(QStyle::CC_SpinBox, option, subControl, widget);
    }
}

void Style::drawCCSpinBox(const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    auto d = d_func();

    const auto spinBoxOption(qstyleoption_cast<const QStyleOptionSpinBox *>(option));
    RETURN_IF_FALSE(spinBoxOption);

    const QPalette &palette(option->palette);
    const QRect &rect(option->rect);

    if (option->subControls & QStyle::SC_SpinBoxFrame)
    {
        // detect flat spinboxes
        bool flat(!spinBoxOption->frame);
        flat |= (rect.height() < 2 * Metrics::Frame_FrameWidth + Metrics::SpinBox_ArrowButtonWidth);
        if (flat)
        {
            const auto &background = Palette::getDefault()->getColor(option->state, Palette::WIDGET);

            painter->setBrush(background);
            painter->setPen(Qt::NoPen);
            painter->drawRect(rect);
        }
        else
        {
            this->drawPrimitive(QStyle::PE_FrameLineEdit, option, painter, widget);
        }
    }

    // 裁剪掉按钮露出圆角边框的区域
    QPainterPath catPath = RenderHelper::roundedPath(RenderHelper::insideMargin(option->rect, 1), AllCorners, 4);
    PainterSaver painterSaver(painter);
    painter->setClipPath(catPath);
    if (option->subControls & QStyle::SC_SpinBoxUp)
    {
        d->renderSpinBoxArrow(QStyle::SC_SpinBoxUp, spinBoxOption, painter, widget);
    }

    if (option->subControls & QStyle::SC_SpinBoxDown)
    {
        d->renderSpinBoxArrow(QStyle::SC_SpinBoxDown, spinBoxOption, painter, widget);
    }
}

QSize Style::tabBarTabSizeFromContents(const QStyleOption *option, const QSize &contentSize, const QWidget *widget) const
{
    const auto tabOption(qstyleoption_cast<const QStyleOptionTab *>(option));
    const bool hasText(tabOption && !tabOption->text.isEmpty());
    const bool hasIcon(tabOption && !tabOption->icon.isNull());
    const bool hasLeftButton(tabOption && !tabOption->leftButtonSize.isEmpty());
    const bool hasRightButton(tabOption && !tabOption->leftButtonSize.isEmpty());

    // calculate width increment for horizontal tabs
    int widthIncrement = 10;
    if (hasIcon && !(hasText || hasLeftButton || hasRightButton)) widthIncrement -= 4;
    if (hasText && hasIcon) widthIncrement += Metrics::TabBar_TabItemSpacing;
    if (hasLeftButton && (hasText || hasIcon)) widthIncrement += Metrics::TabBar_TabItemSpacing;
    if (hasRightButton && (hasText || hasIcon || hasLeftButton)) widthIncrement += Metrics::TabBar_TabItemSpacing;

    // add margins
    QSize size(contentSize);

    // compare to minimum size
    const bool verticalTabs(tabOption && RenderHelper::isVerticalTab(tabOption->shape));
    if (verticalTabs)
    {
        size.rheight() += widthIncrement;
        if (hasIcon && !hasText)
            size = size.expandedTo(QSize(Metrics::TabBar_TabMinHeight, 0));
        else
            size = size.expandedTo(QSize(Metrics::TabBar_TabMinHeight, Metrics::TabBar_TabMinWidth));
    }
    else
    {
        size.rwidth() += widthIncrement;
        if (hasIcon && !hasText)
            size = size.expandedTo(QSize(0, Metrics::TabBar_TabMinHeight));
        else
            size = size.expandedTo(QSize(Metrics::TabBar_TabMinWidth, Metrics::TabBar_TabMinHeight));
    }

    return size;
}

void Style::drawPEFrameTabWidget(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    auto palettes = Palette::getDefault();
    auto background = Palette::getDefault()->getColor(option->state, Palette::WINDOW);
    auto border = Palette::getDefault()->getColor(option->state, Palette::BORDER);
    RenderHelper::renderFrame(painter, option->rect, 1, 0, background, border);
}

void Style::drawControlTabBarTabShape(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto tabOption(qstyleoption_cast<const QStyleOptionTab *>(option));
    RETURN_IF_FALSE(tabOption);

    const QPalette &palette(option->palette);
    const QStyle::State &state(option->state);
    bool enabled(state & QStyle::State_Enabled);
    bool selected(state & QStyle::State_Selected);
    bool mouseOver((state & QStyle::State_Active) && !selected && (state & QStyle::State_MouseOver) && enabled);

    // check if tab is being dragged
    bool isDragged(widget && selected && painter->device() != widget);
    // bool isLocked(widget && _tabBarData->isLocked(widget));

    const QStyleOptionTab::TabPosition &position = tabOption->position;
    const bool isSingle(position == QStyleOptionTab::OnlyOneTab);
    const bool isQtQuickControl(RenderHelper::isQtQuickControl(option, widget));
    bool isFirst(isSingle || position == QStyleOptionTab::Beginning);
    bool isLast(isSingle || position == QStyleOptionTab::End);
    bool isLeftOfSelected(/*!isLocked &&*/ tabOption->selectedPosition == QStyleOptionTab::NextIsSelected);
    bool isRightOfSelected(/*!isLocked &&*/ tabOption->selectedPosition == QStyleOptionTab::PreviousIsSelected);

    // true if widget is aligned to the frame
    // need to check for 'isRightOfSelected' because for some reason the isFirst flag is set when active tab is being moved
    isFirst &= !isRightOfSelected;
    isLast &= !isLeftOfSelected;

    // 若是反向布局交换状态，改变与方向相关属性
    const bool reverseLayout(option->direction == Qt::RightToLeft);
    const bool verticalTabs(RenderHelper::isVerticalTab(tabOption->shape));
    if (reverseLayout && !verticalTabs)
    {
        qSwap(isFirst, isLast);
        qSwap(isLeftOfSelected, isRightOfSelected);
    }

    // 根据TabBar方向调整选项卡圆角的位置
    Corners corners;
    switch (tabOption->shape)
    {
    case QTabBar::RoundedNorth:
    case QTabBar::TriangularNorth:
        corners = CornersTop;
        break;
    case QTabBar::RoundedSouth:
    case QTabBar::TriangularSouth:
        corners = CornersBottom;
        break;
    case QTabBar::RoundedWest:
    case QTabBar::TriangularWest:
        corners = CornersLeft;
        break;
    case QTabBar::RoundedEast:
    case QTabBar::TriangularEast:
        corners = CornersRight;
        break;
    default:
        break;
    }

    auto background = Palette::getDefault()->getColor(option->state, Palette::WIDGET);
    RenderHelper::renderTabBarTab(painter, option->rect, corners, 4, background);
}

QRect Style::tabBarSubElementRect(QStyle::SubElement subElement, const QStyleOption *option, const QWidget *widget) const
{
    const auto *tabOption(qstyleoption_cast<const QStyleOptionTab *>(option));

    if (!widget || !widget->inherits("QTabBar"))
    {
        return QRect();
    }

    const auto tabBar = qobject_cast<const QTabBar *>(widget);
    const QRect &rect = option->rect;

    switch (subElement)
    {
    case QStyle::SE_TabBarTearIndicator:
    case QStyle::SE_TabBarTearIndicatorRight:
        return QRect();

    // QTabBar::setTabButton所设置位于单个Tab左右的控件
    case QStyle::SE_TabBarTabLeftButton:
    case QStyle::SE_TabBarTabRightButton:
    {
        if (!tabOption)
        {
            return QRect();
        }

        bool isLeft = (subElement == QStyle::SE_TabBarTabLeftButton);

        QSize size(isLeft ? tabOption->leftButtonSize : tabOption->rightButtonSize);
        if (size.isEmpty())
        {
            return QRect();
        }

        QRect buttonRect(QPoint(0, 0), size);

        switch (tabOption->shape)
        {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:

        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            if (isLeft)
            {
                buttonRect.moveLeft(rect.left() + Metrics::TabBar_TabMarginWidth);
            }
            else
            {
                buttonRect.moveRight(rect.right() - Metrics::TabBar_TabMarginWidth);
            }
            buttonRect.moveTop((rect.height() - buttonRect.height()) / 2);
            buttonRect = QStyle::visualRect(option->direction, option->rect, buttonRect);
            break;

        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            if (isLeft)
            {
                buttonRect.moveBottom(rect.bottom() - Metrics::TabBar_TabMarginWidth);
            }
            else
            {
                buttonRect.moveTop(rect.top() + Metrics::TabBar_TabMarginWidth);
            }
            buttonRect.moveLeft((rect.width() - buttonRect.width()) / 2);
            break;

        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            if (isLeft)
            {
                buttonRect.moveTop(rect.top() + Metrics::TabBar_TabMarginWidth);
            }
            else
            {
                buttonRect.moveBottom(rect.bottom() - Metrics::TabBar_TabMarginWidth);
            }
            buttonRect.moveLeft((rect.width() - buttonRect.width()) / 2);
            break;

        default:
            break;
        }

        return buttonRect;
    }

    case QStyle::SE_TabBarTabText:
    {
        auto d = d_func();
        QRect textRect;
        QRect iconRect;
        d->tabLayout(tabOption, widget, &textRect, &iconRect);
        return textRect;
    }

    // 当QTabBar过多时，QTabBar左右滑动的按钮
    case QStyle::SE_TabBarScrollLeftButton:
    case QStyle::SE_TabBarScrollRightButton:
    {
        bool isLeft = (subElement == QStyle::SE_TabBarScrollLeftButton);
        const int buttonWidth = this->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, nullptr, widget);
        QRect scrollButtonRect;

        if (RenderHelper::isVerticalTab(tabBar->shape()))
        {
            scrollButtonRect = QRect(0, isLeft ? 0 : (rect.height() - buttonWidth), rect.width(), buttonWidth);
        }
        else
        {
            scrollButtonRect = QRect(isLeft ? 0 : (rect.width() - buttonWidth), 0, buttonWidth, rect.height());
            QStyle::visualRect(option->direction, option->rect, scrollButtonRect);
        }
        return scrollButtonRect;
    }
    default:
        break;
    }

    return QRect();
}

QPainterPath roundedItemPath(const QStyleOptionViewItem *o, const QWidget *w)
{
    int radius = 4;
    int backgroundMarginLeft = 0;
    int backgroundMarginRight = 0;

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(o->rect.adjusted(backgroundMarginLeft, 0, -backgroundMarginRight, 0), radius, radius);
    return path;
}

void Style::drawPanelItemViewRow(const QStyleOption *pOption, QPainter *pPainter, const QWidget *pWidget) const
{
    if (const auto vopt = qstyleoption_cast<const QStyleOptionViewItem *>(pOption))
    {
        pPainter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path = roundedItemPath(vopt, pWidget);
        QPalette::ColorGroup colorGroup = (pWidget ? pWidget->isEnabled() : (vopt->state & QStyle::State_Enabled))
                                              ? QPalette::Normal
                                              : QPalette::Disabled;
        if (colorGroup == QPalette::Normal && !(vopt->state & QStyle::State_Active))
            colorGroup = QPalette::Inactive;

        if ((vopt->state & QStyle::State_Selected) && proxy()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, pOption, pWidget))
        {
            pPainter->fillRect(vopt->rect, vopt->palette.brush(colorGroup, QPalette::Text));  // 选中高亮
        }
        else if (vopt->features & QStyleOptionViewItem::Alternate)
        {
            pPainter->fillPath(path, vopt->palette.brush(colorGroup, QPalette::AlternateBase));
        }
    }
}

void Style::drawPanelItemViewItem(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (const auto vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option))
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path = roundedItemPath(vopt, widget);
        QPalette::ColorGroup cg = (widget ? widget->isEnabled() : (vopt->state & QStyle::State_Enabled))
                                      ? QPalette::Normal
                                      : QPalette::Disabled;
        if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
            cg = QPalette::Inactive;
        if (vopt->showDecorationSelected && (vopt->state & QStyle::State_Selected))
        {
            painter->fillPath(path, vopt->palette.brush(cg, QPalette::Highlight));
        }
        else
        {
            if (vopt->backgroundBrush.style() != Qt::NoBrush)
            {
                QPointF brushOriginOld = painter->brushOrigin();
                painter->setBrushOrigin(vopt->rect.topLeft());
                painter->fillPath(path, vopt->backgroundBrush);
                painter->setBrushOrigin(brushOriginOld);
            }

            if (vopt->state & QStyle::State_Selected)
            {
                QRect textRect = subElementRect(QStyle::SE_ItemViewItemText, option, widget);
                painter->fillRect(textRect, vopt->palette.brush(cg, QPalette::Text));
            }
        }
    }
}
void Style::drawTextEditor(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto itemOption = qstyleoption_cast<const QStyleOptionViewItem *>(option);
    if (itemOption && itemOption->state & QStyle::State_Selected)
    {
        QStyleOptionViewItem newOption(*itemOption);
        //        newOption.palette.setColor(QPalette::Highlight, Qt::transparent);
        QProxyStyle::drawControl(element, &newOption, painter, widget);
        return;
    }

    QProxyStyle::drawControl(element, option, painter, widget);
}

}  // namespace Theme
}  // namespace Kiran
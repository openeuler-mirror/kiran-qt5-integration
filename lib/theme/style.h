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

#pragma once

#include <QAbstractScrollArea>
#include <QProxyStyle>

class QAbstractScrollArea;

namespace Kiran
{
namespace Theme
{
class StylePrivate;
class Palette;

class Style : public QProxyStyle
{
public:
    explicit Style();
    ~Style() override;

    int styleHint(StyleHint hint, const QStyleOption* option = nullptr, const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const override;

    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const override;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;

    QRect subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const override;
    QRect subControlRect(ComplexControl complexControl,
                         const QStyleOptionComplex* option,
                         SubControl subControl,
                         const QWidget* widget) const override;
    QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const override;

    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption* option, const QWidget* widget) const override;
    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption* option, const QWidget* widget) const override;

    void polishScrollArea(QAbstractScrollArea* scrollArea);
    void polish(QWidget* widget) override;
    void polish(QApplication* application) override;
    void polish(QPalette& palette) override;

private:
    // 绘制Frame背景和边框
    void drawPEFrame(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    // 绘制聚焦时候的边框
    void drawPEFrameFocusRect(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    // 绘制QGroupBox控件的边框和背景
    void drawPEFrameGroupBox(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawControlShapedFrame(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    QSize defaultSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;

    // button
    QSize pushButtonSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;
    QSize toolButtonSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;
    QRect toolButtonSubControlRect(const QStyleOptionComplex* option, QStyle::SubControl subControl, const QWidget* widget) const;
    void drawPEPushButton(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    // 绘制列表条目
    void drawItemViewItem(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    // 文本选中颜色
    void drawTextEditor(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEPanelButtonCommand(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEPanelButtonTool(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawControlToolButtonLabel(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawCCToolButton(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const;

    // combobox
    QSize comboBoxSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;
    QRect comboBoxSubControlRect(const QStyleOptionComplex* option, QStyle::SubControl subControl, const QWidget* widget) const;
    void drawCCComboBox(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const;
    void drawControlComboBoxLabel(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

    // edit
    void drawPEFrameLineEdit(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

    // groupbox
    QSize groupBoxSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;
    QRect groupBoxSubControlRect(const QStyleOptionComplex* option, QStyle::SubControl subControl, const QWidget* widget) const;

    // indicator
    void drawPEIndicatorButtonDropDown(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEIndicatorArrowUp(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEIndicatorArrowDown(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEIndicatorArrowLeft(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEIndicatorArrowRight(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    // 绘制单选框按钮
    void drawPEIndicatorRadioButton(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    // 绘制复选框按钮
    void drawPEIndicatorCheckBox(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEIndicatorBranch(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEIndicatorToolBarSeparator(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawPEIndicatorToolBarHandle(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

    //item view
    void drawControlHeaderSection(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawControlHeaderLabel(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawControlHeaderEmptyArea(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

    // menu
    void drawControlMenuBarItem(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawControlMenuBarEmptyArea(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

    // progressbar
    QSize progressBarSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;
    QRect progressBarElementRect(QStyle::SubElement subElement, const QStyleOption* option, const QWidget* widget) const;
    void drawControlProgressBar(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawControlProgressBarGroove(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawControlProgressBarContents(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    void drawControlProgressBarLabel(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

    // scrollbar
    QRect scrollBarSubControlRect(const QStyleOptionComplex* option, QStyle::SubControl subControl, const QWidget* widget) const;
    void drawCCScrollBar(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const;
    // 绘制滑动条可滑动区域
    void drawControlScrollBarSlider(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

    // slider
    QSize sliderSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;
    void drawCCSlider(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const;
    QRect sliderElementRect(QStyle::SubElement subElement, const QStyleOption* option, const QWidget* widget) const;
    QRect sliderSubControlRect(const QStyleOptionComplex* option, QStyle::SubControl subControl, const QWidget* widget) const;

    // spinbox
    QSize spinBoxSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;
    QRect spinBoxSubControlRect(const QStyleOptionComplex* option, QStyle::SubControl subControl, const QWidget* widget) const;
    void drawCCSpinBox(const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const;

    // tabbar FIXME:需要和sizeFromContents获取的大小对应
    QSize tabBarTabSizeFromContents(const QStyleOption* option, const QSize& contentSize, const QWidget* widget) const;
    QRect tabBarSubElementRect(QStyle::SubElement subElement, const QStyleOption* option, const QWidget* widget) const;
    // tab选项卡下边内容控件的背景绘制
    void drawPEFrameTabWidget(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    // tab选项卡形状和背景绘制
    void drawControlTabBarTabShape(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

    // ListItem 行样式 TODO: 弃用
    void drawPanelItemViewRow(const QStyleOption* pOption, QPainter* pPainter, const QWidget* pWidget) const;
    void drawPanelItemViewItem(const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

private:
    StylePrivate* d_ptr;
    Q_DECLARE_PRIVATE(Style);
};

}  // namespace Theme
}  // namespace Kiran
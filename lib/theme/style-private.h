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

#include <QPushButton>
#include "lib/theme/define.h"
#include "lib/theme/style.h"

class QStyleOptionTab;
class QStyleOptionSpinBox;

namespace Kiran
{
namespace Theme
{
class Palette;

class StylePrivate : QObject
{
    Q_OBJECT

    enum ButtonType
    {
        BUTTON_Normal,  /** < 普通按钮 */
        BUTTON_Default, /** < 默认按钮，突出显示 */
        BUTTON_Warning  /** < 警告按钮，突出警示显示 */
    };

    enum ScrollBarButtonType
    {
        NoButton,
        SingleButton,
        DoubleButton
    } _subLineButtons = NoButton,
      _addLineButtons = NoButton;

public:
    explicit StylePrivate(Style* ptr);

    static ButtonType getButtonType(const QPushButton *btn);
    void tabLayout(const QStyleOptionTab* opt, const QWidget* widget, QRect* textRect, QRect* iconRect) const;
    void renderSpinBoxArrow(const QStyle::SubControl& subControl, const QStyleOptionSpinBox* option, QPainter* painter, const QWidget* widget) const;
    int scrollBarButtonHeight(ScrollBarButtonType type) const;
    QRect scrollBarInternalSubControlRect(const QStyleOptionComplex* option, QStyle::SubControl subControl) const;
    void drawPEIndicatorArrow(ArrowOrientation orientation, const QStyleOption* option, QPainter* painter, const QWidget* widget) const;

private:
    Style* q_ptr;
    Q_DECLARE_PUBLIC(Style);
};
}  // namespace Theme
}  // namespace Kiran
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
#ifndef KIRAN_QT5_PLATFORMTHEME_TEST_QT_WIDGET_FACTOR_H_
#define KIRAN_QT5_PLATFORMTHEME_TEST_QT_WIDGET_FACTOR_H_

#include <QWidget>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class QtWidgetFactor;
}
QT_END_NAMESPACE

class QtWidgetFactor : public QWidget
{
    Q_OBJECT

public:
    explicit QtWidgetFactor(QWidget *parent = nullptr);
    ~QtWidgetFactor() override;

    void initStyleSwitch();
    void initDisableSwitch();
    void initMenu();
    void initToolBar();
    void initTabBar();
    /*
     * @description: 初始化主题ICON，测试图标引擎是否生效
     **/
    void initThemedIcon();

private:
    Ui::QtWidgetFactor *ui;
};

#endif  //KIRAN_QT5_PLATFORMTHEME_TEST_QT_WIDGET_FACTOR_H_

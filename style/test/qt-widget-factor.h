//
// Created by lxh on 2021/12/3.
//

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

private:
    Ui::QtWidgetFactor *ui;
};

#endif  //KIRAN_QT5_PLATFORMTHEME_TEST_QT_WIDGET_FACTOR_H_

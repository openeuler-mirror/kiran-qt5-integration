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

#include "examples/widget-factory.h"

#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QMetaEnum>
#include <QRadioButton>
#include <QStyleFactory>
#include <QToolBar>
#include "colors-card.h"
#include "examples/ui_widget-factory.h"
#include "lib/theme/palette.h"
#include "lib/theme/style.h"

#define KIRAN_STYLE_PROPERTY_BUTTON_TYPE "_kiran_button_type"

using namespace Kiran;
QtWidgetFactor::QtWidgetFactor(QWidget* parent) : QWidget(parent), ui(new Ui::QtWidgetFactor)
{
    ui->setupUi(this);

    initStyleSwitch();
    initDisableSwitch();
    initMenu();
    initToolBar();
    initTabBar();
    initThemedIcon();
}

QtWidgetFactor::~QtWidgetFactor()
{
    delete ui;
}

void QtWidgetFactor::initStyleSwitch()
{
    auto keys = QStyleFactory::keys();
    for (auto key : keys)
    {
        ui->comboBox_switchStyle->addItem(key);
    }

    connect(ui->comboBox_switchStyle, &QComboBox::currentTextChanged, [this](const QString& text)
            {
                qInfo() << "set style:" << text;
                qApp->setStyle(QStyleFactory::create(text));
            });

    QString curStyleName = qApp->style()->objectName();
    int curIdx = ui->comboBox_switchStyle->findText(curStyleName);
    ui->comboBox_switchStyle->setCurrentIndex(curIdx);
}

void QtWidgetFactor::initDisableSwitch()
{
    connect(ui->checkBox_disable, &QCheckBox::stateChanged, [this](int state)
            {
                if (state == 2)
                {
                    ui->tabWidget->setDisabled(true);
                }
                else
                {
                    ui->tabWidget->setEnabled(true);
                }
            });
}

void QtWidgetFactor::initMenu()
{
    QMenu* menu = new QMenu;
    menu->addAction("Action1");
    menu->addAction("Action2");
    ui->toolButton_menu->setMenu(menu);
    ui->toolButton_menu_2->setMenu(menu);
}
void QtWidgetFactor::initToolBar()
{
    //toolbar
    auto toolbar = new QToolBar();
    ui->layout_toolbar->addWidget(toolbar);

    QStringList apps = {"firefox", "eom", "gnote", "gpick", "mate-desktop", "fcitx"};
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << "/usr/share/icons/We10X/apps/scalable/");
    qInfo() << QIcon::themeSearchPaths();
    for (const QString& app : apps)
    {
        auto toolButton = new QToolButton;
        toolButton->setIcon(QIcon::fromTheme(app));
        toolbar->addWidget(toolButton);
    }
    // button
    ui->pushButton_10->setProperty(KIRAN_STYLE_PROPERTY_BUTTON_TYPE, 1);
    ui->pushButton_11->setProperty(KIRAN_STYLE_PROPERTY_BUTTON_TYPE, 2);

    auto colorManager = new ColorsCard;
    connect(ui->m_start_colormanager, &QPushButton::clicked, colorManager, &ColorsCard::start_view);
}

void QtWidgetFactor::initTabBar()
{
    //tabbar
    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::LeftSide, new QCheckBox);

    //tab shape
    static const QMap<QTabWidget::TabShape, QRadioButton*> shapeMap = {
        {QTabWidget::Rounded, ui->radio_tabshape_rounded},
        {QTabWidget::Triangular, ui->radio_tabshape_trianguler}};
    auto tabShape = ui->tabWidget->tabShape();
    auto iter = shapeMap.find(tabShape);
    if (iter != shapeMap.end())
    {
        iter.value()->setChecked(true);
    }

    connect(ui->buttonGroup_2, QOverload<QAbstractButton*, bool>::of(&QButtonGroup::buttonToggled), [&](QAbstractButton* button, bool toggled)
            {
                if (toggled)
                {
                    auto radio = qobject_cast<QRadioButton*>(button);
                    if (!radio)
                        return;

                    for (auto iter = shapeMap.begin(); iter != shapeMap.end(); iter++)
                    {
                        if (iter.value() == radio)
                            ui->tabWidget->setTabShape(iter.key());
                    }
                }
            });

    //tab position
    static const QMap<QTabWidget::TabPosition, QRadioButton*> positionMap = {
        {QTabWidget::North, ui->radio_tabposition_north},
        {QTabWidget::South, ui->radio_tabposition_south},
        {QTabWidget::West, ui->radio_tabposition_west},
        {QTabWidget::East, ui->radio_tabposition_east}};
    auto tabPosition = ui->tabWidget->tabPosition();
    auto positionIter = positionMap.find(tabPosition);
    if (positionIter != positionMap.end())
    {
        positionIter.value()->setChecked(true);
    }

    connect(ui->buttonGroup, QOverload<QAbstractButton*, bool>::of(&QButtonGroup::buttonToggled), [&](QAbstractButton* button, bool toggled)
            {
                if (toggled)
                {
                    auto radio = qobject_cast<QRadioButton*>(button);
                    if (!radio)
                        return;

                    for (auto iter = positionMap.begin(); iter != positionMap.end(); iter++)
                    {
                        if (iter.value() == radio)
                            ui->tabWidget->setTabPosition(iter.key());
                    }
                }
            });

    //document mode
    bool documentMode = ui->tabWidget->documentMode();
    ui->check_docmode->setChecked(documentMode);
    connect(ui->check_docmode, &QCheckBox::toggled, [&](bool checked)
            { ui->tabWidget->setDocumentMode(checked); });

    //auto hide
    bool autoHide = ui->tabWidget->tabBarAutoHide();
    ui->check_tab_autohide->setChecked(autoHide);
    connect(ui->check_tab_autohide, &QCheckBox::toggled, [&](bool checked)
            { ui->tabWidget->setTabBarAutoHide(checked); });
}

void QtWidgetFactor::initThemedIcon()
{
    auto mainLayout = new QHBoxLayout(ui->themedIconContainerWidget);

    QList<QString> iconList = {
        "firefox",
        "kcp-authentication-driver",
        "kcp-authentication-fingerprint"
    };
    for(const auto& iconName:iconList)
    {
        auto btn = new QPushButton(ui->themedIconContainerWidget);
        auto icon = QIcon::fromTheme(iconName);
        btn->setIcon(icon);
        btn->setText(iconName);
        btn->setIconSize(QSize(48,48));
        mainLayout->addWidget(btn);
    }
}

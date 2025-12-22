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
#include <qpa/qplatformtheme.h>
#include <qpa/qplatformthemeplugin.h>
#include <QApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QStyleFactory>
#include <QStylePlugin>
#include "examples/widget-factory.h"
#include "lib/theme/palette.h"

int main(int argc, char* argv[])
{
//    qputenv("QT_QPA_PLATFORMTHEME","kiran");
    QApplication app(argc, argv);
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);

    qDebug() << "===================================================";
    qDebug() << "supported style:" << QStyleFactory::keys();
    qDebug() << "current style:" << app.style()->objectName();
    qDebug() << "app palette:" << app.palette().brush(QPalette::Highlight);
    qDebug() << "===================================================";

    QtWidgetFactor factor;
    factor.show();

    // QPalette palette = app.palette();
    // palette.setColor(QPalette::Active, QPalette::Window, "red");
    // app.setPalette(palette, "");
    return app.exec();
}
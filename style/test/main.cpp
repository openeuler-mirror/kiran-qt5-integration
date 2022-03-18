//
// Created by lxh on 2021/10/18.
//

#include <QApplication>
#include <qpa/qplatformthemefactory_p.h>
#include <qpa/qplatformthemeplugin.h>
#include <qpa/qplatformtheme.h>
#include <QStylePlugin>
#include <QPluginLoader>
#include <QDebug>

bool test_load_plugin()
{
    QPluginLoader loader;
    loader.setFileName("/usr/lib64/qt5/plugins/platformthemes/libqkiran.so");
    if(loader.load())
    {
        auto plugin = qobject_cast<QPlatformThemePlugin*>(loader.instance());
        if(!plugin)
        {
            qInfo() << "can't cast to QPlatformThemePlugin";
            return false;
        }
        else
        {
            qInfo() << "cast to QPlatformThemePlugin success";
        }
    }
    else
    {
        qInfo() << "load failed" << loader.errorString();
        return false;
    }

    loader.setFileName("/usr/lib64/qt5/plugins/styles/libkiranstyle.so");
    if(loader.load())
    {
        auto plugin = qobject_cast<QStylePlugin*>(loader.instance());
        if(!plugin)
        {
            qInfo() << "can't cast to QStylePlugin";
            return false;
        }
        else
        {
            qInfo() << "cast to QStylePlugin success";
        }
    }
    else
    {
        qInfo() << "load failed" << loader.errorString();
        return false;
    }

    return true;
}

#include "qt-widget-factor.h"
int main(int argc,char* argv[])
{
    QApplication app(argc,argv);
    QtWidgetFactor factor;
    factor.show();
    return app.exec();
}
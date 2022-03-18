//
// Created by lxh on 2021/10/18.
//

#include "kiran-theme-plugin.h"
#include "kiran-theme.h"

#include <QDebug>

KiranThemePlugin::KiranThemePlugin(QObject *parent)
    : QPlatformThemePlugin(parent)
{
}

KiranThemePlugin::~KiranThemePlugin()
{
}

QPlatformTheme* KiranThemePlugin::create(const QString& key, const QStringList& paramList)
{
    static QSet<QString> keySet = {"kiran","gnome","gtk3","qgnomeplatform"};
    if(keySet.contains(key))
    {
        return new KiranTheme(paramList);
    }
    return nullptr;
}

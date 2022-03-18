//
// Created by lxh on 2021/10/18.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_PLUGIN_H_
#define KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_PLUGIN_H_

#include <qpa/qplatformthemeplugin.h>

class KiranThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "qkiran.json")
public:
    explicit KiranThemePlugin(QObject *parent = nullptr);
    ~KiranThemePlugin() override;

    QPlatformTheme *create(const QString &key, const QStringList &paramList) override;

private:
};

#endif  //KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_THEME_PLUGIN_H_

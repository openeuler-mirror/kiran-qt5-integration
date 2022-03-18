//
// Created by lxh on 2021/11/22.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_PLUGIN_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_PLUGIN_H_

#include <QStylePlugin>

class KiranStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "../kiran-style-plugin.json")
public:
    explicit KiranStylePlugin(QObject* parent = nullptr);
    QStringList keys() const;
    QStyle* create(const QString&) override;
};

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_PLUGIN_H_

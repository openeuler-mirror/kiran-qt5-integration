//
// Created by lxh on 2021/11/22.
//

#include "kiran-style-plugin.h"
#include "style.h"

#include <QCoreApplication>
#include <QDebug>

KiranStylePlugin::KiranStylePlugin(QObject *parent) : QStylePlugin(parent)
{

}

QStringList KiranStylePlugin::keys() const
{
    return QStringList() << "Kiran"
                         << "Kiran-dark";
}

QStyle *KiranStylePlugin::create(const QString & key)
{
    //TODO:根据情况，后期加入黑名单，屏蔽掉一些不需要加载KiranStyle的Qt程序
    qInfo("create style:%s",key.toStdString().c_str());

    if( key.compare("Kiran",Qt::CaseInsensitive) == 0 ||
        key.compare("Kiran-dark",Qt::CaseInsensitive) == 0 )
    {
        return new Kiran::Style::Style();
    }

    return nullptr;
}

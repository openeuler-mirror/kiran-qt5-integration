/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
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

#include "configuration.h"
#include <QMutex>
#include <QMutexLocker>
#include <QRegExp>
#include <QScopedPointer>
#include <QSettings>
#include <declarations.h>
#include <QString>
#include <QSet>
#include <QDebug>

namespace Kiran
{
#define KQI_CONFIGURATION_PATH "/etc/kiran-integration/kiran-qt5-integration.ini"
#define KQI_INI_GROUP_NAME "Style"
#define KQI_INI_KEY_ENABLED_APPS "enabled-apps"

Configuration::Configuration() :
m_settings(new QSettings(KQI_CONFIGURATION_PATH, QSettings::IniFormat))
{
}

Configuration* Configuration::instance()
{
    static QMutex mutex;
    static QScopedPointer<Configuration> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new Configuration);
        }
    }

    return pInst.data();
}

QSet<QString> Configuration::getEnabledApps()
{
    QSet<QString> enabledApps;

    m_settings->beginGroup(KQI_INI_GROUP_NAME);
    QStringList enabledAppsList = m_settings->value(KQI_INI_KEY_ENABLED_APPS).toStringList();
    m_settings->endGroup();

    enabledApps = QSet<QString>::fromList(enabledAppsList);
    return enabledApps;
}

bool Configuration::match(const QString& appName)
{
    QRegExp regExp("", Qt::CaseSensitive, QRegExp::Wildcard);

    for (const QString& appPattern : m_kiranStyleApps)
    {
        regExp.setPattern(appPattern);
        RETURN_VAL_IF_TRUE(regExp.exactMatch(appName), true);
    }

    QSet<QString> enabledApps = getEnabledApps();
    for (const QString& appPattern : enabledApps)
    {
        regExp.setPattern(appPattern);
        RETURN_VAL_IF_TRUE(regExp.exactMatch(appName), true);
    }

    return false;
}
}  // namespace Kiran

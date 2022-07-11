#include "kiran-integration-settings.h"

#include <QMutex>
#include <QMutexLocker>
#include <QScopedPointer>
#include <QSettings>

KiranIntegrationSettings::KiranIntegrationSettings()
{
    init();
}

KiranIntegrationSettings::~KiranIntegrationSettings()
{

}

void KiranIntegrationSettings::init()
{
    QSettings settings("/etc/kiran-qt5-integration/kiran-qt5-integration.ini",QSettings::IniFormat);
    QStringList disableStyleApps = settings.value("Style/disable-kiran-style-apps","").toStringList();
    m_disableKiranStyleApps.append(disableStyleApps);
}

KiranIntegrationSettings* KiranIntegrationSettings::instance()
{
    static QMutex mutex;
    static QScopedPointer<KiranIntegrationSettings> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new KiranIntegrationSettings);
        }
    }

    return pInst.data();
}

QStringList KiranIntegrationSettings::getDisableKiranStyleApps()
{
    return m_disableKiranStyleApps;
}

#include "kiran-integration-settings.h"

#include <QMutex>
#include <QMutexLocker>
#include <QScopedPointer>
#include <QSettings>
#include <QRegExp>

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
    QStringList kiranStyleApps = settings.value("Style/kiran-style-apps","").toStringList();
    m_kiranStyleApps.unite(kiranStyleApps.toSet());
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

QSet<QString> KiranIntegrationSettings::getKiranStyleApps()
{
    return m_kiranStyleApps;
}

bool KiranIntegrationSettings::appKiranStyleAvailable(const QString& app)
{
    bool matched = false;
    QRegExp regExp("",Qt::CaseSensitive,QRegExp::Wildcard);

    auto set = KiranIntegrationSettings::instance()->getKiranStyleApps();
    foreach(auto pattern,set)
    {
        regExp.setPattern(pattern);
        matched = regExp.exactMatch(app);
        if( matched )
            break;
    }

    return matched;
}
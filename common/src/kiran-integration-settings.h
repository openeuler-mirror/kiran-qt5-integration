#pragma once

#include <QStringList>

class KiranIntegrationSettings
{
public:
    static KiranIntegrationSettings* instance();
    ~KiranIntegrationSettings();
    
    QStringList getDisableKiranStyleApps();

private:
    KiranIntegrationSettings();
    void init();
    
private:
    QStringList m_disableKiranStyleApps = {"lightdm-kiran-greeter","kiran-screensaver"};
};
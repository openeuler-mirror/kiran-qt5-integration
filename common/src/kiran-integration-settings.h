#pragma once

#include <QStringList>
#include <QSet>

class KiranIntegrationSettings
{
public:
    static KiranIntegrationSettings* instance();
    ~KiranIntegrationSettings();

    QSet<QString> getKiranStyleApps();
    static bool appKiranStyleAvailable(const QString& app);

private:
    KiranIntegrationSettings();
    void init();

private:
    QSet<QString> m_kiranStyleApps = {
        "kiran-control-panel",
        "kiran-cpanel-*",
        "kiran-polkit-agent",
        "kiran-avatar-editor",
        "kiran-calculator",
        "kiran-power-status-icon",
        "kiran-network-status-icon",
        "kiran-audio-status-icon",
        "kiran-flameshot"
    };
};
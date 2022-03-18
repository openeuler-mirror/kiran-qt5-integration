//
// Created by lxh on 2021/11/9.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_APPEARANCE_MONITOR_H_
#define KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_APPEARANCE_MONITOR_H_

#include <QObject>
#include <QFont>

class KiranDisplayProxy;
class KiranAppearanceProxy;
class KiranAppearanceMonitor : public QObject
{
    Q_OBJECT
private:
    explicit KiranAppearanceMonitor(QObject* parent= nullptr);
public:
    static KiranAppearanceMonitor* instance();
    ~KiranAppearanceMonitor() override;

    QFont appFont() const;
    QFont titleBarFont() const;
    QString iconTheme() const;
    QString gtkTheme() const;
    int scaleFactor() const;

signals:
    void appFontChanged(QFont font);
    void titleBarFontChanged(QFont font);
    void scaleFactorChanged(int factor);
    void iconThemeChanged(QString iconTheme);
    void gtkThemeChanged(QString gtkTheme);

private:
    static bool parseFontValue(const QString& font,QString& fontName,int& fontSize);

private slots:
    void handleFontSettingChanged(int type,const QString& fontValue);
    void handleWindowScaleFactorChanged(int scaleFactor);
    void handleThemeSettingChanged(int type,const QString& themeName);

private:
    QString m_appFontName = "Noto Sans CJK";
    int m_appFontSize = 10;

    QString m_titleBarFontName = "Noto Sans CJK";
    int m_titleBarFontSize = 11;

    int m_scaleFactor = 0;

    QString m_iconTheme = "hicolor";
    QString m_gtkThemeName = "kiran";
    KiranDisplayProxy* m_displayIface;
    KiranAppearanceProxy* m_appearanceIface;
};

#endif  //KIRAN_QT5_PLATFORMTHEME_PLATFORMTHEME_KIRAN_APPEARANCE_MONITOR_H_

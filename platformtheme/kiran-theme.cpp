/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "kiran-theme.h"
#include "kiran-appearance-monitor.h"
#include "logging-category.h"

#include <private/qguiapplication_p.h>
#include <private/qiconloader_p.h>
#include <private/qwidgetwindow_p.h>
#include <private/qwindow_p.h>
#include <qpa/qplatformscreen.h>
#define private public
#include <private/qhighdpiscaling_p.h>
#undef private

#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QStyle>
#include <QTimer>
#include <QVariant>
#include <QWindow>
#include "style-palette.h"

using namespace Kiran;

KiranTheme::KiranTheme(const QStringList& paramList)
    : QGenericUnixTheme()
{
    init();
}

KiranTheme::~KiranTheme()
{
}

bool KiranTheme::usePlatformNativeDialog(QPlatformTheme::DialogType type) const
{
    return QGenericUnixTheme::usePlatformNativeDialog(type);
}

QPlatformDialogHelper* KiranTheme::createPlatformDialogHelper(QPlatformTheme::DialogType type) const
{
    return QGenericUnixTheme::createPlatformDialogHelper(type);
}

QIconEngine* KiranTheme::createIconEngine(const QString& iconName) const
{
    return QGenericUnixTheme::createIconEngine(iconName);
}

QPixmap KiranTheme::standardPixmap(QPlatformTheme::StandardPixmap sp, const QSizeF& size) const
{
    return QGenericUnixTheme::standardPixmap(sp, size);
}

QIcon KiranTheme::fileIcon(const QFileInfo& fileInfo, QPlatformTheme::IconOptions iconOptions) const
{
    return QGenericUnixTheme::fileIcon(fileInfo, iconOptions);
}

QVariant KiranTheme::themeHint(QPlatformTheme::ThemeHint hint) const
{
    switch (hint)
    {
    case SystemIconThemeName:
        return QStringList() << m_settingsMonitor->iconTheme();
    case SystemIconFallbackThemeName:
        return QStringList() << "hicolor";
    case IconThemeSearchPaths:
        return QStringList() << ".local/share/icons"
                             << "/usr/share/icons"
                             << "/usr/local/share/icons";
    case StyleNames:
        return QStringList{"kiran","fusion"};
    default:
        break;
    }
    return QGenericUnixTheme::themeHint(hint);
}

const QPalette* KiranTheme::palette(QPlatformTheme::Palette type) const
{
    if (type != SystemPalette)
    {
        return QGenericUnixTheme::palette(type);
    }

    static QPalette kiranThemePalette;
    StylePalette::instance()->polishPalette(&kiranThemePalette);
    return &kiranThemePalette;
}

void KiranTheme::init()
{
    qDebug(kiranPlatformTheme) << "init kiran theme...";
    m_settingsMonitor = KiranAppearanceMonitor::instance();

    m_scaleFactor = m_settingsMonitor->scaleFactor();
    qDebug(kiranPlatformTheme) << "\tscale factor:" << m_scaleFactor;

    m_systemFont.setFamily(m_settingsMonitor->appFont().family());
    m_systemFont.setPointSize(m_settingsMonitor->appFont().pointSize());
    QApplication::setFont(m_systemFont);
    qDebug(kiranPlatformTheme) << "\tapplication font:" << m_settingsMonitor->appFont().family() << m_settingsMonitor->appFont().pointSize();

    m_titleBarFont.setFamily(m_settingsMonitor->titleBarFont().family());
    m_titleBarFont.setPointSize(m_settingsMonitor->titleBarFont().pointSize());
    qDebug(kiranPlatformTheme) << "\ttitle bar font:" << m_settingsMonitor->titleBarFont().family() << m_settingsMonitor->titleBarFont().pointSize();

    QObject::connect(m_settingsMonitor, &KiranAppearanceMonitor::appFontChanged, this, &KiranTheme::handleAppFontChanged);
    QObject::connect(m_settingsMonitor, &KiranAppearanceMonitor::titleBarFontChanged, this, &KiranTheme::handleTitleBarFontChanged);
    QObject::connect(m_settingsMonitor, &KiranAppearanceMonitor::iconThemeChanged, this, &KiranTheme::handleIconThemeChanged);
    QObject::connect(m_settingsMonitor, &KiranAppearanceMonitor::scaleFactorChanged, this, &KiranTheme::handleScaleFactorChanged);

    // 不从KiranAppearanceMonitor接受主题变更事件，修改为接受KiranPalette的主题变更信号，能监听到系统主题变更以及应用程序手动指定主题
    //QObject::connect(m_settingsMonitor, &KiranAppearanceMonitor::gtkThemeChanged, this, &KiranTheme::handleThemeChanged);
    QObject::connect(StylePalette::instance(),&StylePalette::themeChanged,this,&KiranTheme::handleThemeChanged);

    QObject::connect(qApp, &QApplication::screenAdded, this, &KiranTheme::handleScreenAdded);

    handleScaleFactorChanged(m_scaleFactor);
}

const QFont* KiranTheme::font(QPlatformTheme::Font type) const
{
    switch (type)
    {
    case QPlatformTheme::TitleBarFont:
    {
        static QFont titleBarFont = QFont(QString());
        titleBarFont.setFamily(m_titleBarFont.family());
        titleBarFont.setPointSize(m_titleBarFont.pointSize());
        return &titleBarFont;
    }
    case SystemFont:
    case FixedFont:
    {
        static QFont tempFont = QFont(QString());
        tempFont.setFamily(m_systemFont.family());
        tempFont.setPointSize(m_systemFont.pointSize());
        return &tempFont;
    }
    default:
        break;
    }

    return QGenericUnixTheme::font(type);
}

// NOTE:也可通过删除QGuiApplication::app_font,对qapp下的所有window发送ApplicationFontChanged事件
// 但是若是设置了样式表(setStyleSheet)会抑制字体的传递。该方法可能耗资源更少
void KiranTheme::handleAppFontChanged()
{
    qDebug(kiranPlatformTheme) << "application font changed:"
                               << m_settingsMonitor->appFont().family()
                               << m_settingsMonitor->appFont().pointSize();

    m_systemFont.setFamily(m_settingsMonitor->appFont().family());
    m_systemFont.setPointSize(m_settingsMonitor->appFont().pointSize());
    QApplication::setFont(m_systemFont);
}

void KiranTheme::handleTitleBarFontChanged()
{
    qDebug(kiranPlatformTheme) << "title bar font changed:"
                               << m_settingsMonitor->titleBarFont().family()
                               << m_settingsMonitor->titleBarFont().pointSize();

    m_titleBarFont.setFamily(m_settingsMonitor->titleBarFont().family());
    m_titleBarFont.setPointSize(m_settingsMonitor->titleBarFont().pointSize());

    // 遍历所有窗口，判断是否是KiranWidget中的自定义标题栏修改其标题栏字体
    for (auto window : qGuiApp->allWindows())
    {
        if (window->inherits("QWidgetWindow"))
        {
            auto widgetWindow = static_cast<QWidgetWindow*>(window);
            if (widgetWindow->widget() && widgetWindow->widget()->inherits("KiranTitlebarWindow"))
            {
                auto titleBarWidget = widgetWindow->widget();
                auto titleLabel = titleBarWidget->findChild<QLabel*>("KiranTitlebarTitle");
                titleLabel->setFont(m_titleBarFont);
            }
        }
    }
}

void KiranTheme::handleScaleFactorChanged(int factor)
{
    qDebug(kiranPlatformTheme) << "scale factor changed:"
                               << m_settingsMonitor->scaleFactor();

    // 缩放的话，同时也应该调整设置到窗口管理器的边缘阴影属性
    if (enableRealTimeScaling())
    {
        if (factor == 0)  // 自动缩放
        {
            // QPlatformScreen* screenHandle = screen->handle();
            // qInfo() << screenHandle->name();
            // qInfo() << "\t logical dpi:" << screenHandle->logicalDpi();
            // qInfo() << "\t pixel density:" << screenHandle->pixelDensity();
            // TODO:清空全局分辨率,对每个屏幕单独设置分辨率
        }
        else if (factor == 1 || factor == 2)  // 手动固定分辨率
        {
            qDebug(kiranPlatformTheme) << "update scale factor:" << factor;
            if (qFuzzyCompare(QHighDpiScaling::m_factor, factor))
            {
                return;
            }

            // 清空屏幕的子缩放率
            {
                for (QScreen* screen : qGuiApp->screens())
                {
                    if (!qFuzzyCompare(QHighDpiScaling::screenSubfactor(screen->handle()), 1))
                    {
                        QHighDpiScaling::setScreenFactor(screen, 1);
                    }
                }
            }

            // 设置全局缩放率
            QHighDpiScaling::setGlobalFactor(factor);

            {
                for (QScreen* s : qGuiApp->screens())
                {
                    Q_EMIT s->geometryChanged(s->geometry());
                }
            }

            // 更新窗口大小
            {
                for (QWindow* window : QGuiApplication::allWindows())
                {
                    if (window->type() == Qt::ForeignWindow || window->type() == Qt::Desktop)
                    {
                        return;
                    }

                    if (!window->handle() || !window->isTopLevel())
                        return;

                    const QRect currentGeo = QWindowPrivate::get(window)->geometry;
                    if (!currentGeo.isValid())
                        return;

                    // qInfo() << window->type();
                    // qInfo() << "current geo:" << currentGeo;
                    QRect nativeGeo = window->handle()->geometry();
                    qreal scale = QHighDpiScaling::factor(window);
                    nativeGeo.setSize(currentGeo.size() * scale);
                    // qInfo() << "native geo:" << nativeGeo;

                    window->handle()->setGeometry(nativeGeo);
                    QGuiApplication::sendEvent(window, new QEvent(QEvent::UpdateRequest));
                }
            }
        }
        else
        {
            qDebug(kiranPlatformTheme) << "not support this factor:" << factor;
            return;
        }
    }
    else
    {
        qDebug(kiranPlatformTheme) << "disable real time scaling,ignore scale factor changed!";
    }
}

bool KiranTheme::enableRealTimeScaling()
{
    static bool enable = !qEnvironmentVariableIsSet("QT_DEVICE_PIXEL_RATIO") &&
                         !qEnvironmentVariableIsSet("QT_SCALE_FACTOR") &&
                         !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR") &&
                         !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS") &&
                         !QCoreApplication::testAttribute(Qt::AA_DisableHighDpiScaling) &&
                         !QCoreApplication::testAttribute(Qt::AA_EnableHighDpiScaling) &&
                         qGuiApp->platformName().endsWith("xcb");
    return enable;
}

void KiranTheme::handleScreenAdded(QScreen* screen)
{
    if (m_scaleFactor == 0)  // 计算并自动调整该屏幕的缩放
    {
        // TODO:根据QPlatformTheme中的pixel density设置该屏幕子分辨率
    }
    else if (m_scaleFactor == 1 || m_scaleFactor == 2)
    {
        // TODO:应用全局分辨率至屏幕上
    }
}

void KiranTheme::handleIconThemeChanged()
{
    qInfo() << "update icon theme:" << m_settingsMonitor->iconTheme();
    QIconLoader::instance()->updateSystemTheme();

    // 通知所有窗口重绘制
    QEvent update(QEvent::UpdateRequest);
    for (QWindow* window : qGuiApp->allWindows())
    {
        if (window->type() == Qt::Desktop)
            continue;

        qApp->sendEvent(window, &update);
    }
}

void KiranTheme::handleThemeChanged()
{
    // NOTE: SchemeLoader会接收KiranAppearanceMonitor的GTK主题改变信号，重新加载配色方案
    // 此处只需等到下一个事件循环，通知QGuiApplication重新更新palette
    // clang-format off
    QTimer::singleShot(0, [this] {
        // 此事件会促使QGuiApplication重新从QPlatformTheme中获取系统级别的QPalette
        QWindowSystemInterfacePrivate::ThemeChangeEvent event(nullptr);

        ///NOTE:
        ///QGuiApplicationPrivate::processThemeChanged -> QGuiApplicationPrivate::notifyThemeChanged
        ///5.11.1
        /// QGuiApplicationPrivate::notifyThemeChanged若非特殊设置过Palette，是直接清空QGuiApplicajtionPrivate::app_pal,
        /// 然后重新从QPlatformTheme之中取出system palette,重新构建QGuiApplicajtionPrivate::app_pal
        ///5.15.2
        /// QGuiApplicationPrivate::app_pal初始化
        /// 默认初始化一个QPalette,若QAGuiApplicationPrivate::app_pal不存在，则调用qpalette.cpp之中qt_palette_from_color构造一个
        /// QGuiApplicationPrivate::app_pal会采用上述的QPalette作为默认值
        ///
        /// 后续调用processThemeChanged->updatePalette,都只是QGuiApplicationPrivate::app_pal从QPlatformTheme::palette(system)之中取出缺失的
        /// 所以QGuiApplicationPrivate::app_pal第一次初始化后，第一次主题应用能更新系统级别QPalette，但是之后主题变更应用不了

        ///FIXME:
        ///1.后续看是否有更好的方式能应用更新系统级别QPalette
        ///2.加入判断是否外部自定义设置Application palette，若外部自定义设置了，则应不做处理
        QGuiApplicationPrivate::app_pal = new QPalette(*palette(SystemPalette));

        // 该接口原本用于windows通知窗口主题变化时使用,现用来通知调用QGuiApplicationPrivate::notifyThemeChanged
        QGuiApplicationPrivate::processThemeChanged(&event);
        emit qApp->paletteChanged(*palette(SystemPalette));
    });
    // clang-format on
}

/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
 * svg-icon-switcher is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     youzhengcai <youzhengcai@kylinsec.com.cn>
 */

#include "plugins/platformtheme/themed-icon-engine.h"
#include <private/qiconloader_p.h>
#include <QCryptographicHash>
#include <QDebug>
#include <QFileInfo>
#include <QGuiApplication>
#include <QPainter>
#include <QSvgRenderer>
#include <QTimer>
#include <QWindow>

namespace Kiran
{
namespace Platformtheme
{
ThemedIconEngine::ThemedIconEngine(const QString &themeSvgIconName, const SvgConvertType type)
    : QIconEngine(),
      m_iconName(themeSvgIconName),
      m_svgConvertType(type)
{
    m_settingsMonitor = Kiran::Platformtheme::AppearanceMonitor::instance();
    QObject::connect(m_settingsMonitor, &Kiran::Platformtheme::AppearanceMonitor::gtkThemeChanged, [this]()
                     { changeSvgIconColor(); });
}

ThemedIconEngine::~ThemedIconEngine() = default;

bool ThemedIconEngine::isValid(const QString &themeSvgIconName)
{
    auto themeIconInfo = QIconLoader::instance()->loadIcon(themeSvgIconName);

    auto scalableSvgIcons = getScalableSvgIconFromTheme(themeSvgIconName);
    return !scalableSvgIcons.isEmpty();
}

QSize ThemedIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    auto pmckey = pixmapCacheKey(size, mode, state);
    if (m_pixmapCache.contains(pmckey))
    {
        QPixmap temp = m_pixmapCache.value(pmckey);
        return temp.size();
    }

    auto pm = pixmap(size, mode, state);
    if (pm.isNull())
        return {};

    return pm.size();
}

QPixmap ThemedIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    ensureLoaded();
    auto pmckey = pixmapCacheKey(size, mode, state);

    QPixmap pm;
    if (m_pixmapCache.contains(pmckey))
    {
        QPixmap temp = m_pixmapCache.value(pmckey);
        return temp;
    }

    QSvgRenderer renderer(m_svgIconPath);
    // 设置为图标大小，防止原图像过大导致经img反转像素后图片过大
    auto actualSize = size;

    QImage img(actualSize, QImage::Format_ARGB32_Premultiplied);
    img.fill(0x00000000);

    QPainter p(&img);
    renderer.render(&p);

    // Selected 状态下保持本色
    if (mode != QIcon::Selected)
    {
        if (m_svgConvertType == SvgConvertType::SVG_CONVERT_COMPOSITION)
        {
            m_svgColor = Kiran::Theme::Palette::getDefault()->getBaseColors().baseForeground;
            p.setCompositionMode(QPainter::CompositionMode_SourceIn);
            p.fillRect(QRect(0, 0, actualSize.width(), actualSize.height()), m_svgColor);
        }
        else if (m_svgConvertType == SvgConvertType::SVG_CONVERT_INVERT_PIXELS)
        {
            if (!m_settingsMonitor->gtkTheme().contains("dark"))
            {
                img.invertPixels(QImage::InvertRgb);
            }
        }
    }

    p.end();
    pm = QPixmap::fromImage(img);
    if (!pm.isNull())
    {
        m_pixmapCache.insert(pmckey, pm);
    }
    return pm;
}

void ThemedIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    ensureLoaded();

    QSize pixmapSize = rect.size();
    if (painter->device())
    {
        pixmapSize *= painter->device()->devicePixelRatioF();
    }

    painter->drawPixmap(rect, pixmap(pixmapSize, mode, state));
}

QIconEngine *ThemedIconEngine::clone() const
{
    return new ThemedIconEngine(*this);
}

QByteArray ThemedIconEngine::pixmapCacheKey(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_iconName << size.width() << size.height() << mode << state;
    return QCryptographicHash::hash(data, QCryptographicHash::Sha1);
}

QStringList ThemedIconEngine::getScalableSvgIconFromTheme(const QString &iconName)
{
    QStringList res;
    auto themeIconInfo = QIconLoader::instance()->loadIcon(iconName);
    if (themeIconInfo.entries.isEmpty())
    {
        return res;
    }

    for (auto themeIconEntry : themeIconInfo.entries)
    {
        if ((themeIconEntry->dir.type == QIconDirInfo::Scalable) &&
            (themeIconEntry->filename.endsWith(".svg")) && (QFileInfo::exists(themeIconEntry->filename)))
        {
            res << themeIconEntry->filename;
        }
    }

    return res;
}

void ThemedIconEngine::ensureLoaded()
{
    if (!(QIconLoader::instance()->themeKey() == m_iconLoaderThemeKey))
    {
        qInfo() << "cache need update" << QIconLoader::instance()->themeKey() << "," << m_iconLoaderThemeKey;
        m_svgIconPath.clear();
        m_pixmapCache.clear();

        m_iconLoaderThemeKey = QIconLoader::instance()->themeKey();
        auto resList = getScalableSvgIconFromTheme(m_iconName);

        qInfo() << "update cache" << resList << "choose" << (resList.isEmpty() ? "null" : resList[0]);
        if (!resList.isEmpty())
        {
            m_svgIconPath = resList[0];
        }
    }
}

void ThemedIconEngine::virtual_hook(int id, void *data)
{
    ensureLoaded();

    switch (id)
    {
    case QIconEngine::IconNameHook:
    {
        QString &name = *reinterpret_cast<QString *>(data);
        name = m_iconName;
    }
    break;
    case QIconEngine::IsNullHook:
    {
        *reinterpret_cast<bool *>(data) = m_svgIconPath.isEmpty();
    }
    break;
    default:
        QIconEngine::virtual_hook(id, data);
    }
}

void ThemedIconEngine::changeSvgIconColor()
{
    // 清理图片缓存
    m_pixmapCache.clear();

    // 通知所有窗口重绘制
    QEvent update(QEvent::UpdateRequest);
    for (QWindow *window : qGuiApp->allWindows())
    {
        if (window->type() == Qt::Desktop)
            continue;
        qApp->sendEvent(window, &update);
    }
}
}  // namespace Platformtheme
}  // namespace Kiran

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

#define private public
#include <QFont>
#undef private
#include <private/qfont_p.h>

#include "font-size-manager-private.h"
#include "font-size-manager.h"

#include <QMutex>
#include <QScopedPointer>
#include <QWidget>

#define WIDGET_PROPERTY_BINDED "_kiran_font_manager_binded_"

namespace Kiran
{
FontSizeManager* FontSizeManager::instance()
{
    static QMutex mutex;
    static QScopedPointer<FontSizeManager> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new FontSizeManager);
        }
    }

    return pInst.data();
}

FontSizeManager::~FontSizeManager()
{
}

int FontSizeManager::fontPixelSize(const QFont& font)
{
    int px = font.pixelSize();

    // 若该字体大小使用的为pt，则根据dpi进行相对应转换
    if (px == -1)
    {
        // px = qRound(std::floor(((font.pointSizeF() * font.d->dpi) / 72) * 100 + 0.5) / 100);
        px = qRound((font.pointSizeF() * font.d->dpi) / 72);
    }

    return px;
}

FontSizeManager::FontSizeManager(QObject* parent)
    : QObject(parent),
      d_ptr(new FontSizeManagerPrivate(this))
{
    d_ptr->init();
}

void FontSizeManager::setUniversalFontSize(int fontSize)
{
    int diff = fontSize - d_ptr->m_fontSizes[FontSizeManager::FONT_LEVEL_4];

    if (diff != d_ptr->m_fontSizeDiff)
    {
        d_ptr->m_fontSizeDiff = diff;
        d_ptr->updateWidgetFont();
    }
}

void FontSizeManager::bind(QWidget* w, FontSizeManager::FontLevelEnum fontLevel)
{
    bind(w, fontLevel, w->font().weight());
}

void FontSizeManager::bind(QWidget* w, FontSizeManager::FontLevelEnum fontLevel, int weight)
{
    if (w == nullptr || (fontLevel < 0 || fontLevel >= FontSizeManager::FONT_LEVEL_LAST))
        return;

    d_ptr->m_binderMap[fontLevel].append(w);
    w->setFont(getFont(fontLevel, weight, w->font()));

    if (!w->property(WIDGET_PROPERTY_BINDED).toBool())
    {
        d_ptr->m_destorySignalMapper.setMapping(w, w);
        connect(w, &QWidget::destroyed, &d_ptr->m_destorySignalMapper, QOverload<>::of(&QSignalMapper::map));
        w->setProperty(WIDGET_PROPERTY_BINDED, true);
    }
}

void FontSizeManager::unbind(QWidget* w)
{
    bool removed = false;

    for (auto& i : d_ptr->m_binderMap)
    {
        if (i.removeOne(w))
        {
            removed = true;
        }
    }

    if (removed)
    {
        disconnect(w, &QWidget::destroyed, &d_ptr->m_destorySignalMapper, QOverload<>::of(&QSignalMapper::map));
        d_ptr->m_destorySignalMapper.removeMappings(w);
        w->setProperty(WIDGET_PROPERTY_BINDED, false);
    }
}

QFont FontSizeManager::getFont(FontSizeManager::FontLevelEnum fontLevel, const QFont& base) const
{
    return getFont(fontLevel, base.weight(), base);
}

QFont FontSizeManager::getFont(FontSizeManager::FontLevelEnum fontLevel, int weight, const QFont& base) const
{
    QFont font = base;

    font.setPixelSize(d_ptr->m_fontSizes[fontLevel] + d_ptr->m_fontSizeDiff);
    font.setWeight(weight);

    return font;
}
}  // namespace Kiran
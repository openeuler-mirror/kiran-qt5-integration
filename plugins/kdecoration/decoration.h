/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-qt5-integraiton is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#pragma once
#include <KDecoration2/Decoration>
#include <KDecoration2/DecorationButtonGroup>
#include <KDecoration2/DecorationThemeProvider>
#include <KPluginMetaData>
#include <QVariant>
#include "utils.h"

namespace Kiran
{
namespace KDecoration
{
class InternelSetting;
class Decoration : public KDecoration2::Decoration
{
    friend class Button;
    Q_OBJECT
public:
    Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Decoration() override;
    void paint(QPainter *painter, const QRect &repaintRegion) override;

public slots:
    void init() override;

private:
    InternelSetting *getInternelSetting() const
    {
        return m_internelSetting;
    }
    bool atEdge(Qt::Edge edge) const;
    void updateBorders();
    void updateResizeBorders();
    void updateTitleBar();
    void updateButtonsGeometry();
    void updateShadow();

    QColor frameBackgroundColor() const;
    int borderWidth() const;
    QColor borderColor() const;
    QColor titleBarBackgroundColor() const;
    QColor titleBarForegroundColor() const;

    void paintFrameBackground(QPainter *painter, const QRect &repaintRegion) const;
    void paintTitleBarBackground(QPainter *painter, const QRect &repaintRegion) const;
    void paintCaption(QPainter *painter, const QRect &repaintRegion) const;
    void paintButtons(QPainter *painter, const QRect &repaintRegion) const;
    void paintBorder(QPainter *painter, const QRect &repaintRegion) const;

private:
    KDecoration2::DecorationButtonGroup *m_leftButtons;
    KDecoration2::DecorationButtonGroup *m_rightButtons;
    InternelSetting *m_internelSetting;
};

// 外部提取插件主题的接口
class ThemeProvider : public KDecoration2::DecorationThemeProvider
{
    Q_OBJECT
public:
    explicit ThemeProvider(QObject *parent,
                           const KPluginMetaData &data,
                           const QVariantList &args);

    QList<KDecoration2::DecorationThemeMetaData> themes() const override
    {
        return m_themes;
    }

private:
    void init();

private:
    const KPluginMetaData m_data;
    QList<KDecoration2::DecorationThemeMetaData> m_themes;
};

}  // namespace KDecoration
}  // namespace Kiran

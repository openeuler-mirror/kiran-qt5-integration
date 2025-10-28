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
#include <QVariant>

namespace Kiran
{
namespace KDecoration
{
class CloseButton;
class MaximizeButton;
class MinimizeButton;
class Decoration : public KDecoration2::Decoration
{
    Q_OBJECT
    friend class Button;

public:
    Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Decoration() override;

    void paint(QPainter *painter, const QRect &repaintRegion) override;

public slots:
    void init() override;

private:
    void updateBorders();
    void updateResizeBorders();
    void updateTitleBar();
    void updateButtonsGeometry();
    void updateShadow();

    int titleBarHeight() const;

    QColor titleBarBackgroundColor() const;
    QColor titleBarForegroundColor() const;

    void paintFrameBackground(QPainter *painter, const QRect &repaintRegion) const;
    void paintTitleBarBackground(QPainter *painter, const QRect &repaintRegion) const;
    void paintCaption(QPainter *painter, const QRect &repaintRegion) const;
    void paintButtons(QPainter *painter, const QRect &repaintRegion) const;

private:
    KDecoration2::DecorationButtonGroup *m_leftButtons;
    KDecoration2::DecorationButtonGroup *m_rightButtons;
};

}  // namespace KDecoration
}  // namespace Kiran

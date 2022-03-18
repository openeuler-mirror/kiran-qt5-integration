//
// Created by lxh on 2021/11/22.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_H_

#include <QAbstractItemDelegate>

#include "draw-helper/draw-common-helper.h"

#if 1
#include <private/qfusionstyle_p.h>
#define ParentStyle QFusionStyle
#else
#include <QCommonStyle>
#define ParentStyle QCommonStyle
#endif

namespace Kiran
{
namespace Style
{
class SchemeLoader;
class Style : public ParentStyle
{
public:
    explicit Style();
    ~Style();

    int styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const override;

    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const override;
    void drawComplexControl(ComplexControl control,const QStyleOptionComplex* option,QPainter* painter,const QWidget* widget) const override;
    void drawControl(ControlElement element,const QStyleOption* option,QPainter* painter,const QWidget* widget) const override;

    QRect subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const override;
    QRect subControlRect(ComplexControl cc,const QStyleOptionComplex* opt,SubControl sc,const QWidget* widget) const override;
    QSize sizeFromContents(ContentsType type,const QStyleOption* option,const QSize& contentSize,const QWidget* widget) const override;

    void polish(QWidget* widget) override;
    void polish(QApplication* app) override;
    void polish(QPalette& pal) override;

private:
    SchemeLoader* m_schemeLoader = nullptr;
};
}  // namespace Style
}  // namespace Kiran

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_KIRAN_STYLE_H_

#ifndef __STYLE_DEFINE_H__
#define __STYLE_DEFINE_H__

#include <QFlags>
#include <QObject>
namespace Kiran
{
namespace Style
{
Q_NAMESPACE
enum ArrowOrientation
{
    Arrow_Up,
    Arrow_Down,
    Arrow_Left,
    Arrow_Right
};
enum Corner
{
    CornerTopLeft = 0x1,
    CornerTopRight = 0x2,
    CornerBottomLeft = 0x4,
    CornerBottomRight = 0x8,
    CornersTop = CornerTopLeft|CornerTopRight,
    CornersBottom = CornerBottomLeft|CornerBottomRight,
    CornersLeft = CornerTopLeft|CornerBottomLeft,
    CornersRight = CornerTopRight|CornerBottomRight,
    AllCorners = CornerTopLeft|CornerTopRight|CornerBottomLeft|CornerBottomRight,
};
Q_DECLARE_FLAGS( Corners, Corner )
}
}

#endif
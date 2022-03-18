//
// Created by lxh on 2021/12/8.
//

#ifndef KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_RENDER_HELPER_H_
#define KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_RENDER_HELPER_H_

#include <QTabBar>
#include <define.h>

class QStyleOption;
class QStyleOptionToolButton;

class QWidget;
class QPainter;
namespace Kiran
{
namespace Style
{
class PainterSaver
{
public:
    explicit PainterSaver(QPainter* painter);
    ~PainterSaver();
private:
    QPainter* _painter;
};
namespace RenderHelper
{
//获取助记符的文本标志位
int mnemonicsTextFlags();

bool showIconsOnPushButtons();

//判断是否绘制对象为QtQuick控件
bool isQtQuickControl(const QStyleOption *option, const QWidget *widget);

//根据TabBar形状获取是否显示为垂直
bool isVerticalTab(const QTabBar::Shape &shape);

//判断控件是否位于菜单
bool isMenuTitle(const QWidget* widget);

//当QPainter中QPen宽度为１时，矩形每个笔画像素侧面向内移动半个像素
//这样可以使笔划是像素完美的，而不是像素之间的模糊，并防止带有笔划的矩形变得大于矩形的原始大小
QRectF strokedRect(const QRectF &rect, const qreal penWidth);

//矩形收缩内边距
QRect insideMargin(const QRect& r,int margin);
QRect insideMargin(const QRect& r, int marginWidth, int marginHeight);

//获取矩形中居中一块大小的矩形
QRect centerRect(const QRect& rect, int width, int height);
QRect centerRect(const QRect& rect, const QSize& size);

//矩形往外扩张边距大小
QSize expandSize(const QSize& size, int margin);
QSize expandSize(const QSize& size, int marginWidth, int marginHeight);

//获取圆角绘制路径
QPainterPath roundedPath(const QRectF& rect, Corners corners, int radius);

//绘制边框
void renderFrame(QPainter* painter, const QRect& rect, int penWidth, int radius, const QColor& color, const QColor& outline = QColor());
void renderFlatFrame(QPainter *painter, const QRect &rect, int radius, const QColor &color, const QColor &outline);

//绘制分割线
void renderSeparator(QPainter* painter, const QRect& rect, bool vertical, const QColor& color);

//获知菜单标题按钮
void renderMenuTitle(const QStyle* style,QPainter* painter,const QStyleOptionToolButton* option,const QWidget* widget,const QColor& separatorColor);

//修改SVG图片之中的fill属性颜色，生成制定大小的Pixmap
QPixmap changeSVGFillColor(const QString& svgFile,const QColor& fillColor,const QSize& size);

//绘制箭头
void renderArrow(QPainter* painter, const QRect& rect, ArrowOrientation orientation, const QColor& color);

//绘制TabBar Tab
void renderTabBarTab(QPainter* painter, const QRect& rect, Corners corners, int radius, const QColor& background, const QColor& border = QColor());
};
}
}

#endif  //KIRAN_QT5_PLATFORMTHEME_STYLE_SRC_RENDER_HELPER_H_

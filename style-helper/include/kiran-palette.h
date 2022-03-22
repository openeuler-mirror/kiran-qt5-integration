#pragma once

#include <QObject>

class KiranPalettePrivate;
class QPalette;
//KiranStyle提供的更便捷的方法取出KiranStyle里相关颜色的类
class KiranPalette : public QObject
{
    Q_OBJECT
public:
    enum ColorState
    {
        Normal,       //普通状态
        Active,       //激活或点击状态
        Hover,        //悬浮状态
        Disabled      //禁用状态
    };
    Q_ENUM(ColorState);

    enum WidgetType
    {
        Window,       //窗口类型或提示框类型
        Bare,         //无边框控件
        Widget,       //默认控件类型
        View,         //视图类型，含有可选中的文本
        Selection,    //选中高亮的类型
        WM            //窗口管理器类型
    };
    Q_ENUM(WidgetType)

    enum WidgetColorRule
    {
        Background,   //背景色
        Foreground,   //前景色
        Border        //边框
    };
    Q_ENUM(WidgetColorRule)

    enum FlagColorRule
    {
        Warning,      //告警-标志色
        Error,        //错误-标志色
        Success       //成功-标志色
    };
    Q_ENUM(FlagColorRule)

    explicit KiranPalette(QObject *parent=0);
    ~KiranPalette();

    void polishPalette(QPalette* palette);

    /**
     * 根据控件类型，控件状态，颜色类型取出颜色表之中预定的颜色
     * \param state 颜色状态，例如:悬浮，禁用，正常等
     * \param type　控件类型，详情见WidgetType枚举值定义
     * \param rule  控件颜色类型，例如:背景，前景，边框
     * \return 颜色表之中预定义的颜色
     */
    QColor color(ColorState state,WidgetType type,WidgetColorRule rule);

    /**
     * 根据控件状态，标志颜色类型取出颜色表之中预定的颜色
     * \param state 颜色状态，例如:悬浮，禁用，正常等
     * \param rule　标志颜色类型，例如:成功，失败，警告等
     * \return 颜色表之中预定义的颜色
     */
    QColor color(ColorState state,FlagColorRule rule);

private:
    KiranPalettePrivate* d_ptr;
};

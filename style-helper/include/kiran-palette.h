#pragma once

#include <kiran-style-global.h>
#include <QObject>

class KiranPalettePrivate;
class QPalette;
class QWidget;
class QStyleOption;
//KiranStyle提供的更便捷的方法取出KiranStyle里相关颜色的类

//TODO: 添加public接口可手动更改palette类型修改当前颜色表不根据系统走
//TODO: 添加private接口提供给platform theme使用修改当前颜色表
//TODO: 修改清理KiranStyle里直接通过SchemeLoader拿颜色的调用,参考kiran-gtk-theme
class KiranPalette : public QObject
{
    Q_OBJECT
private:
    explicit KiranPalette(QObject* parent = nullptr);

public:
    //扩展状态类型，尽量避免使用传入QStyleOption进行匹配
    enum ColorState
    {
        Normal,   //普通状态
        Active,   //激活
        Checked,  //选中状态
        Hover,    //悬浮状态
        Disabled  //禁用状态
    };
    Q_ENUM(ColorState);
    Q_DECLARE_FLAGS(ColorStateFlags, ColorState)
    Q_FLAG(ColorStateFlags)

    //WARNING:WidgetType,WidgetColorRule，FlagColorRule枚举值切勿随意更改
    //该枚举与SchemeLoader::SchemePropertyName相关联
    enum WidgetType
    {
        //窗口类型或提示框类型
        Window          = 0x00000020,
        //无边框控件
        Bare            = 0x00000030,
        //默认控件类型
        Widget          = 0x00000040,
        //视图类型，含有可选中的文本
        View            = 0x00000050,
        //选中高亮的类型
        Selection       = 0x00000060,
        //标题栏
        TitleBar        = 0x00000070
    };
    Q_ENUM(WidgetType)

    enum WidgetColorRule
    {
        //背景色
        Background = 0x00000000,
        //前景色
        Foreground = 0x00000001,
        //边框
        Border     = 0x00000002
    };
    Q_ENUM(WidgetColorRule)

    enum FlagColorRule
    {
        //告警-标志色
        Warning      = 0x00000010,
        //错误-标志色
        Error        = 0x00000011,
        //成功-标志色
        Success      = 0x00000012
    };
    Q_ENUM(FlagColorRule)

    static KiranPalette* instance();
    ~KiranPalette();

    /**
     * 将Kiran调色盘转换至Qt调色盘
     * \param palette
     */
    void polishPalette(QPalette* palette);

    /**
     * 获取当前主题调色盘类型
     * \return 调色盘类型
     */
    KiranStyle::PaletteType paletteType();

    /**
     * 应用程序指定主题调色盘类型
     * Warning:　指定主题调色盘后，不会根据系统主题变更更改主题调色盘
     * \param type　调色盘类型
     */
    void setDesignatedPaletteType(KiranStyle::PaletteType type);

    /**
     * 根据控件类型，控件状态，颜色类型取出颜色表之中预定的颜色
     * \param state 颜色状态，例如:悬浮，禁用，正常等
     * \param type　控件类型，详情见WidgetType枚举值定义
     * \param rule  控件颜色类型，例如:背景，前景，边框
     * \return 颜色表之中预定义的颜色
     */
    QColor color(ColorStateFlags state, WidgetType type, WidgetColorRule rule);

    /**
     * 根据控件状态，标志颜色类型取出颜色表之中预定的颜色
     * \param rule　标志颜色类型，例如:成功，失败，警告等
     * \return 颜色表之中预定义的颜色
     */
    QColor color(FlagColorRule rule);

    /**
     * 根据传入QWidget指针以及StyleOption进行伪状态猜测并进行匹配
     * 匹配顺序(找到一项即返回):
     * 1. 全匹配: QStyleOption之中存在高优先状态(eg:禁用，按下,悬浮,聚焦) 和 传入的specialState　和 从派生关系与QStyleOption检测出的状态
     * 2. 匹配: 高优先级状态: QStyleOption之中存在高优先状态(eg:禁用，按下,悬浮,聚焦)
     * 3. 匹配: 传入的specialState　和 从派生关系与QStyleOption检测出的扩展状态
     * 4. 匹配: 传入的specialState
     * 5. 匹配: 不带伪状态选择器匹配
     * \param widget            控件
     * \param option            QStyle绘制的选项
     * \param type              控件类型
     * \param rule              控件颜色类型
     * \param specialState      指定加入匹配的状态
     * \return 颜色表之中预定义的颜色
     */
    QColor color(const QWidget* widget, const QStyleOption* option, WidgetType type, WidgetColorRule rule, ColorState specialState = Normal);

    void dump();
private:
    KiranPalettePrivate* d_ptr;
};

#pragma once

class QPushButton;
class QProgressBar;
namespace Kiran
{
namespace Style
{
enum ButtonType
{
    BUTTON_Normal,  /** < 普通按钮 */
    BUTTON_Default, /** < 默认按钮，突出显示 */
    BUTTON_Warning  /** < 警告按钮，突出警示显示 */
};
enum ProgressBarTextPosition
{
    PROGRESS_TEXT_LEFT,   /** < 水平-左侧 垂直-上侧 */
    PROGRESS_TEXT_CENTER, /** < 中间 */
    PROGRESS_TEXT_RIGHT   /** < 水平-右侧 垂直-下侧 */
};
/**
 * @brief
 * KiranStyle自定义属性读写接口,可定制部分KiranStyle绘制细节,KiranStyle读出属性值进行特殊的绘制
 * 若控件当前style不为kiranstyle，则不会生效
 */
namespace PropertyHelper
{
/**
  * @brief 获取按钮类型
  * @param btn 按钮控件，从该按钮中取出属性
  * @return 该按钮的Style绘制类型,采取什么风格进行绘制
  * @see Kiran::Style::ButtonType
  */
ButtonType getButtonType(const QPushButton *btn);
/**
 * @brief 设置按钮类型
 * @param btn  按钮
 * @param type 按钮的Style绘制类型,采取什么风格进行绘制
 * @see Kiran::Style::ButtonType
 */
void setButtonType(QPushButton *btn, ButtonType type);

/**
 * @brief 设置QProgressBar进度文本显示位置
 * @param progressBar   进度条
 * @param textPosition  文本绘制位置
 * @see Kiran::Style::ProgressBarTextPosition
 */
void setProgressBarTextPosition(QProgressBar *progressBar, ProgressBarTextPosition textPosition);
/**
 * @brief 获取QProgressBar文本绘制位置
 * @param progressBar 进度条
 * @return 文本绘制位置
 * @see Kiran::Style::ProgressBarTextPosition
 */
ProgressBarTextPosition getProgressBarTextPosition(const QProgressBar *progressBar);
}  // namespace PropertyHelper
}  // namespace Style
}  // namespace Kiran
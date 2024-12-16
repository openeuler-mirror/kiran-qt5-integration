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

#pragma once

#include <QFont>
#include <QObject>

/**
 * 该类主要是解决系统字号设置为单一的应用程序设置，并不会做过多的区分设置项，导致系统描述字号级别不明显
 * 提供以下功能：
 * 1. 字体分级
 *    支持将系统设置的应用程序字体分级，支持1～10个级别,需要相应分级的字体可从该接口统一获取
 * 2. QWidget绑定通用字体的指定字号级别
 *    支持将指定的QWidget以及派生类绑定到指定的字号级别，系统更改 <应用程序字体设置> 时,将会重新计算分级字体大小重新更新字号和字体
 * 3. QWidget绑定指定的字体（不跟随系统），但是字号绑定字号级别，跟随系统变化
 *    支持将指定的QWidget以及派生类绑定定指定的字体以及字号级别，
 */
namespace Kiran
{
class FontSizeManagerPrivate;
class FontSizeManager : public QObject
{
    Q_OBJECT
public:
    enum FontLevelEnum
    {
        FONT_LEVEL_1,
        FONT_LEVEL_2,
        FONT_LEVEL_3,
        //控制中心设置项 "应用程序字体" 对应的字体级别,常规字体
        FONT_LEVEL_4,
        FONT_LEVEL_5,
        FONT_LEVEL_6,
        FONT_LEVEL_7,
        FONT_LEVEL_LAST
    };

public:
    static FontSizeManager* instance();

    ~FontSizeManager();

    /// 通过字体磅数计算字体像素尺寸
    /// \param font 需获取字体像素大小的字体
    /// \return 字体像素尺寸
    static int fontPixelSize(const QFont& font);

    /// 设置系统通用字号大小,且会同步影响所有字号级别的大小
    /// \param fontSize 系统通用字号大小(px)
    void setUniversalFontSize(int fontSize);

    /// 控件绑定字号级别
    /// \param w 需绑定字号的控件
    /// \param fontLevel 字号级别
    void bind(QWidget* w, FontLevelEnum fontLevel);

    /// 控件绑定字号级别
    /// \param w 需绑定字号的控件
    /// \param weight 字体粗细，可直接使用QFont::Weight
    /// \param fontLevel 字号级别
    void bind(QWidget* w, FontLevelEnum fontLevel, int weight);

    /// 控件解除绑定字号级别
    void unbind(QWidget* w);

    /// 获取指定的字体
    /// \param fontLevel 字号级别
    /// \param base 基础字体，可忽略该参数，使用系统字体构造字体对象
    /// \return 指定字体
    QFont getFont(FontLevelEnum fontLevel, const QFont& base = QFont()) const;

    /// 获取指定的字体
    /// \param fontLevel 级别
    /// \param weight 字体粗细，可直接使用QFont::Weight
    /// \param base 基础字体,可忽略该参数
    /// \return 指定的字体
    QFont getFont(FontLevelEnum fontLevel, int weight, const QFont& base = QFont()) const;

private:
    explicit FontSizeManager(QObject* parent = nullptr);

private:
    FontSizeManagerPrivate* d_ptr;
    Q_DECLARE_PRIVATE(FontSizeManager);
};
}  // namespace Kiran
/**
  * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
  * kiran-qt5-integration is licensed under Mulan PSL v2.
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
#include "box-shadow-helper.h"
#include <QVector>
#include <cmath>

namespace Kiran
{
namespace KDecoration
{
namespace BoxShadowHelper
{
// According to the CSS Level 3 spec, standard deviation must be equal to
// half of the blur radius. https://www.w3.org/TR/css-backgrounds-3/#shadow-blur
// Current window size is too small for sigma equal to half of the blur radius.
// As a workaround, sigma blur scale is lowered. With the lowered sigma
// blur scale, area under the kernel equals to 0.98, which is pretty enough.
// Maybe, it should be changed in the future.
const qreal SIGMA_BLUR_SCALE = 0.4375;

inline qreal radiusToSigma(qreal radius)
{
    return radius * SIGMA_BLUR_SCALE;
}

inline int boxSizeToRadius(int boxSize)
{
    return (boxSize - 1) / 2;
}

QVector<int> computeBoxSizes(int radius, int numIterations)
{
    const qreal sigma = radiusToSigma(radius);

    // Box sizes are computed according to the "Fast Almost-Gaussian Filtering"
    // paper by Peter Kovesi.
    int lower = std::floor(std::sqrt(12 * std::pow(sigma, 2) / numIterations + 1));
    if (lower % 2 == 0)
    {
        lower--;
    }

    const int upper = lower + 2;
    const int threshold = std::round((12 * std::pow(sigma, 2) - numIterations * std::pow(lower, 2) - 4 * numIterations * lower - 3 * numIterations) / (-4 * lower - 4));

    QVector<int> boxSizes;
    boxSizes.reserve(numIterations);
    for (int i = 0; i < numIterations; ++i)
    {
        boxSizes.append(i < threshold ? lower : upper);
    }

    return boxSizes;
}

void boxBlurPass(const QImage &src, QImage &dst, int boxSize)
{
    const int alphaStride = src.depth() >> 3;
    const int alphaOffset = QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3;

    const int radius = boxSizeToRadius(boxSize);
    const qreal invSize = 1.0 / boxSize;

    const int dstStride = dst.width() * alphaStride;

    for (int y = 0; y < src.height(); ++y)
    {
        const uchar *srcAlpha = src.scanLine(y);
        uchar *dstAlpha = dst.scanLine(0);

        srcAlpha += alphaOffset;
        dstAlpha += alphaOffset + y * alphaStride;

        const uchar *left = srcAlpha;
        const uchar *right = left + alphaStride * radius;

        int window = 0;
        for (int x = 0; x < radius; ++x)
        {
            window += *srcAlpha;
            srcAlpha += alphaStride;
        }

        for (int x = 0; x <= radius; ++x)
        {
            window += *right;
            right += alphaStride;
            *dstAlpha = static_cast<uchar>(window * invSize);
            dstAlpha += dstStride;
        }

        for (int x = radius + 1; x < src.width() - radius; ++x)
        {
            window += *right - *left;
            left += alphaStride;
            right += alphaStride;
            *dstAlpha = static_cast<uchar>(window * invSize);
            dstAlpha += dstStride;
        }

        for (int x = src.width() - radius; x < src.width(); ++x)
        {
            window -= *left;
            left += alphaStride;
            *dstAlpha = static_cast<uchar>(window * invSize);
            dstAlpha += dstStride;
        }
    }
}

void boxBlurAlpha(QImage &image, int radius, int numIterations)
{
    // Temporary buffer is transposed so we always read memory
    // in linear order.
    QImage tmp(image.height(), image.width(), image.format());

    const QVector<int> boxSizes = computeBoxSizes(radius, numIterations);
    for (const int &boxSize : boxSizes)
    {
        boxBlurPass(image, tmp, boxSize);  // horizontal pass
        boxBlurPass(tmp, image, boxSize);  // vertical pass
    }
}

/**
 * @brief 在指定区域绘制带有模糊效果的阴影
 * 
 * 实现思路：
 * 1. 计算阴影图像尺寸，考虑模糊半径和高分屏缩放。
 * 2. 创建透明 QImage 作为阴影缓冲区。
 * 3. 在缓冲区中用黑色填充原始区域。
 * 4. 对 alpha 通道进行多次 box blur，模拟高斯模糊。
 * 5. 用目标颜色着色阴影区域，仅保留 alpha 区域。
 * 6. 计算阴影显示位置并绘制到目标 QPainter。
 */
void boxShadow(QPainter *p, const QRect &box, const QPoint &offset, int radius, const QColor &color)
{
    // 1. 计算阴影图像尺寸（比原区域大，能容纳模糊扩散），并考虑设备像素比（高分屏适配）
    const QSize size = box.size() + 2 * QSize(radius, radius);
    const qreal dpr = p->device()->devicePixelRatioF();

    QPainter painter;

    // 2. 创建一个透明的 QImage 作为阴影缓冲区
    QImage shadow(size * dpr, QImage::Format_ARGB32_Premultiplied);
    shadow.setDevicePixelRatio(dpr);
    shadow.fill(Qt::transparent);

    // 3. 在缓冲区中，先用黑色填充原始区域（此时只是一个黑色矩形）
    painter.begin(&shadow);
    painter.fillRect(QRect(QPoint(radius, radius), box.size()), Qt::black);
    painter.end();

    // 4. 对缓冲区的 alpha 通道进行多次 box blur（盒式模糊），实现高斯模糊近似，让黑色区域边缘变得柔和，形成阴影的模糊效果
    // 只需模糊 alpha 通道，无需模糊 RGB
    const int numIterations = 3;
    boxBlurAlpha(shadow, radius, numIterations);

    // 5. 用目标颜色（如半透明黑色）覆盖整个阴影区域，保留原本的Alpha，原本有内容的地方更新颜色为Color
    painter.begin(&shadow);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(shadow.rect(), color);
    painter.end();

    // 6. 计算阴影最终显示的位置（中心点偏移），并将阴影绘制到目标 QPainter 上
    QRect shadowRect = shadow.rect();
    shadowRect.setSize(shadowRect.size() / dpr);
    shadowRect.moveCenter(box.center() + offset);
    p->drawImage(shadowRect, shadow);
}

}  // namespace BoxShadowHelper
}  // namespace KDecoration
}  // namespace Kiran
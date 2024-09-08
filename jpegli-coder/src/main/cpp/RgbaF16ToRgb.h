/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#ifndef AVIF_RGBAF16BITNBITU8_H
#define AVIF_RGBAF16BITNBITU8_H

#include <vector>
#include <types.h>

namespace coder {
void RgbaF16ToRgb(const uint16_t *sourceData, uint32_t srcStride,
                  uint8_t *dst, uint32_t dstStride, uint32_t width,
                  uint32_t height, uint32_t bitDepth, Scalar scalar);
}

#endif //AVIF_RGBAF16BITNBITU8_H

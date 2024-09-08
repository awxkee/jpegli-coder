/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#ifndef AVIF_RGB565_H
#define AVIF_RGB565_H

#include <cstdint>

namespace coder {
void Rgb565ToRGB(const uint16_t *sourceData, uint32_t srcStride,
                 uint8_t *dst, uint32_t dstStride, uint32_t width,
                 uint32_t height);

}

#endif //AVIF_RGB565_H

/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#ifndef AVIF_RGB1010102_H
#define AVIF_RGB1010102_H

#include <vector>
#include <types.h>

namespace coder {
void Rgba1010102ToRgb(const uint8_t * src, uint32_t srcStride,
                      uint8_t * dst, uint32_t dstStride,
                      uint32_t width, uint32_t height, Scalar scalar);
}

#endif //AVIF_RGB1010102_H

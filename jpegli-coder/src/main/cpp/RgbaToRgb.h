/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#ifndef JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_RGBATORGB_H_
#define JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_RGBATORGB_H_

#include <cstdint>
#include <types.h>

namespace coder {
void RgbaToRgb(const uint8_t *sourceData, uint32_t srcStride,
               uint8_t *dst, uint32_t dstStride, uint32_t width,
               uint32_t height, Scalar scalar);
}

#endif //JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_RGBATORGB_H_

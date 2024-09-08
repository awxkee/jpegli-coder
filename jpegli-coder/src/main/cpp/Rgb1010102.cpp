/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */


#include "Rgb1010102.h"
#include <vector>
#include <thread>
#include <algorithm>
#include "half.hpp"

using namespace std;

namespace coder {

void Rgba1010102ToRgb(const uint8_t *__restrict__ src, uint32_t srcStride,
                      uint8_t *__restrict__ dst, uint32_t dstStride,
                      uint32_t width, uint32_t height, Scalar scalar) {
  auto mDstPointer = reinterpret_cast<uint8_t *>(dst);
  auto mSrcPointer = reinterpret_cast<const uint8_t *>(src);

  const uint32_t mask = (1u << 10u) - 1u;

  for (int y = 0; y < height; ++y) {

    auto dstPointer = reinterpret_cast<uint8_t *>(mDstPointer);
    auto srcPointer = reinterpret_cast<const uint8_t *>(mSrcPointer);

    for (int x = 0; x < width; ++x) {
      uint32_t rgba1010102 = reinterpret_cast<const uint32_t *>(srcPointer)[0];

      uint32_t r = (rgba1010102) & mask;
      uint32_t g = (rgba1010102 >> 10) & mask;
      uint32_t b = (rgba1010102 >> 20) & mask;
      uint32_t a1 = (rgba1010102 >> 30);

      uint8_t ru = r >> 2;
      uint8_t gu = g >> 2;
      uint8_t bu = b >> 2;
      auto au = static_cast<uint8_t >(static_cast<float>(a1) * (255.f / 3.f));

      ru = (static_cast<uint16_t>(ru) * static_cast<uint16_t>(au)) / static_cast<uint16_t >(255);
      gu = (static_cast<uint16_t>(gu) * static_cast<uint16_t>(au)) / static_cast<uint16_t >(255);
      bu = (static_cast<uint16_t>(bu) * static_cast<uint16_t>(au)) / static_cast<uint16_t >(255);

      if (au == 0) {
        ru = scalar.r;
        gu = scalar.g;
        bu = scalar.b;
      }

      dstPointer[0] = ru;
      dstPointer[1] = gu;
      dstPointer[2] = bu;

      srcPointer += 4;
      dstPointer += 3;
    }

    mSrcPointer += srcStride;
    mDstPointer += dstStride;
  }
}

}
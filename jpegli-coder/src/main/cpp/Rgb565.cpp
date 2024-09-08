/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#include "Rgb565.h"
#include <thread>
#include <vector>
#include "half.hpp"
#include <algorithm>

using namespace std;

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "Rgb565.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();

namespace coder::HWY_NAMESPACE {

using namespace hwy::HWY_NAMESPACE;
using hwy::float16_t;

void
Rgb565ToU8HWYRow(const uint16_t *source, uint8_t *destination, uint32_t width) {
  const FixedTag<uint16_t, 8> du16;
  const FixedTag<uint8_t, 8> du8x8;
  using VU16 = Vec<decltype(du16)>;

  int x = 0;
  const int pixels = 8;

  auto src = reinterpret_cast<const uint16_t *>(source);
  auto dst = reinterpret_cast<uint8_t *>(destination);

  const VU16 redBytes = Set(du16, 0b1111100000000000);
  const VU16 greenBytes = Set(du16, 0b11111100000);
  const VU16 blueBytes = Set(du16, 0b11111);

  for (; x + pixels < width; x += pixels) {
    VU16 row = LoadU(du16, src);
    auto rdu16Vec = DemoteTo(du8x8, ShiftRight<8>(And(row, redBytes)));
    auto gdu16Vec = DemoteTo(du8x8, ShiftRight<3>(And(row, greenBytes)));
    auto bdu16Vec = DemoteTo(du8x8, ShiftLeft<3>(And(row, blueBytes)));

    StoreInterleaved3(rdu16Vec, gdu16Vec, bdu16Vec, du8x8, dst);

    src += pixels;
    dst += pixels * 3;
  }

  for (; x < width; ++x) {
    uint16_t color565 = src[0];

    uint16_t red8 = (color565 & 0b1111100000000000) >> 8;
    uint16_t green8 = (color565 & 0b11111100000) >> 3;
    uint16_t blue8 = (color565 & 0b11111) << 3;

    dst[0] = red8;
    dst[1] = green8;
    dst[2] = blue8;

    src += 1;
    dst += 3;
  }
}

void Rgb565ToU8HWY(const uint16_t *sourceData, uint32_t srcStride,
                   uint8_t *dst, uint32_t dstStride, uint32_t width,
                   uint32_t height) {

  auto mSrc = reinterpret_cast<const uint8_t *>(sourceData);
  auto mDst = reinterpret_cast<uint8_t *>(dst);

  for (uint32_t y = 0; y < height; ++y) {
    Rgb565ToU8HWYRow(reinterpret_cast<const uint16_t *>(mSrc + srcStride * y),
                     reinterpret_cast<uint8_t *>(mDst + dstStride * y),
                     width);
  }
}
}

HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace coder {
HWY_EXPORT(Rgb565ToU8HWY);

void Rgb565ToRGB(const uint16_t *sourceData, uint32_t srcStride,
                 uint8_t *dst, uint32_t dstStride, uint32_t width,
                 uint32_t height) {
  HWY_DYNAMIC_DISPATCH(Rgb565ToU8HWY)(sourceData,
                                      srcStride,
                                      dst,
                                      dstStride,
                                      width,
                                      height);
}

}
#endif
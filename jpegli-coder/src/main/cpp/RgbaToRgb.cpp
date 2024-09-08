/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#include <string>
#include <RgbaToRgb.h>

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "RgbaToRgb.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace coder::HWY_NAMESPACE {
using namespace hwy;
using namespace hwy::HWY_NAMESPACE;
void
RgbaToRGBImpl(const uint8_t *__restrict__ src, const uint32_t srcStride,
              uint8_t *__restrict__ dst, const uint32_t newStride,
              const uint32_t width, const uint32_t height, Scalar scalar) {

  ScalableTag<uint8_t> du;
  using V = Vec<decltype(du)>;
  const int pixels = du.MaxLanes();

  auto replaceR = Set(du, scalar.r);
  auto replaceG = Set(du, scalar.g);
  auto replaceB = Set(du, scalar.b);

  for (int y = 0; y < height; ++y) {

    auto srcPixels = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(src) +
        y * srcStride);
    auto dstPixels = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(dst) +
        y * newStride);

    int x = 0;

    for (; x + pixels < width; x += pixels) {
      V pixels1;
      V pixels2;
      V pixels3;
      V pixels4;
      LoadInterleaved4(du, srcPixels, pixels1, pixels2, pixels3, pixels4);

      auto mask = Eq(pixels4, Zero(du));

      pixels1 = IfThenElse(mask, replaceR, pixels1);
      pixels2 = IfThenElse(mask, replaceG, pixels2);
      pixels3 = IfThenElse(mask, replaceB, pixels3);

      StoreInterleaved3(pixels1, pixels2, pixels3, du, dstPixels);

      srcPixels += 4 * pixels;
      dstPixels += 3 * pixels;
    }

    for (; x < width; ++x) {
      dstPixels[0] = srcPixels[0];
      dstPixels[1] = srcPixels[1];
      dstPixels[2] = srcPixels[2];

      if (srcPixels[3] == 0) {
        dstPixels[0] = scalar.r;
        dstPixels[1] = scalar.g;
        dstPixels[2] = scalar.b;
      }

      srcPixels += 4;
      dstPixels += 3;
    }
  }
}
}
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace coder {
HWY_EXPORT(RgbaToRGBImpl);

void RgbaToRgb(const uint8_t *sourceData, uint32_t srcStride,
               uint8_t *dst, uint32_t dstStride, uint32_t width,
               uint32_t height, Scalar scalar) {
  HWY_DYNAMIC_DISPATCH(RgbaToRGBImpl)(sourceData,
                                      srcStride,
                                      dst,
                                      dstStride,
                                      width,
                                      height,
                                      scalar);
}

}
#endif
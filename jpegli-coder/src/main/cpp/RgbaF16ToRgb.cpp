/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#include "RgbaF16ToRgb.h"
#include "half.hpp"
#include <algorithm>
#include <thread>
#include <vector>
#include <types.h>

using namespace std;

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "RgbaF16ToRgb.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();

namespace coder::HWY_NAMESPACE {

using hwy::HWY_NAMESPACE::Set;
using hwy::HWY_NAMESPACE::FixedTag;
using hwy::HWY_NAMESPACE::Vec;
using hwy::HWY_NAMESPACE::Mul;
using hwy::HWY_NAMESPACE::Max;
using hwy::HWY_NAMESPACE::Min;
using hwy::HWY_NAMESPACE::Zero;
using hwy::HWY_NAMESPACE::BitCast;
using hwy::HWY_NAMESPACE::ConvertTo;
using hwy::HWY_NAMESPACE::PromoteTo;
using hwy::HWY_NAMESPACE::DemoteTo;
using hwy::HWY_NAMESPACE::Combine;
using hwy::HWY_NAMESPACE::Rebind;
using hwy::HWY_NAMESPACE::LowerHalf;
using hwy::HWY_NAMESPACE::UpperHalf;
using hwy::HWY_NAMESPACE::LoadInterleaved4;
using hwy::HWY_NAMESPACE::StoreInterleaved4;
using hwy::HWY_NAMESPACE::Load;
using hwy::HWY_NAMESPACE::Round;
using hwy::HWY_NAMESPACE::Store;
using hwy::HWY_NAMESPACE::Rebind;
using hwy::float16_t;
using hwy::float32_t;

template<class D, class V>
inline V
ClampRound(const D, const V x, const V min, const V max) {
  return Max(Min(Round(x), max), min);
}

HWY_INLINE Vec<FixedTag<uint8_t, 8>>
ConvertRowDirect(Vec<FixedTag<uint16_t, 8>> v,
                 const float maxColors) {
  FixedTag<hwy::float16_t, 4> df16;
  FixedTag<uint16_t, 4> dfu416;
  FixedTag<uint8_t, 8> du8;
  Rebind<float, decltype(df16)> rf32;
  Rebind<int32_t, decltype(rf32)> ri32;
  Rebind<uint8_t, decltype(rf32)> ru8;

  using VU8 = Vec<decltype(du8)>;

  const auto minColors = Zero(rf32);
  const auto vMaxColors = Set(rf32, maxColors);

  auto lf = PromoteTo(rf32, BitCast(df16, LowerHalf(v)));

  auto lower = DemoteTo(ru8, ConvertTo(ri32,
                                       ClampRound(rf32, Mul(
                                           lf,
                                           vMaxColors), minColors, vMaxColors)
  ));
  auto hf = PromoteTo(rf32,
                      BitCast(df16, UpperHalf(dfu416, v)));
  auto upper = DemoteTo(ru8, ConvertTo(ri32,
                                       ClampRound(rf32, Mul(
                                           hf,
                                           vMaxColors), minColors, vMaxColors)
  ));
  return Combine(du8, upper, lower);
}

HWY_INLINE Vec<FixedTag<uint8_t, 8>>
ConvertRow(Vec<FixedTag<uint16_t, 8>> v,
           const float maxColors,
           Vec<FixedTag<float, 4>> alphaLow,
           Vec<FixedTag<float, 4>> alphaHigh) {
  FixedTag<hwy::float16_t, 4> df16;
  FixedTag<uint16_t, 4> dfu416;
  FixedTag<uint8_t, 8> du8;
  Rebind<float, decltype(df16)> rf32;
  Rebind<int32_t, decltype(rf32)> ri32;
  Rebind<uint8_t, decltype(rf32)> ru8;

  using VU8 = Vec<decltype(du8)>;

  const auto minColors = Zero(rf32);
  const auto vMaxColors = Set(rf32, maxColors);

  auto lf = Mul(PromoteTo(rf32, BitCast(df16, LowerHalf(v))), alphaLow);

  auto lower = DemoteTo(ru8, ConvertTo(ri32,
                                       ClampRound(rf32, Mul(
                                           lf,
                                           vMaxColors), minColors, vMaxColors)
  ));
  auto hf = Mul(PromoteTo(rf32,
                          BitCast(df16, UpperHalf(dfu416, v))), alphaHigh);
  auto upper = DemoteTo(ru8, ConvertTo(ri32,
                                       ClampRound(rf32, Mul(
                                           hf,
                                           vMaxColors), minColors, vMaxColors)
  ));
  return Combine(du8, upper, lower);
}

void
RGBAF16BitToNBitRowU8(const uint16_t *source,
                      uint8_t *destination,
                      const uint32_t width,
                      const float maxColors,
                      Scalar scalar) {
  const FixedTag<uint16_t, 8> du16;
  const FixedTag<uint16_t, 4> du16x4;
  const FixedTag<uint8_t, 8> du8;
  const FixedTag<float16_t, 4> df16x4;
  const FixedTag<float32_t, 4> df32x4;
  using VU16 = Vec<decltype(du16)>;
  using VF32 = Vec<decltype(df32x4)>;
  using VU8 = Vec<decltype(du8)>;

  int x = 0;
  const int pixels = 8;

  auto src = reinterpret_cast<const uint16_t *>(source);
  auto dst = reinterpret_cast<uint8_t *>(destination);

  auto replaceR = Set(du8, scalar.r);
  auto replaceG = Set(du8, scalar.g);
  auto replaceB = Set(du8, scalar.b);

  for (; x + pixels < width; x += pixels) {
    VU16 ru16Row;
    VU16 gu16Row;
    VU16 bu16Row;
    VU16 au16Row;
    LoadInterleaved4(du16,
                     reinterpret_cast<const uint16_t *>(src),
                     ru16Row,
                     gu16Row,
                     bu16Row,
                     au16Row);

    auto alphaLow = PromoteTo(df32x4, BitCast(df16x4, LowerHalf(au16Row)));
    auto alphaHigh = PromoteTo(df32x4, BitCast(df16x4, UpperHalf(du16x4, au16Row)));

    auto r16Row = ConvertRow(ru16Row, maxColors, alphaLow, alphaHigh);
    auto g16Row = ConvertRow(gu16Row, maxColors, alphaLow, alphaHigh);
    auto b16Row = ConvertRow(bu16Row, maxColors, alphaLow, alphaHigh);
    auto a16Row = ConvertRowDirect(au16Row, maxColors);

    auto mask = Eq(a16Row, Zero(du8));

    r16Row = IfThenElse(mask, replaceR, r16Row);
    g16Row = IfThenElse(mask, replaceG, g16Row);
    b16Row = IfThenElse(mask, replaceB, b16Row);

    StoreInterleaved3(r16Row, g16Row, b16Row, du8, dst);

    src += 4 * pixels;
    dst += 3 * pixels;
  }

  for (; x < width; ++x) {
    auto tmpR = (uint8_t) std::clamp(std::roundf(LoadHalf(src[0]) * maxColors), 0.0f, maxColors);
    auto tmpG = (uint8_t) std::clamp(std::roundf(LoadHalf(src[1]) * maxColors), 0.0f, maxColors);
    auto tmpB = (uint8_t) std::clamp(std::roundf(LoadHalf(src[2]) * maxColors), 0.0f, maxColors);
    auto tmpA = (uint8_t) std::clamp(std::roundf(LoadHalf(src[3]) * maxColors), 0.0f, maxColors);

    tmpR =
        (static_cast<uint16_t>(tmpR) * static_cast<uint16_t>(tmpA)) / static_cast<uint16_t >(255);
    tmpG =
        (static_cast<uint16_t>(tmpG) * static_cast<uint16_t>(tmpA)) / static_cast<uint16_t >(255);
    tmpB =
        (static_cast<uint16_t>(tmpB) * static_cast<uint16_t>(tmpA)) / static_cast<uint16_t >(255);

    if (tmpA == 0) {
      tmpR = scalar.r;
      tmpG = scalar.g;
      tmpB = scalar.b;
    }

    dst[0] = tmpR;
    dst[1] = tmpG;
    dst[2] = tmpB;

    src += 4;
    dst += 3;
  }
}

void RGBAF16BitToNBitU8(const uint16_t *sourceData, uint32_t srcStride,
                        uint8_t *dst, uint32_t dstStride, uint32_t width,
                        uint32_t height, uint32_t bitDepth, Scalar scalar) {

  const uint32_t maxColors = (1 << bitDepth) - 1;

  auto mSrc = reinterpret_cast<const uint8_t *>(sourceData);
  auto mDst = reinterpret_cast<uint8_t *>(dst);

  for (uint32_t y = 0; y < height; ++y) {
    RGBAF16BitToNBitRowU8(reinterpret_cast<const uint16_t *>(mSrc + y * srcStride),
                          reinterpret_cast<uint8_t *>(mDst + y * dstStride), width,
                          static_cast<float>(maxColors), scalar);
  }
}
}

HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace coder {
HWY_EXPORT(RGBAF16BitToNBitU8);
HWY_DLLEXPORT void RgbaF16ToRgb(const uint16_t *sourceData, uint32_t srcStride,
                                uint8_t *dst, uint32_t dstStride, uint32_t width,
                                uint32_t height, uint32_t bitDepth, Scalar scalar) {
  HWY_DYNAMIC_DISPATCH(RGBAF16BitToNBitU8)(sourceData, srcStride, dst, dstStride, width,
                                           height, bitDepth, scalar);
}
}
#endif
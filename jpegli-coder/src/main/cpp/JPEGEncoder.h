/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#pragma once

#include <cstdint>
#include <vector>

namespace coder {
class JPEGLIEncoder {
 public:
  JPEGLIEncoder(uint8_t *data, uint32_t stride, uint32_t width, uint32_t height, bool useXYB, bool isProgressive)
      : data(data), stride(stride),
        width(width), height(height),
        useXYB(useXYB), isProgressive(isProgressive) {

  }

  std::vector<uint8_t> encode();

  void setQuality(int mQuality) {
    this->quality = mQuality;
  }

 private:
  int quality = 81;
  uint8_t *data;
  const uint32_t stride;
  const uint32_t width;
  const uint32_t height;
  const bool useXYB;
  const bool isProgressive;
};
}
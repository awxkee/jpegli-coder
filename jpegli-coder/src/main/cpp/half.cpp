/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#include "half.hpp"

float LoadHalf(uint16_t f) {
    half_float::half h;
    h.data_ = f;
    return (float)h;
}
/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#ifndef JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_TYPES_H_
#define JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_TYPES_H_

#include <cstdint>
#include <stdexcept>

struct Scalar {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

class EncoderError : public std::runtime_error {
  std::string what_message;

 public:
  EncoderError(const std::string &str) : runtime_error(str), what_message(str) {

  }

  const char *what() const noexcept override {
    return what_message.c_str();
  }
};

#endif //JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_TYPES_H_

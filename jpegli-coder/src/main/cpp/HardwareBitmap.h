/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#ifndef JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_HARDWAREBITMAP_H_
#define JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_HARDWAREBITMAP_H_

#include <jni.h>
#include "android/bitmap.h"
#include <types.h>
#include "JniException.h"
#include "HardwareBuffersCompat.h"
#include <android/hardware_buffer.h>
#include <android/log.h>

#define LOG_TAG "HardwareBitmap"
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class HardwareBitmap {
 private:
  AndroidBitmapInfo info = {0};
  AHardwareBuffer *buffer_ = nullptr;
  AHardwareBuffer_Desc desc = {0};

 public:
  HardwareBitmap(JNIEnv *env, jobject bitmap) {
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
      std::string exception = "Can't retrieve bitmap info";
      throwException(env, exception);
      return;
    }

    if (!(info.flags & ANDROID_BITMAP_FLAGS_IS_HARDWARE)) {
      std::string exception = "Bitmap is not hardware, when expected to be hardware";
      throwException(env, exception);
      return;
    }

    if (androidOSVersion() < 30) {
      std::string exception = "Hardware bitmaps are supported only on APIs 30+";
      throwException(env, exception);
      return;
    }

    if (!loadAHardwareBuffersAPI()) {
      std::string exception =
          "Can't access Hardware bitmap APIs, supported only on 30+ and real devices, not a simulator";
      throwException(env, exception);
      return;
    }

    auto result = AndroidBitmap_getHardwareBuffer_compat(env, bitmap, &buffer_);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS || !buffer_) {
      std::string exception =
          "Retrieving hardware buffer from bitmap is failed, supported only on 30+ and real devices, not a simulator";
      throwException(env, exception);
      return;
    }

    AHardwareBuffer_describe_compat(buffer_, &desc);

    if (desc.usage & ~(AHARDWAREBUFFER_USAGE_CPU_READ_MASK |
        AHARDWAREBUFFER_USAGE_CPU_WRITE_MASK)) {
      std::string exception =
          "Invalid usage flags passed to AHardwareBuffer_lock; only  AHARDWAREBUFFER_USAGE_CPU_* flags are allowed";
      throwException(env, exception);
      return;
    }

    void *bufferData = nullptr;
    auto lockingResult = AHardwareBuffer_lock_compat(buffer_,
                                                     AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN,
                                                     -1,
                                                     nullptr,
                                                     &bufferData);
    if (lockingResult != 0) {
      std::string exception =
          "Can't acquire access for a hardware buffer";
      throwException(env, exception);
      return;
    }

    if (AHardwareBuffer_unlock_compat(buffer_, nullptr) != 0) {
      std::string exception =
          "Can't properly release Hardware buffer";
      throwException(env, exception);
      return;
    }

    ALOGI("Buffer format %d", desc.format);
  }

};

#endif //JPEGLICODER_JPEGLI_CODER_SRC_MAIN_CPP_HARDWAREBITMAP_H_

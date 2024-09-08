/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#include <jni.h>
#include <string>
#include "android/bitmap.h"
#include "JniException.h"
#include <cstdint>
#include <vector>
#include <types.h>
#include "RgbaToRgb.h"
#include "Rgb565.h"
#include "RgbaF16ToRgb.h"
#include "Rgb1010102.h"
#include "JPEGEncoder.h"
#include "OutputStreamAdapter.h"

extern "C"
JNIEXPORT void JNICALL
Java_io_github_awxkee_jpegli_coder_JpegliCoder_00024Companion_compressImpl(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jobject outputStream,
                                                                           jobject bitmap,
                                                                           jint quality,
                                                                           jint strategy,
                                                                           jint replacementR,
                                                                           jint replacementG,
                                                                           jint replacementB,
                                                                           jboolean progressive) {
  try {
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
      throwPixelsException(env);
      return;
    }

    if (info.flags & ANDROID_BITMAP_FLAGS_IS_HARDWARE) {
      throwHardwareBitmapException(env);
      return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 &&
        info.format != ANDROID_BITMAP_FORMAT_RGB_565 &&
        info.format != ANDROID_BITMAP_FORMAT_RGBA_F16 &&
        info.format != ANDROID_BITMAP_FORMAT_RGBA_1010102) {
      throwInvalidPixelsFormat(env);
      return;
    }

    void *addr;
    if (AndroidBitmap_lockPixels(env, bitmap, &addr) != 0) {
      throwPixelsException(env);
      return;
    }

    uint32_t width = info.width;
    uint32_t height = info.height;
    uint32_t newStride = info.width * 3;
    Scalar scalar{static_cast<uint8_t>(replacementR),
                  static_cast<uint8_t>(replacementG),
                  static_cast<uint8_t>(replacementB)};

    auto sourceImage = addr;

    std::vector<uint8_t> sourceData(info.height * newStride);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
      coder::RgbaToRgb(reinterpret_cast<uint8_t *>(sourceImage),
                       info.stride,
                       sourceData.data(),
                       newStride,
                       width,
                       height,
                       scalar);
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
      coder::Rgb565ToRGB(reinterpret_cast<uint16_t *>(sourceImage),
                         info.stride,
                         sourceData.data(),
                         newStride,
                         width,
                         height);
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_F16) {
      coder::RgbaF16ToRgb(reinterpret_cast<uint16_t *>(sourceImage),
                          info.stride,
                          sourceData.data(),
                          newStride,
                          width,
                          height,
                          8,
                          scalar);
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_1010102) {
      coder::Rgba1010102ToRgb(reinterpret_cast<uint8_t *>(sourceImage),
                              info.stride,
                              sourceData.data(),
                              newStride,
                              width,
                              height,
                              scalar);
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    auto encoder = coder::JPEGLIEncoder(sourceData.data(),
                                        newStride,
                                        width,
                                        height,
                                        strategy == 1,
                                        progressive);
    encoder.setQuality(quality);
    auto encodedData = encoder.encode();

    OutputStreamAdapter adapter(env, outputStream);

    size_t chunkSize = 4096;
    size_t chunksCount = encodedData.size() / 4096;
    size_t remainder = encodedData.size() - chunksCount * chunkSize;

    for (size_t chunk = 0; chunk < chunksCount; ++chunk) {
      auto srcPtr = encodedData.data() + chunkSize * chunk;
      adapter.write(reinterpret_cast<const char *>(srcPtr), static_cast<jint>(chunkSize));
    }

    if (remainder > 0) {
      auto srcPtr = encodedData.data() + chunkSize * chunksCount;
      adapter.write(reinterpret_cast<const char *>(srcPtr), static_cast<jint>(remainder));
    }

    adapter.flush();
    adapter.close();
  } catch (std::bad_alloc &err) {
    std::string exception = "Not enough memory to encode this image";
    throwException(env, exception);
  } catch (EncoderError &err) {
    std::string exception(err.what());
    throwException(env, exception);
  }
}
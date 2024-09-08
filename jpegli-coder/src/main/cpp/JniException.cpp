/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#include "JniException.h"

jint throwHardwareBitmapException(JNIEnv *env) {
  jclass exClass;
  exClass =
      env->FindClass("java/lang/Exception");
  return env->ThrowNew(exClass,
                       "This hardware bitmap configuration is not supported. See method info to find out more");
}

jint throwInvalidPixelsFormat(JNIEnv *env) {
  jclass exClass;
  exClass = env->FindClass("java/lang/Exception");
  return env->ThrowNew(exClass,
                       "Unsupported pixel format is used. See method info to find out more");
}

jint throwPixelsException(JNIEnv *env) {
  jclass exClass;
  exClass = env->FindClass("java/lang/Exception");
  return env->ThrowNew(exClass, "Can't retrieve image data");
}

jint throwException(JNIEnv *env, std::string &msg) {
  jclass exClass;
  exClass = env->FindClass("java/lang/Exception");
  return env->ThrowNew(exClass, msg.c_str());
}

int androidOSVersion() {
  return android_get_device_api_level();
}
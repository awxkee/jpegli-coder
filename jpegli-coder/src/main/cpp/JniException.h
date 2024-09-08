/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#ifndef AVIF_JNIEXCEPTION_H
#define AVIF_JNIEXCEPTION_H

#include <jni.h>
#include <string>

jint throwBitDepthException(JNIEnv *env);

jint throwInvalidPixelsFormat(JNIEnv *env);

jint throwPixelsException(JNIEnv *env);

jint throwHardwareBitmapException(JNIEnv *env);

jint throwException(JNIEnv *env, std::string &msg);

int androidOSVersion();

#endif //AVIF_JNIEXCEPTION_H

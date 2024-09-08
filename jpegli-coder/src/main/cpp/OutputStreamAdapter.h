/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#pragma once

#include <jni.h>
#include <android/log.h>
#include "types.h"

#define LOG_TAG "OutputStreamAdapter"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class OutputStreamAdapter {
 public:
  OutputStreamAdapter(JNIEnv *env, jobject outputStream);
  ~OutputStreamAdapter();

  void write(const char *data, jint length);

  void flush();

  void close();

 private:
  JNIEnv *env_;
  jobject outputStream_;  // Java OutputStream reference
  jclass outputStreamClass_;
  jmethodID writeMethod_;
  jmethodID flushMethod_;
  jmethodID closeMethod_;
};

OutputStreamAdapter::OutputStreamAdapter(JNIEnv *env, jobject outputStream)
    : env_(env), outputStream_(env->NewGlobalRef(outputStream)) {

  // Get the OutputStream class and methods
  outputStreamClass_ = env->FindClass("java/io/OutputStream");
  if (!outputStreamClass_) {
    LOGE("Failed to find OutputStream class.");
    throw EncoderError("Failed to find OutputStream class.");
    return;
  }

  // Get the write(byte[] b, int off, int len) method ID
  writeMethod_ = env->GetMethodID(outputStreamClass_, "write", "([BII)V");
  if (!writeMethod_) {
    LOGE("Failed to find OutputStream.write() method.");
    throw EncoderError("Failed to find OutputStream.write() method.");
    return;
  }

  // Get the flush() method ID
  flushMethod_ = env->GetMethodID(outputStreamClass_, "flush", "()V");
  if (!flushMethod_) {
    LOGE("Failed to find OutputStream.flush() method.");
    throw EncoderError("Failed to find OutputStream.flush() method.");
    return;
  }

  closeMethod_ = env->GetMethodID(outputStreamClass_, "close", "()V");
  if (!closeMethod_) {
    LOGE("Failed to find OutputStream.close() method.");
    throw EncoderError("Failed to find OutputStream.close() method.");
    return;
  }
}

OutputStreamAdapter::~OutputStreamAdapter() {
  if (outputStream_) {
    env_->DeleteGlobalRef(outputStream_);
  }
}

// Write method: Writes data to the Java OutputStream
void OutputStreamAdapter::write(const char *data, jint length) {
  if (!writeMethod_) {
    LOGE("writeMethod is null, cannot write to OutputStream.");
    throw EncoderError("writeMethod is null, cannot write to OutputStream.");
    return;
  }

  // Create a byte array for the data
  jbyteArray byteArray = env_->NewByteArray(length);
  env_->SetByteArrayRegion(byteArray, 0, length, reinterpret_cast<const jbyte *>(data));

  // Call the write method on the Java OutputStream
  env_->CallVoidMethod(outputStream_, writeMethod_, byteArray, 0, length);

  // Check for exceptions
  if (env_->ExceptionCheck()) {
    env_->ExceptionDescribe();
    env_->ExceptionClear();
    LOGE("Exception occurred while writing to the OutputStream.");
    throw EncoderError("Java Exception occurred while writing to the OutputStream.");
    return;
  }

  // Clean up the local reference
  env_->DeleteLocalRef(byteArray);
}

// Flush method: Flushes the Java OutputStream
void OutputStreamAdapter::flush() {
  if (!flushMethod_) {
    LOGE("flushMethod is null, cannot flush OutputStream.");
    throw EncoderError("flushMethod is null, cannot flush OutputStream.");
    return;
  }

  // Call the flush method on the Java OutputStream
  env_->CallVoidMethod(outputStream_, flushMethod_);

  // Check for exceptions
  if (env_->ExceptionCheck()) {
    env_->ExceptionDescribe();
    env_->ExceptionClear();
    LOGE("Exception occurred while flushing the OutputStream.");
    throw EncoderError("Java Exception occurred while writing to the OutputStream.");
    return;
  }

}

// Close method: Closes the Java OutputStream
void OutputStreamAdapter::close() {
  if (!flushMethod_) {
    LOGE("flushMethod is null, cannot close OutputStream.");
    throw EncoderError("flushMethod is null, cannot close OutputStream.");
    return;
  }

  // Call the flush method on the Java OutputStream
  env_->CallVoidMethod(outputStream_, closeMethod_);

  // Check for exceptions
  if (env_->ExceptionCheck()) {
    env_->ExceptionDescribe();
    env_->ExceptionClear();
    LOGE("Exception occurred while flushing the OutputStream.");
    throw EncoderError("Java Exception occurred while writing to the OutputStream.");
    return;
  }
}
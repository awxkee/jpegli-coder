/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

#include "JPEGEncoder.h"
#include "jpegli/jpeglib.h"
#include <setjmp.h> // NOLINT(*-deprecated-headers)
#include "hwy/highway.h"
#include "jpegli/encode.h"
#include <string>
#include <types.h>

using namespace hwy;
using namespace hwy::HWY_NAMESPACE;

namespace coder {

struct coder_jpeg_error_mng {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

struct stdvector_destination_mgr {
  struct jpeg_destination_mgr pub;        // public fields
  std::vector<uint8_t> *vec = nullptr;  // destination vector
};

void init_stdvector_destination(j_compress_ptr /*cinfo*/) {
  // Nothing to do
}

boolean
empty_stdvector_output_buffer(j_compress_ptr cinfo) {
  auto *dest = reinterpret_cast< stdvector_destination_mgr * >( cinfo->dest );

  // Double vector capacity
  const auto currentSize = dest->vec->size();
  dest->vec->resize(currentSize * 2);

  // Point to newly allocated data
  dest->pub.next_output_byte = dest->vec->data() + currentSize;
  dest->pub.free_in_buffer = currentSize;

  return TRUE;
}

void
term_stdvector_destination(j_compress_ptr cinfo) {
  auto *dest = reinterpret_cast< stdvector_destination_mgr * >( cinfo->dest );

  // Resize vector to number of bytes actually used
  const auto used_bytes = dest->vec->capacity() - dest->pub.free_in_buffer;
  dest->vec->resize(used_bytes);
}

void jpeg_custom_dest(j_compress_ptr cinfo, std::vector<unsigned char> &vec) {
  if (cinfo->dest == nullptr) {
    cinfo->dest = (struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small)((j_common_ptr) cinfo,
                                                                             JPOOL_PERMANENT,
                                                                             sizeof(stdvector_destination_mgr));
  }

  auto *dest = reinterpret_cast< stdvector_destination_mgr * >( cinfo->dest );
  dest->pub.init_destination = init_stdvector_destination;
  dest->pub.empty_output_buffer = empty_stdvector_output_buffer;
  dest->pub.term_destination = term_stdvector_destination;

  // Set output buffer and initial size
  dest->vec = &vec;
  dest->vec->resize(4096);

  // Initialize public buffer ptr and size
  dest->pub.next_output_byte = dest->vec->data();
  dest->pub.free_in_buffer = dest->vec->size();
}

METHODDEF(void)
handleJpegError(j_common_ptr cinfo) {
  coder_jpeg_error_mng *myerr = (coder_jpeg_error_mng *) cinfo->err;
  longjmp(myerr->setjmp_buffer, 1);
}

std::vector<uint8_t> JPEGLIEncoder::encode() {
  struct jpeg_compress_struct cinfo = {0};
  struct coder_jpeg_error_mng jerr = {0};

  cinfo.err = jpegli_std_error(reinterpret_cast<jpeg_error_mgr *>(&jerr));
  jerr.pub.error_exit = handleJpegError;

  jpegli_create_compress(&cinfo);

  std::vector<uint8_t> output;

  jpeg_custom_dest(&cinfo, output);

//  jpegli_mem_dest(&cinfo, &outputBuffer, &outputSize);

  if (setjmp(jerr.setjmp_buffer)) {
    jpegli_destroy_compress(&cinfo);
    std::string msg("JPEG compression has failed");
    throw EncoderError(msg);
  }

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  if (useXYB) {
    jpegli_set_xyb_mode(&cinfo);
  }

  jpegli_set_defaults(&cinfo);
  jpegli_set_quality(&cinfo, quality, TRUE);
  if (isProgressive) {
    jpegli_simple_progression(&cinfo);
    jpegli_set_progressive_level(&cinfo, 2);
  } else {
    jpegli_set_progressive_level(&cinfo, 0);
  }

  jpegli_start_compress(&cinfo, TRUE);

  JSAMPROW row_pointer;

  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer =
        reinterpret_cast<uint8_t *>(this->data) + cinfo.next_scanline * this->stride;
    jpegli_write_scanlines(&cinfo, &row_pointer, 1);
  }

  jpegli_finish_compress(&cinfo);
  jpegli_destroy_compress(&cinfo);

  return output;
}
}
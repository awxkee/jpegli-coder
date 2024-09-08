/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

package io.github.awxkee.jpegli.coder

import android.graphics.Bitmap
import androidx.annotation.IntRange
import androidx.annotation.Keep
import java.io.OutputStream

@Keep
class JpegliCoder {

    companion object {
        init {
            System.loadLibrary("jpeglicoder")
        }

        external fun compressImpl(
            outputStream: OutputStream,
            bitmap: Bitmap,
            quality: Int,
            strategy: Int,
            replacementR: Int,
            replacementG: Int,
            replacementB: Int,
            progressive: Boolean
        )

        /**
         * Compresses a [Bitmap] into a specified quality and writes the result to the given [OutputStream].
         *
         * This method allows you to compress a bitmap image to a desired quality level. The compression is
         * handled using the specified [IccStrategy] and allows for optional background color and progressive
         * compression.
         * Supports [Bitmap.Config] is [Bitmap.Config.ARGB_8888], [Bitmap.Config.RGBA_F16], [Bitmap.Config.RGBA_1010102], [Bitmap.Config.RGB_565]
         *
         * @param bitmap The [Bitmap] object that you want to compress.
         * @param quality The quality of the compressed image, represented as an integer between 0 and 100.
         *                A higher value indicates better quality but larger file size. A lower value results
         *                in more compression and smaller file size. The default value is 100 (best quality).
         * @param strategy The [IccStrategy] used for image compression. It determines the color profile
         *                 and compression technique. Default is [IccStrategy.XYB].
         * @param background The background color to use if the bitmap has transparency. This is represented
         *                   as a [Scalar] object, with the default being white (255, 255, 255).
         * @param progressive If `true`, the image will be saved in progressive format, which allows for
         *                     incremental rendering. Default is `false`.
         * @param outputStream The [OutputStream] where the compressed image will be written.
         *
         * @throws IOException If an error occurs while writing to the [OutputStream].
         * @throws IllegalArgumentException If the quality parameter is not within the range of 0 to 100.
         *
         * @see IccStrategy
         * @see Scalar
         */
        fun compress(
            bitmap: Bitmap,
            @IntRange(from = 0, to = 100) quality: Int,
            strategy: IccStrategy = IccStrategy.XYB,
            background: Scalar = Scalar(255),
            progressive: Boolean = false,
            outputStream: OutputStream,
        ) {
            if (quality < 0 && quality > 100) {
                throw IllegalArgumentException("Invalid quality level")
            }
            compressImpl(
                outputStream,
                bitmap,
                quality,
                strategy.value,
                background.red,
                background.green,
                background.blue,
                progressive,
            )
        }
    }
}
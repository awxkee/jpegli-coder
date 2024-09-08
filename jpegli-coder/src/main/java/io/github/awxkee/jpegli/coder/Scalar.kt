/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

package io.github.awxkee.jpegli.coder

import androidx.annotation.Keep

/**
 * Data class representing an RGB color with red, green, and blue components.
 *
 * This class is used to encapsulate a color value using its red, green, and blue color components,
 * each represented as an integer.
 *
 * @property red The red component of the color, represented as an integer. Expected range is 0-255.
 * @property green The green component of the color, represented as an integer. Expected range is 0-255.
 * @property blue The blue component of the color, represented as an integer. Expected range is 0-255.
 *
 * @constructor Creates an instance of [Scalar] with the specified red, green, and blue components.
 *
 * @throws IllegalArgumentException If any of the color components are not within the range of 0-255.
 */
@Keep
data class Scalar(val red: Int, val green: Int, val blue: Int) {

    constructor(ones: Int) : this(ones, ones, ones)

    init {
        require(red in 0..255) { "Red component must be between 0 and 255" }
        require(green in 0..255) { "Green component must be between 0 and 255" }
        require(blue in 0..255) { "Blue component must be between 0 and 255" }
    }

    companion object {
        val ZERO = Scalar(0)
    }
}
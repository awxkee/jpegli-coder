/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

package io.github.awxkee.jpegli.coder

import androidx.annotation.Keep

/**
 * Enum class representing different ICC (International Color Consortium) strategies for image
 * compression.
 *
 * The `IccStrategy` enum defines strategies for how color profiles are managed during image
 * compression. Each strategy has a unique integer value associated with it.
 *
 * @property value The integer value associated with the ICC strategy.
 *
 * @constructor Creates an instance of [IccStrategy] with the given integer value.
 *
 * @see [DEFAULT] for the default ICC strategy.
 * @see [XYB] for an alternative ICC strategy.
 */
@Keep
enum class IccStrategy(val value: Int) {
    /**
     * Default ICC strategy, represented by the integer value 0.
     * This is the standard strategy to ignore ColorSpace attached to bitmap
     */
    DEFAULT(0),
    /**
     * Alternative ICC strategy, represented by the integer value 1.
     * This strategy might be used when you wish to use specific Jpegli `XYB` feature
     */
    XYB(1)
}
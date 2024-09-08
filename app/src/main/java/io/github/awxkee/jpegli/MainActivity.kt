/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

package io.github.awxkee.jpegli

import android.annotation.SuppressLint
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.ImageDecoder
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.tooling.preview.Preview
import io.github.awxkee.jpegli.coder.IccStrategy
import io.github.awxkee.jpegli.coder.JpegliCoder
import io.github.awxkee.jpegli.coder.compressJpegli
import io.github.awxkee.jpegli.ui.theme.JpegliCoderTheme
import java.io.ByteArrayOutputStream

class MainActivity : ComponentActivity() {
    @SuppressLint("NewApi", "WrongThread")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

//        val bitmap = BitmapFactory.decodeResource(this.resources, R.drawable.pexels).copy(Bitmap.Config.HARDWARE, false)

        val source = ImageDecoder.createSource(resources, R.drawable.pexels)

        // Decode the image with options to use hardware acceleration
        val bitmap = ImageDecoder.decodeBitmap(source) { decoder, info, source ->
            decoder.setAllocator(ImageDecoder.ALLOCATOR_HARDWARE)
        }

        val bos = ByteArrayOutputStream()
        JpegliCoder.compress(bitmap, 70, strategy = IccStrategy.XYB, progressive = true, outputStream = bos)

        val encoded = bos.toByteArray()
        val decoded = BitmapFactory.decodeByteArray(encoded, 0, encoded.size)

        setContent {
            JpegliCoderTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Greeting(
                        name = "Android",
                        modifier = Modifier.padding(innerPadding)
                    )
                    Image(
                        bitmap = decoded.asImageBitmap(),
                        contentDescription = "some useful description",
                    )
                }
            }
        }
    }
}

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello $name!",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    JpegliCoderTheme {
        Greeting("Android")
    }
}
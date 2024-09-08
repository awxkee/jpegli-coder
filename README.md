# Jpegli encoder for android

Supports version from Android 5.0 (API Level 21)

Image processing speeded up by [libhwy](https://github.com/google/highway)

# Usage example

```kotlin
val bos = ByteArrayOutputStream()
JpegliCoder.compress(bitmap, 70, strategy = IccStrategy.XYB, progressive = true, outputStream = bos)
// Convention
val bos = ByteArrayOutputStream()
bitmap.compressJpegli(outputStream = bos, quality = 70)
```

# Add to project

Add jitpack repository

```groovy
repositories {
    maven { url "https://jitpack.io" }
}
```

```groovy
implementation 'com.github.awxkee:jpegli-coder:1.0.0' // or any version above picker from release tags
```

# Copyrights

This library created with [`libjxl`](https://github.com/libjxl/libjxl/tree/main) which belongs to
JPEG XL Project
Authors which licensed with BSD-3 license

### This project is licensed under either of

- BSD-3-Clause License (see [LICENSE](LICENSE.md))
- Apache License, Version 2.0 (see [LICENSE](LICENSE-APACHE.md))

at your option.
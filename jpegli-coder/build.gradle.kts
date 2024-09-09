import com.vanniktech.maven.publish.AndroidMultiVariantLibrary

/*
 * // Copyright 2024 (c) the Radzivon Bartoshyk. All rights reserved.
 * //
 * // Use of this source code is governed by a BSD-style
 * // license that can be found in the LICENSE file.
 */

plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.kotlin.android)
    id("maven-publish")
    id("signing")
    alias(libs.plugins.vanniktech.publish)
}

task("androidSourcesJar", Jar::class) {
    archiveClassifier.set("sources")
    from(android.sourceSets.getByName("main").java.srcDirs)
}

afterEvaluate {
    publishing {
        publications {
            create<MavenPublication>("mavenJava") {
                groupId = "com.github.awxkee"
                artifactId = "jpegli-coder"
                version = "1.0.0"
                from(components["release"])
//                artifact("androidSourcesJar")
            }
        }
    }
}

mavenPublishing {
    configure(
        AndroidMultiVariantLibrary(
            sourcesJar = true,
            publishJavadocJar = true,
        )
    )

    coordinates("io.github.awxkee", "jpegli-coder", System.getenv("VERSION_NAME") ?: "1.0.0")

    pom {
        name.set("Jpegli Coder")
        description.set("Jpegli encoder for Android")
        inceptionYear.set("2024")
        url.set("https://github.com/awxkee/jpegli-coder")
        licenses {
            license {
                name.set("The Apache License, Version 2.0")
                url.set("http://www.apache.org/licenses/LICENSE-2.0.txt")
                distribution.set("http://www.apache.org/licenses/LICENSE-2.0.txt")
            }
            license {
                name.set("The 3-Clause BSD License")
                url.set("https://opensource.org/license/bsd-3-clause")
                description.set("https://opensource.org/license/bsd-3-clause")
            }
        }
        developers {
            developer {
                id.set("awxkee")
                name.set("Radzivon Bartoshyk")
                url.set("https://github.com/awxkee")
                email.set("radzivon.bartoshyk@proton.me")
            }
        }
        scm {
            url.set("https://github.com/awxkee/jpegli-coder")
            connection.set("scm:git:git@github.com:awxkee/jpegli-coder.git")
            developerConnection.set("scm:git:ssh://git@github.com/awxkee/jpegli-coder.git")
        }
    }
}

android {
    publishing {
        singleVariant("release") {
            withSourcesJar()
            withJavadocJar()
        }
    }

    namespace = "io.github.awxkee.jpegli.coder"
    compileSdk = 34

    defaultConfig {
        minSdk = 21

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        consumerProguardFiles("consumer-rules.pro")
        externalNativeBuild {
            cmake {
                cppFlags("-std=c++17")
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    externalNativeBuild {
        cmake {
            path("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}
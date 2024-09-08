cmake_minimum_required(VERSION 3.22.1)

project("jpeglicoder")

add_library(libjpegli STATIC IMPORTED)

add_library(${CMAKE_PROJECT_NAME} SHARED
        JPEGEncoder.cpp
        Rgb565.cpp
        half.cpp
        RgbaToRgb.cpp
        RgbaF16ToRgb.cpp
        Rgb1010102.cpp
        JniException.cpp
        hwy/aligned_allocator.cc
        hwy/per_target.cc
        hwy/timer.cc
        hwy/targets.cc
        coder.cpp)

set_target_properties(jpeglicoder libjpegli PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libjpegli-static.a)

target_include_directories(jpeglicoder PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/jpegli)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DHWY_COMPILE_ONLY_STATIC)
endif ()

target_link_libraries(${CMAKE_PROJECT_NAME}
        libjpegli
        android
        -ljnigraphics
        log)
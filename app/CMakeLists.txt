# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#设置第三方动态库的头文件路径
set(lib_include_DIR ${CMAKE_SOURCE_DIR}/src/main/cpp/include)

#设置第三方动态库的路径
set(lib_src_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})

#导入第三方库的头文件目录
#include_directories(${lib_include_DIR})
include_directories(
        ${CMAKE_SOURCE_DIR}/src/main/cpp/include
)

#添加第三方动态库依赖
add_library(avcodec-57_lib SHARED IMPORTED)
set_target_properties(avcodec-57_lib PROPERTIES IMPORTED_LOCATION
                             ${lib_src_DIR}/libavcodec-57.so)

#添加第三方动态库依赖
add_library(avdevice-57_lib SHARED IMPORTED)
set_target_properties(avdevice-57_lib PROPERTIES IMPORTED_LOCATION
                             ${lib_src_DIR}/libavdevice-57.so)

#添加第三方动态库依赖
add_library(avfilter-57_lib SHARED IMPORTED)
set_target_properties(avfilter-57_lib PROPERTIES IMPORTED_LOCATION
                             ${lib_src_DIR}/libavfilter-6.so)

#添加第三方动态库依赖
add_library(avformat-57_lib SHARED IMPORTED)
set_target_properties(avformat-57_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavformat-57.so)

#添加第三方动态库依赖
add_library(avutil-55_lib SHARED IMPORTED)
set_target_properties(avutil-55_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavutil-55.so)

#添加第三方动态库依赖
add_library(postproc-55_lib SHARED IMPORTED)
set_target_properties(postproc-55_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libpostproc-54.so)

#添加第三方动态库依赖
add_library(swresample-2_lib SHARED IMPORTED)
set_target_properties(swresample-2_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswresample-2.so)

#添加第三方动态库依赖
add_library(swscale-4_lib SHARED IMPORTED)
set_target_properties(swscale-4_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswscale-4.so)


add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).

               src/main/cpp/pushvideo.cpp
               )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib
android
OpenSLES
avcodec-57_lib
avdevice-57_lib
avfilter-57_lib
avformat-57_lib
avutil-55_lib
postproc-55_lib
swresample-2_lib
swscale-4_lib

                       ${log-lib} )
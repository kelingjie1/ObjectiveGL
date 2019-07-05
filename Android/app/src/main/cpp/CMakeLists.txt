# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

file(GLOB_RECURSE SRC_FILES
        *.cpp)
#        ${PROJECT_ROOT}/include/*.cpp)
#        ${PROJECT_ROOT}/Source/platform/android/*.cpp)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             objective-gl

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
            ${SRC_FILES} )

target_include_directories(
        objective-gl
        PRIVATE
        "${PROJECT_ROOT}/include/ObjectiveGL")

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        android-lib
        android)

find_library( # Sets the name of the path variable.
        log-lib
        log)

find_library( # Sets the name of the path variable.
        egl-lib
        EGL)

find_library( # Sets the name of the path variable.
        gles-lib
        GLESv3)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       objective-gl

                       # Links the target library to the log library
                       # included in the NDK.
                        ${android-lib}
                        ${log-lib}
                        ${egl-lib}
                        ${gles-lib})
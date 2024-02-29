#!/bin/sh

# Check if the number of arguments is greater than expected
if [ "$#" -gt 3 ]; then
    echo "error: too many arguments"
    echo "usage: GenerateProjectAndroid.sh [INSTALL_DIR [BUILD_DIR [SOURCE_DIR]]]"
    exit 1
fi

# Verify that the ANDROID_NDK_HOME environment variable is set
if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "error: environment variable 'ANDROID_NDK_HOME' not set"
    exit 1
fi

# Default values
ANDROID_API_LEVEL=21
SOURCE_DIR="."
BUILD_DIR="build_android"
INSTALL_DIR=$(pwd)  # Default to current directory's absolute path
echo $INSTALL_DIR

# Assign optional arguments
if [ "$#" -ge 1 ]; then
    # Convert INSTALL_DIR to an absolute path
    INSTALL_DIR=$(realpath "$1")
fi

if [ "$#" -ge 2 ]; then
    BUILD_DIR=$2
fi

if [ "$#" -ge 3 ]; then
    SOURCE_DIR=$3
fi

if command -v cygpath >/dev/null 2>&1; then
    INSTALL_DIR=$(cygpath -w "$INSTALL_DIR")
fi

ANDROID_CMAKE_TOOLCHAIN="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake"

# Validate the presence of the CMake toolchain file
if [ ! -f "$ANDROID_CMAKE_TOOLCHAIN" ]; then
    echo "error: CMake toolchain not found: ${ANDROID_CMAKE_TOOLCHAIN}"
    exit 1
fi

# Ensure the source directory contains a CMakeLists.txt file
if [ ! -f "$SOURCE_DIR/CMakeLists.txt" ]; then
    echo "error: CMakeLists.txt not found: $SOURCE_DIR/CMakeLists.txt"
    exit 1
fi

# Define architectures to build for
ARCHS="arm64-v8a armeabi-v7a"

for ARCH in $ARCHS; do
    # Set and create the architecture-specific build directory
    ARCH_BUILD_DIR="${BUILD_DIR}_${ARCH}"
    if [ ! -d "$ARCH_BUILD_DIR" ]; then
        mkdir "$ARCH_BUILD_DIR"
    fi

    # Compute the relative path to the source directory
    RELATIVE_SOURCE_DIR=$(realpath --relative-to="$ARCH_BUILD_DIR" "$SOURCE_DIR")

    # Change directory to the build directory and execute cmake with the configured parameters
    (cd "$ARCH_BUILD_DIR";

    cmake "$RELATIVE_SOURCE_DIR" \
        -DCMAKE_TOOLCHAIN_FILE="$ANDROID_CMAKE_TOOLCHAIN" \
        -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
        -DANDROID_ABI=$ARCH \
        -DANDROID_PLATFORM=$ANDROID_API_LEVEL \
        -DANDROID_STL=c++_shared \
        -DANDROID_CPP_FEATURES="rtti exceptions" \
        -DGIRAFFE_BUILD_RENDERER_OPENGLES3=ON \
        -DGIRAFFE_BUILD_TESTS=ON \
        -DGIRAFFE_BUILD_EXAMPLES=ON \
        -DGIRAFFE_BUILD_STATIC_LIB=OFF \
        -DLLGL_BUILD_RENDERER_NULL=OFF \
        -DGIRAFFE_TARGET_PLATFORM=Android \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
        -DARCH_INSTALL_DIR=$ARCH \
        -DNOMINMAX=ON \
        -G "Unix Makefiles"
    )
done

#!/bin/bash

export UNITY_BASE="/g/Programs/Unity/2023.3.0a18"

export NDK=/g/Programs/AndroidNDK/android-ndk-r26b-windows/android-ndk-r26b
export NDK_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake
export NDK_MAKE_PROGRAM=$UNITY_BASE/Editor/Data/PlaybackEngines/AndroidPlayer/SDK/cmake/3.22.1/bin/ninja

cmake -Bandroid_build . \
  -DCMAKE_CROSSCOMPILING=True \
  -DCMAKE_TOOLCHAIN_FILE=$NDK_TOOLCHAIN_FILE \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-23 \
	-DLLVM_DEFAULT_TARGET_TRIPLE=aarch64-linux-gnu \
  -DCMAKE_MAKE_PROGRAM=$NDK_MAKE_PROGRAM  \
  -DPIP2_INCLUDE_TESTS=OFF  \
  -GNinja

cmake --build android_build
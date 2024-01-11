#!/bin/bash

export UNITY_BASE="/g/Programs/Unity/2023.3.0a18"

export NDK=$UNITY_BASE/Editor/Data/PlaybackEngines/AndroidPlayer/NDK
export NDK_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake
export NDK_MAKE_PROGRAM=$UNITY_BASE/Editor/Data/PlaybackEngines/AndroidPlayer/SDK/cmake/3.22.1/bin/ninja

export HOST_COMPILER_BASE=/g/Programs/llvm-mingw/bin

export CC=$HOST_COMPILER_BASE/x86_64-w64-mingw32-clang
export CXX=$HOST_COMPILER_BASE/x86_64-w64-mingw32-clang++

export LLVM_PATH=../llvm-project/llvm

# https://github.com/Jman420/llvm_for_android/blob/develop/build_llvm_android.sh#L19

PatchableSourceFiles=( "$LLVM_PATH/lib/Transforms/CMakeLists.txt" )
SourcePatchReplacePatterns=( "^add_subdirectory(Hello)$" )
SourcePatchReplaceValues=( "#add_subdirectory(Hello)" )

echo "Patching LLVM Source Files..."
PatchesLength=${#PatchableSourceFiles[*]}

for (( patchCounter=0; patchCounter < $PatchesLength; patchCounter++ ))
do
    patchFile=${PatchableSourceFiles[$patchCounter]}
    replacePattern=${SourcePatchReplacePatterns[$patchCounter]}
    replaceValue=${SourcePatchReplaceValues[$patchCounter]}
    
    echo "Patching Source File : $patchFile ..."
    if [ ! -f "$patchFile.orig" ]
    then
        cp "$patchFile" "$patchFile.orig"
    fi
    
    echo "Replacing $replacePattern with $replaceValue ..."
    sed -i "s/$replacePattern/$replaceValue/" "$patchFile"
    echo "Successfully patched Source File : $patchFile !"
done

cmake -Bbuild $LLVM_PATH -DCMAKE_CROSSCOMPILING=True -DCMAKE_TOOLCHAIN_FILE=$NDK_TOOLCHAIN_FILE \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-23 \
	-DLLVM_DEFAULT_TARGET_TRIPLE=aarch64-linux-gnu \
	-DLLVM_TARGET_ARCH=AArch64 \
	-DLLVM_TARGETS_TO_BUILD=AArch64 \
  -DCMAKE_MAKE_PROGRAM=$NDK_MAKE_PROGRAM  \
  -DCMAKE_C_COMPILER=$CC  \
  -DCMAKE_CXX_COMPILER=$CXX \
  -DLLVM_CCACHE_BUILD=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_INCLUDE_DOCS=OFF \
  -DLLVM_INCLUDE_BENCHMARKS=OFF \
  -DLLVM_INCLUDE_TESTS=OFF \
  -DLLVM_BUILD_EXAMPLES=OFF \
  -DLLVM_BUILD_BENCHMARKS=OFF \
  -DLLVM_BUILD_TESTS=OFF \
  -DLLVM_BUILD_LLVM_DYLIB=OFF \
  -DLLVM_HOST_TRIPLE=aarch64-linux-android \
  -DLLVM_INCLUDE_TOOLS=OFF \
  -DLLVM_BUILD_TOOLS=OFF \
  -DLLVM_INCLUDE_EXAMPLES=OFF \
  -DLLVM_BUILD_EXAMPLES=OFF \
  -DLLVM_ENABLE_RTTI=ON \
  -GNinja
  
cmake --build build
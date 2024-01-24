add_library(llvm INTERFACE)

if (ANDROID)
  set(LLVM_LIBS_PATH lib/android-arm64-v8a)
  
  file(GLOB llvm_LIBS "${LLVM_LIBS_PATH}/*.a")
  target_include_directories(llvm INTERFACE include/android-arm64-v8a)
  
  target_link_libraries(llvm INTERFACE ${llvm_LIBS} z)
elseif (WINDOWS)
  set(LLVM_LIBS_PATH lib/windows-x86_64)
  
  file(GLOB llvm_LIBS "${LLVM_LIBS_PATH}/*.lib")
  target_include_directories(llvm INTERFACE include/windows-x86_64)

  target_link_libraries(llvm INTERFACE ${llvm_LIBS} zlib_std)
endif()
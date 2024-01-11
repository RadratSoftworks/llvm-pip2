cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(llvm-pip2)
option(PIP2_INCLUDE_TESTS "Include tests" OFF)

if (ANDROID)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")
  add_definitions(-DLLVM_ON_UNIX=1 -DPIP2_ANDROID=1)
endif()

add_subdirectory(3rdparty)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

if (PIP2_INCLUDE_TESTS)
  add_subdirectory(tests)
endif()
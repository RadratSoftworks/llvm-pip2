add_library(zlib_std INTERFACE)

target_link_libraries(zlib_std INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/z.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/zstd.lib)
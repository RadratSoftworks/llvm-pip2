add_subdirectory(zlib_std)
add_subdirectory(llvm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(json)

set(CATCH_DEVELOPMENT_BUILD OFF CACHE INTERNAL "")
add_subdirectory(Catch2)
add_subdirectory(zlib_std)
add_subdirectory(llvm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(json)

if (PIP2_INCLUDE_TESTS)
	set(CATCH_DEVELOPMENT_BUILD OFF CACHE INTERNAL "")
	add_subdirectory(Catch2)
endif()

add_library(libco STATIC
	libco/libco.c)

target_include_directories(libco PUBLIC libco)
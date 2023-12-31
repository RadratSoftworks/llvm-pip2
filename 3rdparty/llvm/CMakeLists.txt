add_library(llvm INTERFACE)
target_include_directories(llvm INTERFACE include)

file(GLOB llvm_LIBS "lib/*.lib")
target_link_libraries(llvm INTERFACE ${llvm_LIBS} zlib_std)
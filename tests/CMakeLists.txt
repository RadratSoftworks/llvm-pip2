add_executable(TestEmit TestEmit.cpp)
add_executable(TestJitBasic TestJitBasic.cpp)

target_link_libraries(TestEmit PRIVATE llvm-pip2 llvm nlohmann_json::nlohmann_json)
target_link_libraries(TestJitBasic PRIVATE llvm-pip2 llvm)
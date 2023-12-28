add_executable(TestAnalysis TestAnalysis.cpp)

target_link_libraries(TestAnalysis PRIVATE llvm-pip2 nlohmann_json::nlohmann_json)
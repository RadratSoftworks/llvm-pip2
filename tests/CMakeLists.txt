add_executable(TestEmit TestEmit.cpp)
add_executable(TestJitBasic TestJitBasic.cpp)

add_executable(TestUnits
        units/TestEnvironment.cpp
        units/TestEnvironment.h
        units/ModifiablePoolItems.cpp
        units/ModifiablePoolItems.h
        units/TestArithmetic.cpp
        units/RandomIntGenerator.h
        units/TestMiscs.cpp
        units/TestLoadStore.cpp
)

target_link_libraries(TestEmit PRIVATE llvm-pip2 llvm nlohmann_json::nlohmann_json)
target_link_libraries(TestJitBasic PRIVATE llvm-pip2 llvm)
target_link_libraries(TestUnits PRIVATE llvm-pip2 llvm Catch2WithMain)
add_executable(TestEmit smoke/TestEmit.cpp)
add_executable(TestJitBasic smoke/TestJitBasic.cpp)
add_executable(TestTasks smoke/TestTasks.cpp)

add_custom_command(TARGET TestTasks POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/smoke/binary/task.export.bin $<TARGET_FILE_DIR:TestTasks>/task.export.bin
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/smoke/binary/task.export.info $<TARGET_FILE_DIR:TestTasks>/task.export.info)

add_executable(TestUnits
        units/TestEnvironment.cpp
        units/TestEnvironment.h
        units/ModifiablePoolItems.cpp
        units/ModifiablePoolItems.h
        units/TestArithmetic.cpp
        units/RandomIntGenerator.h
        units/TestMiscs.cpp
        units/TestLoadStore.cpp
        units/TestControlFlow.cpp
)

target_link_libraries(TestEmit PRIVATE llvm-pip2 nlohmann_json::nlohmann_json)
target_link_libraries(TestJitBasic PRIVATE llvm-pip2)
target_link_libraries(TestTasks PRIVATE llvm-pip2 nlohmann_json::nlohmann_json)
target_link_libraries(TestUnits PRIVATE llvm-pip2 Catch2WithMain)
add_library(llvm-pip2 SHARED
        Constants.h
        Instruction.h
        Opcode.h
        PoolItems.cpp
        PoolItems.h
        Register.h
        VMConfig.h
        VMContext.h
        VMEngine.cpp
        VMEngine.h
        ProgramAnalysis.cpp
        ProgramAnalysis.h
        Common.h
        Common.cpp
        Translator.cpp
        Translator.h
        Function.h
        Translator/Arthimetic.cpp
        Translator/ControlFlow.cpp
        VMOptions.h
        Translator/Miscs.cpp
        Translator/LoadStore.cpp
        Translator/Memory.cpp
        ObjectCache.cpp
        ObjectCache.h
        LinkerFix.h
        LinkerFix.cpp
        CInterface.cpp
        VMConfigParameters.h
        TaskHandler.cpp
        TaskHandler.h
        SpecialFunction.cpp
        SpecialFunction.h
        Callback.h
)

target_include_directories(llvm-pip2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(llvm-pip2 PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(llvm-pip2 PUBLIC llvm libco)

if (WIN32)
  target_link_options(llvm-pip2 PRIVATE "LINKER:--export-all-symbols")
endif()
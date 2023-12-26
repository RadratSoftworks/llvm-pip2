add_library(llvm-pip2
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
        ProgramAnalysis.h)

target_link_libraries(llvm-pip2 PRIVATE llvm)
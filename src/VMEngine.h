#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>

namespace Pip2 {
    class VMEngine {
    private:
        static bool s_mcjit_initialized_;

        llvm::LLVMContext llvm_context_;

        static void initialize_mcjit();

    public:
        VMEngine() = default;
        virtual ~VMEngine() = default;

        virtual void execute() = 0;
    };
}
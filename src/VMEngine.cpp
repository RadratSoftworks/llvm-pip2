#include "VMEngine.h"
#include "Register.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/MCJIT.h>

namespace Pip2 {
    void VMEngine::initialize_mcjit() {
        if (!s_mcjit_initialized_) {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmParser();
            llvm::InitializeNativeTargetAsmPrinter();
            LLVMLinkInMCJIT();

            s_mcjit_initialized_ = true;
        }
    }
}
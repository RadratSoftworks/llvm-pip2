#include "VMEngine.h"
#include "Register.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/MCJIT.h>

namespace Pip2 {
    VMEngine::VMEngine(const VMConfig &config, const VMOptions &options)
        : config_(config), options_(options) {

    }

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
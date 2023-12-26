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

    void VMEngine::initialize_types() {
        std::vector<llvm::Type*> context_element_types;

        context_element_types.insert(context_element_types.begin(), Register::TotalCount, llvm::Type::getInt32Ty(llvm_context_));
        context_type_ = llvm::StructType::create(llvm_context_, context_element_types, "VMContext");
    }
}
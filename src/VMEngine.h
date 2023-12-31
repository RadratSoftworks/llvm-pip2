#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

namespace Pip2 {
    struct VMConfig;
    struct VMOptions;

    class VMEngine {
    private:
        static bool s_mcjit_initialized_;

        llvm::LLVMContext llvm_context_;
        std::unique_ptr<llvm::ExecutionEngine> execution_engine_;

        const VMConfig &config_;
        const VMOptions &options_;

        static void initialize_mcjit();

    public:
        explicit VMEngine(const VMConfig &config, const VMOptions &options);
        virtual ~VMEngine() = default;

        virtual void execute() = 0;
    };
}
#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include <thread>

#include "ObjectCache.h"
#include "VMContext.h"

namespace Pip2 {
    struct VMConfig;
    struct VMOptions;

    typedef void (*HleHandler)(int hleFunctionCode);
    typedef void (*RuntimeFunction)(VMContext &context, std::uint32_t *memory_base, void **runtime_function_lookup,
                                    HleHandler hle_handler);

    class VMEngine {
    private:
        static bool s_mcjit_initialized_;

        llvm::LLVMContext llvm_context_;

        std::unique_ptr<ObjectCache> object_cache_;
        std::unique_ptr<llvm::ExecutionEngine> execution_engine_;
        std::unique_ptr<std::thread> execution_thread_;

        std::vector<void*> runtime_function_lookup_;
        VMContext context_{};

        const VMConfig &config_;
        const VMOptions &options_;

        std::string module_name_;
        RuntimeFunction found_runtime_function_{};

        static void initialize_mcjit();

    private:
        void initialize_execution_engine();
        void load_and_compile_module();

    public:
        static void default_optimize(llvm::Module &module);

        explicit VMEngine(std::string module_name, const VMConfig &config, const VMOptions &options);
        virtual ~VMEngine() = default;

        virtual void execute();

        [[nodiscard]] VMContext &context() { return context_; }
        [[nodiscard]] const VMContext &context() const { return context_; }
    };
}
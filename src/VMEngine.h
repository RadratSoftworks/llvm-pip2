#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include <thread>

#include "Callback.h"
#include "ObjectCache.h"
#include "VMContext.h"
#include "VMConfigParameters.h"
#include "VMConfig.h"
#include "VMOptions.h"
#include "TaskHandler.h"

namespace Pip2 {
    struct VMConfig;
    struct VMOptions;

    class VMEngine {
    private:
        static bool s_mcjit_initialized_;

        llvm::LLVMContext llvm_context_;

        std::unique_ptr<ObjectCache> object_cache_;
        std::unique_ptr<llvm::ExecutionEngine> execution_engine_;
        std::unique_ptr<TaskHandler> task_handler_;

        std::vector<void*> runtime_function_lookup_;

        VMConfig config_;
        VMOptions options_;

        std::string module_name_;
        RuntimeFunction found_runtime_function_{};
        void *active_handler_userdata_{};
        bool module_use_task_;

        static void initialize_mcjit();

    private:
        void initialize_execution_engine();
        void load_and_compile_module();
        void prepare_runtime_function();

        void run_task(TaskData &task_data, HleHandler hle_handler);

    public:
        static void default_optimize(llvm::Module &module);

        explicit VMEngine(std::string module_name, const VMConfigParameters &config, VMOptions &&options);
        ~VMEngine();

        virtual void execute(HleHandler handler = nullptr, void *userdata = nullptr);
        virtual void execute_task_aware(HleHandler handler = nullptr, void *userdata = nullptr);

        virtual std::uint32_t reg(Register reg) const;
        virtual void reg(Register reg, std::uint32_t value);

        [[nodiscard]] VMContext &context() { return task_handler_->current_task_context(); }
        [[nodiscard]] const VMContext &context() const { return task_handler_->current_task_context(); }

        TaskHandler *task_handler() { return task_handler_.get(); }
        void *userdata() { return active_handler_userdata_; }
    };

    extern thread_local VMEngine *engine_instance;
}
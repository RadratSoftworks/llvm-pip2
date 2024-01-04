#include "VMEngine.h"
#include "VMOptions.h"
#include "VMConfig.h"
#include "ProgramAnalysis.h"
#include "Translator.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/FileSystem.h>

#include <utility>

namespace Pip2 {
    bool VMEngine::s_mcjit_initialized_ = false;

    VMEngine::VMEngine(std::string module_name, const VMConfigParameters &config, VMOptions &&options)
        : module_name_(std::move(module_name))
        , config_(config.memory_base_, static_cast<std::size_t>(config.memory_size_), config.pool_items_base_, static_cast<std::size_t>(config.pool_item_count_))
        , options_(options)
        , context_()
        , found_runtime_function_(nullptr) {
        initialize_mcjit();

        if (options_.cache_)
        {
            object_cache_ = std::make_unique<ObjectCache>(options_.cache_root_path_ ? options_.cache_root_path_ : "");
        }

        initialize_execution_engine();
        load_and_compile_module();
    }

    void VMEngine::initialize_execution_engine() {
        auto null_module = std::make_unique<llvm::Module>("_null_module_", llvm_context_);

        execution_engine_.reset(llvm::EngineBuilder(std::move(null_module))
            .setEngineKind(llvm::EngineKind::JIT)
            .setOptLevel(llvm::CodeGenOpt::Aggressive)
            .setRelocationModel(llvm::Reloc::Model::PIC_)
            .create());

        if (options_.cache_)
        {
            execution_engine_->setObjectCache(object_cache_.get());
        }
    }

    void VMEngine::default_optimize(llvm::Module &module) {
        // Create the analysis managers.
        // These must be declared in this order so that they are destroyed in the
        // correct order due to inter-analysis-manager references.
        llvm::LoopAnalysisManager LAM;
        llvm::FunctionAnalysisManager FAM;
        llvm::CGSCCAnalysisManager CGAM;
        llvm::ModuleAnalysisManager MAM;

        // Create the new pass manager builder.
        // Take a look at the PassBuilder constructor parameters for more
        // customization, e.g. specifying a TargetMachine or various debugging
        // options.
        llvm::PassBuilder PB;

        // Register all the basic analyses with the managers.
        PB.registerModuleAnalyses(MAM);
        PB.registerCGSCCAnalyses(CGAM);
        PB.registerFunctionAnalyses(FAM);
        PB.registerLoopAnalyses(LAM);
        PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

        // Create the pass manager.
        // This one corresponds to a typical -O2 optimization pipeline.
        llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);

        // Optimize the IR!
        MPM.run(module, MAM);
    }

    void VMEngine::load_and_compile_module() {
        if (options_.cache_) {
            if (object_cache_->does_cache_exist(module_name_))
            {
                auto cache_buffer = object_cache_->load(module_name_);

                if (cache_buffer)
                {
                    if (auto object_file = llvm::object::ObjectFile::createObjectFile(*cache_buffer))
                    {
                        execution_engine_->addObjectFile(llvm::object::OwningBinary<llvm::object::ObjectFile>(
                                std::move(*object_file),
                                std::move(cache_buffer)));

                        return;
                    }
                }
            }
        }

        // Need to recompile, first analyze
        ProgramAnalysis analysis(reinterpret_cast<std::uint32_t*>(config_.memory_base()),
                                 options_.text_base_, config_.memory_size(), config_.pool_items());

        std::vector<Function> found_functions = analysis.analyze(options_.entry_point_);

        // Then translate
        Translator translator(llvm_context_, config_, options_);
        auto module = translator.translate(module_name_, found_functions);

        // Optimize
        default_optimize(*module);

        // Add to execution engine
        execution_engine_->addModule(std::move(module));
        execution_engine_->finalizeObject();
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

    void VMEngine::execute(HleHandler hle_handler, void *userdata) {
        if (!found_runtime_function_) {
            static constexpr std::size_t TOTAL_LOOKUP_STORAGE = 0x40000;

            found_runtime_function_ = reinterpret_cast<RuntimeFunction>(execution_engine_->getFunctionAddress("entry_point"));
            runtime_function_lookup_.resize(TOTAL_LOOKUP_STORAGE);

            std::fill(runtime_function_lookup_.begin(), runtime_function_lookup_.end(), reinterpret_cast<std::uint64_t*>(0x123456789ABCDEF));
        }

        found_runtime_function_(context_, reinterpret_cast<std::uint32_t*>(config_.memory_base()), runtime_function_lookup_.data(), hle_handler, userdata);
    }

    std::uint32_t VMEngine::reg(Register reg) const {
        if (reg > Register::PC) {
            throw std::runtime_error("Invalid register");
        }

        return context_.regs_[reg >> 2];
    }

    void VMEngine::reg(Register reg, std::uint32_t value) {
        if (reg > Register::PC) {
            throw std::runtime_error("Invalid register");
        }

        context_.regs_[reg >> 2] = value;
    }
}
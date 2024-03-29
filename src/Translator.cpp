#include "Translator.h"
#include "Register.h"
#include "Common.h"
#include "Constants.h"
#include "SpecialFunction.h"

#include <format>
#include <iostream>

namespace Pip2 {
    static bool is_branch_cutoff(Instruction instruction, const PoolItems &pool_items, std::uint32_t next_word)
    {
        switch (instruction.word_encoding.opcode)
        {
        case Opcode::BEQ:
        case Opcode::BEQI:
        case Opcode::BEQIB:
        case Opcode::BNE:
        case Opcode::BNEI:
        case Opcode::BNEIB:
        case Opcode::BLT:
        case Opcode::BLTI:
        case Opcode::BLTIB:
        case Opcode::BLTU:
        case Opcode::BLTUI:
        case Opcode::BLTUIB:
        case Opcode::BLE:
        case Opcode::BLEI:
        case Opcode::BLEIB:
        case Opcode::BLEU:
        case Opcode::BLEUI:
        case Opcode::BLEUIB:
        case Opcode::BGT:
        case Opcode::BGTI:
        case Opcode::BGTIB:
        case Opcode::BGTU:
        case Opcode::BGTUI:
        case Opcode::BGTUIB:
        case Opcode::BGE:
        case Opcode::BGEI:
        case Opcode::BGEIB:
        case Opcode::BGEU:
        case Opcode::BGEUI:
        case Opcode::BGEUIB:
        case Opcode::JPl:
        case Opcode::RET:
        case Opcode::JPr:
            return true;

        case Opcode::CALLl: {
            if (Common::get_immediate_pip_dword(next_word)) {
                return false;
            }

            return (pool_items.is_pool_item_terminate_function(next_word));
        }

        case Opcode::CALLr:
            return false;

        default:
            return false;
        }
    }

    std::uint32_t Translator::fetch_immediate() {
        std::uint32_t dword = *reinterpret_cast<std::uint32_t*>(config_.memory_base() + current_addr_ + INSTRUCTION_SIZE);
        current_addr_ += 4;

        if (std::optional<std::uint32_t> constant_dword = Common::get_immediate_pip_dword(dword)) {
            return constant_dword.value();
        }

        if (config_.pool_items().is_pool_item_constant(dword)) {
            return config_.pool_items().get_pool_item_constant(dword);
        }

        throw std::runtime_error("Invalid immediate");
    }

    llvm::Value *Translator::get_register_pointer(const Register reg) {
        if ((reg & 3) != 0)
        {
            throw std::runtime_error("Invalid register");
        }

        return builder_.CreateGEP(context_type_,
                                  current_context_,
                                  { builder_.getInt32(0), builder_.getInt32(reg >> 2) });
    }

    template <>
    llvm::Value *Translator::get_register<std::uint32_t>(Register src) {
        if (src == Register::ZR) {
            return builder_.getInt32(0);
        }

        return builder_.CreateLoad(i32_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::int32_t>(Register src) {
        if (src == Register::ZR) {
            return builder_.getInt32(0);
        }

        return builder_.CreateLoad(i32_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::uint16_t>(Register src) {
        if (src == Register::ZR) {
            return builder_.getInt16(0);
        }

        return builder_.CreateLoad(i16_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::int16_t>(Register src) {
        if (src == Register::ZR) {
            return builder_.getInt16(0);
        }

        return builder_.CreateLoad(i16_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::uint8_t>(Register src) {
        if (src == Register::ZR) {
            return builder_.getInt8(0);
        }

        return builder_.CreateLoad(i8_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::int8_t>(Register src) {
        if (src == Register::ZR) {
            return builder_.getInt8(0);
        }

        return builder_.CreateLoad(i8_type_, get_register_pointer(src));
    }

    void Translator::set_register(Pip2::Register dest, llvm::Value *value) {
        if (dest == Register::ZR) {
            throw std::runtime_error("Can't set to ZR register!");
        }

        builder_.CreateStore(value, get_register_pointer(dest));
    }

    void Translator::translate_function(llvm::Function *function, const Function &function_info) {
        blocks_.clear();
        current_function_jump_table_translate_state_.clear();

        current_context_ = function->getArg(0);
        current_memory_base_ = function->getArg(1);
        current_function_lookup_array_ = function->getArg(2);
        current_hle_handler_pointer_ = function->getArg(3);
        current_hle_handler_callee_ = llvm::FunctionCallee(hle_handler_function_type_,
                                                           builder_.CreateIntToPtr(function->getArg(3), hle_handler_function_type_->getPointerTo()));
        current_hle_handler_userdata_ = function->getArg(4);
        current_function_analysis_ = &function_info;

        auto entry_block = llvm::BasicBlock::Create(context_, "entry", function);

        for (const auto &label: function_info.labels_) {
            blocks_.emplace(label, llvm::BasicBlock::Create(context_, std::format("label_{:08X}", label), function));
        }

        builder_.SetInsertPoint(entry_block);
        builder_.CreateBr(blocks_[function_info.addr_]);

        for (const auto &jump_table: function_info.jump_tables_) {
            current_function_jump_table_translate_state_.emplace(jump_table.switch_value_resolved_addr_,
                                                                 JumpTableTranslateState(jump_table.switch_value_resolved_addr_, jump_table.switch_value_register_));
        }

        Instruction previous_inst = { 0 };
        std::uint32_t previous_next_dword = 0;

        current_function_ = function;

        for (current_addr_ = function_info.addr_; current_addr_ < function_info.addr_ + function_info.length_; current_addr_ += INSTRUCTION_SIZE) {
            if (blocks_.find(current_addr_) != blocks_.end()) {
                // If not branching, it's continuous block
                if (current_addr_ != function_info.addr_ && !is_branch_cutoff(previous_inst, config_.pool_items(), previous_next_dword))
                {
                    builder_.CreateBr(blocks_[current_addr_]);
                }

                builder_.SetInsertPoint(blocks_[current_addr_]);
            }

            // Store values for jump table
            if (!current_function_jump_table_translate_state_.empty()) {
                auto jump_table_translate_state = current_function_jump_table_translate_state_.find(current_addr_);
                if (jump_table_translate_state != current_function_jump_table_translate_state_.end()) {
                    jump_table_translate_state->second.case_value_ = get_register<std::uint32_t>(jump_table_translate_state->second.case_value_register_);
                }
            }

            Instruction instruction { *reinterpret_cast<std::uint32_t*>(config_.memory_base() + current_addr_) };

            if (current_addr_ + INSTRUCTION_SIZE < function_info.addr_ + function_info.length_) {
                previous_next_dword = *reinterpret_cast<std::uint32_t*>(config_.memory_base() + current_addr_ + INSTRUCTION_SIZE);
            }

            InstructionTranslator translator = instruction_translators_[instruction.word_encoding.opcode];

            if (translator == nullptr) {
                throw std::runtime_error(std::format("Unhandled instruction: {}", (int)instruction.word_encoding.opcode));
            }

            (this->*translator)(instruction);

            previous_inst = instruction;
        }
    }

    void Translator::generate_hle_handler_trampoline(llvm::Module *module) {
        auto unimplemented_func = llvm::Function::Create(function_type_, llvm::GlobalValue::ExternalLinkage,
                                                       "sub_unimplemented", module);

        auto unimplemented_block = llvm::BasicBlock::Create(context_, "1", unimplemented_func);
        builder_.SetInsertPoint(unimplemented_block);

        auto hle_callee = llvm::FunctionCallee(hle_handler_function_type_,
                                               builder_.CreateIntToPtr(unimplemented_func->getArg(3), hle_handler_function_type_->getPointerTo()));

        builder_.CreateCall(hle_callee, {
            unimplemented_func->getArg(4),
            builder_.getInt32(Common::exception_to_hle_code(Common::ExceptionCode::NotCompiledFunction))
        });

        builder_.CreateRetVoid();
    }

    void Translator::generate_entry_point_function(const std::uint32_t entry_point_addr) {
        auto entry_point_sub = functions_[entry_point_addr];
        auto entry_point_func = llvm::Function::Create(function_type_, llvm::GlobalValue::ExternalLinkage,
                                                       "entry_point", entry_point_sub->getParent());

        auto entry_point_block = llvm::BasicBlock::Create(context_, "1", entry_point_func);
        builder_.SetInsertPoint(entry_point_block);

        auto lookup_table = entry_point_func->getArg(2);

        for (const auto &[addr, function_llvm]: functions_) {
            auto function_pointer = builder_.CreateGEP(get_pointer_integer_type(), lookup_table, { builder_.getInt32(addr >> 2) });
            builder_.CreateStore(function_llvm, function_pointer);
        }

        builder_.CreateCall(entry_point_sub, {
            entry_point_func->getArg(0),
            entry_point_func->getArg(1),
            entry_point_func->getArg(2),
            entry_point_func->getArg(3),
            entry_point_func->getArg(4)
        });

        builder_.CreateRetVoid();
    }

    std::unique_ptr<llvm::Module> Translator::translate(const std::string &module_name, const std::vector<Function> &functions, bool use_task) {
        use_task_ = use_task;
        functions_.clear();

        auto module = std::make_unique<llvm::Module>(module_name, context_);

        for (const Function &function: functions) {
            functions_.emplace(function.addr_, llvm::Function::Create(function_type_, llvm::GlobalValue::ExternalLinkage,
                               std::format("sub_{:08X}", function.addr_), module.get()));
        }

        for (const Function &function: functions) {
            translate_function(functions_[function.addr_], function);
        }

        // Generate entry point function, which also setup the lookup table
        for (const Function &function: functions) {
            if (function.is_entry_point_) {
                generate_entry_point_function(function.addr_);
                break;
            }
        }

        generate_hle_handler_trampoline(module.get());

        return module;
    }

    llvm::Type *Translator::get_pointer_integer_type()
    {
        return i64_type_;
    }

    void Translator::initialize_types() {
        void_type_ = llvm::Type::getVoidTy(context_);

        i8_type_ = llvm::Type::getInt8Ty(context_);
        i16_type_ = llvm::Type::getInt16Ty(context_);
        i32_type_ = llvm::Type::getInt32Ty(context_);
        i64_type_ = llvm::Type::getInt64Ty(context_);

        std::vector<llvm::Type*> context_element_types;

        context_element_types.insert(context_element_types.begin(), Register::TotalCount, i32_type_);
        context_type_ = llvm::StructType::create(context_, context_element_types, "VMContext");
        function_type_ = llvm::FunctionType::get(void_type_, {
                context_type_->getPointerTo(),                      // VMContext*
                i8_type_->getPointerTo(),                           // std::uint8_t* memory_base
                get_pointer_integer_type()->getPointerTo(),         // VMFunctionPointer* function_lookup_array
                get_pointer_integer_type()->getPointerTo(),         // HLEHandlerFunctionPointer hle_handler
                i8_type_->getPointerTo()                          // void* hle_handler_userdata
            },false);

        hle_handler_function_type_ = llvm::FunctionType::get(void_type_, {
                i8_type_->getPointerTo(),
                i32_type_
            }, false);

        for (auto i = 0; i < std_call_type_.size(); i++)
        {
            std::vector<llvm::Type*> std_call_type_args(i, i32_type_);
            std_call_type_[i] = llvm::FunctionType::get(void_type_, { std_call_type_args.data(), std_call_type_args.size() }, false);
        }

        for (auto i = 0; i < std_call_type_with_return_.size(); i++)
        {
            std::vector<llvm::Type*> std_call_type_args(i, i32_type_);
            std_call_type_with_return_[i] = llvm::FunctionType::get(i32_type_, { std_call_type_args.data(), std_call_type_args.size() }, false);
        }

        wrapper_function_type_ = llvm::FunctionType::get(void_type_, {
            context_type_->getPointerTo()
        }, false);
    }

    void Translator::call_special_function(SpecialPoolFunction function)
    {
        if (special_functions_.find(function) == special_functions_.end()) {
            auto special_function_info = SPECIAL_POOL_FUNCTION_INFOS.find(function);
            if (special_function_info == SPECIAL_POOL_FUNCTION_INFOS.end()) {
                throw std::runtime_error("Invalid special function");
            }

            auto module = current_function_->getParent();
            auto function_type = special_function_info->second.has_return_value_ ?
                    std_call_type_with_return_[special_function_info->second.arg_count_] :
                    std_call_type_[special_function_info->second.arg_count_];

            auto external_function = module->getOrInsertFunction(special_function_info->second.name_, function_type);
            auto function_wrapper = llvm::Function::Create(wrapper_function_type_, llvm::GlobalValue::ExternalLinkage,
                                                           special_function_info->second.name_ + "_wrapper", module);

            auto function_wrapper_block = llvm::BasicBlock::Create(context_, "entry", function_wrapper);

            auto previous_insert_point = builder_.GetInsertBlock();
            builder_.SetInsertPoint(function_wrapper_block);

            auto context_arg = function_wrapper->getArg(0);
            std::vector<llvm::Value*> function_args;

            for (auto i = 0; i < special_function_info->second.arg_count_; i++)
            {
                auto arg_value = builder_.CreateLoad(i32_type_, builder_.CreateGEP(
                        context_type_,
                        context_arg,
                        { builder_.getInt32(0), builder_.getInt32((Register::P0 >> 2) + i) }));

                function_args.push_back(arg_value);
            }

            auto ret_value = builder_.CreateCall(external_function, function_args);

            if (special_function_info->second.has_return_value_)
            {
                builder_.CreateStore(ret_value, builder_.CreateGEP(
                        context_type_,
                        context_arg,
                        { builder_.getInt32(0), builder_.getInt32(Register::R0 >> 2) }));
            }

            builder_.CreateRetVoid();
            builder_.SetInsertPoint(previous_insert_point);

            special_functions_.emplace(function, function_wrapper);
        }

        builder_.CreateCall(special_functions_[function], {
            current_context_
        });
    }

    Translator::Translator(llvm::LLVMContext &context, const VMConfig &config, const VMOptions &options)
        : context_(context)
        , config_(config)
        , options_(options)
        , builder_(context)
        , current_context_(nullptr)
        , void_type_(nullptr)
        , i8_type_(nullptr)
        , i16_type_(nullptr)
        , i32_type_(nullptr)
        , current_addr_(0)
        , use_task_(false) {
        initialize_types();
    }
}

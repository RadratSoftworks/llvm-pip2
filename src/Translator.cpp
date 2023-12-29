#include "Translator.h"
#include "Register.h"
#include "Common.h"
#include "Constants.h"

#include <format>

namespace Pip2 {
    std::uint32_t Translator::fetch_immediate() {
        std::uint32_t dword = *reinterpret_cast<std::uint32_t*>(config_.memory_base() + current_addr_ + INSTRUCTION_SIZE);
        current_addr_ += 4;

        if (std::optional<std::uint32_t> constant_dword = Common::get_immediate_pip_dword(dword)) {
            return constant_dword.value();
        }

        if (pool_items_.is_pool_item_constant(dword)) {
            return pool_items_.get_pool_item_constant(dword);
        }

        throw new std::runtime_error("Invalid immediate");
    }

    llvm::Value *Translator::get_register_pointer(const Register reg) {
        if ((reg & 3) != 0)
        {
            throw new std::runtime_error("Invalid register");
        }

        return builder_.CreateGEP(context_type_,
                                  current_context_,
                                  { builder_.getInt32(0), builder_.getInt32(reg >> 2) });
    }

    template <>
    llvm::Value *Translator::get_register<std::uint32_t>(Register src) {
        return builder_.CreateLoad(i32_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::int32_t>(Register src) {
        return builder_.CreateLoad(i32_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::uint16_t>(Register src) {
        return builder_.CreateLoad(i16_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::int16_t>(Register src) {
        return builder_.CreateLoad(i16_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::uint8_t>(Register src) {
        return builder_.CreateLoad(i8_type_, get_register_pointer(src));
    }

    template <>
    llvm::Value *Translator::get_register<std::int8_t>(Register src) {
        return builder_.CreateLoad(i8_type_, get_register_pointer(src));
    }

    void Translator::set_register(Pip2::Register dest, llvm::Value *value) {
        builder_.CreateStore(value, get_register_pointer(dest));
    }

    llvm::Module *Translator::translate(const std::string &module_name, const std::vector<Function> &functions) {
        auto module = new llvm::Module(module_name, context_);

        for (const Function &function: functions) {
        }

        return module;
    }

    void Translator::initialize_types() {
        void_type_ = llvm::Type::getVoidTy(context_);

        i8_type_ = llvm::Type::getInt8Ty(context_);
        i16_type_ = llvm::Type::getInt16Ty(context_);
        i32_type_ = llvm::Type::getInt32Ty(context_);

        std::vector<llvm::Type*> context_element_types;

        context_element_types.insert(context_element_types.begin(), Register::TotalCount, i32_type_);
        context_type_ = llvm::StructType::create(context_, context_element_types, "VMContext");
    }

    Translator::Translator(llvm::LLVMContext &context, const VMConfig &config, const PoolItems &pool_items)
        : context_(context)
        , config_(config)
        , pool_items_(pool_items)
        , builder_(context)
        , current_context_(nullptr)
        , void_type_(nullptr)
        , i8_type_(nullptr)
        , i16_type_(nullptr)
        , i32_type_(nullptr)
        , current_addr_(0) {
        initialize_types();
    }
}

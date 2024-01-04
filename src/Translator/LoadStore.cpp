#include "../Translator.h"
#include "../Common.h"

namespace Pip2
{
    llvm::Value *Translator::get_memory_pointer(llvm::Value *address)
    {
        return builder_.CreateGEP(i8_type_, current_memory_base_, { address });
    }

    void Translator::LDI(Instruction instruction)
    {
        auto value = llvm::ConstantInt::get(i32_type_, fetch_immediate());
        set_register(instruction.two_sources_encoding.rd, value);
    }

    void Translator::LDQ(Instruction instruction)
    {
        auto value = llvm::ConstantInt::get(i32_type_, Common::sign_extend(instruction.word_encoding.imm));
        set_register(instruction.two_sources_encoding.rd, value);
    }

    void Translator::LDWd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        auto value = builder_.CreateLoad(i32_type_, get_memory_pointer(address));
        set_register(instruction.two_sources_encoding.rd, value);
    }

    void Translator::LDHUd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        auto value = builder_.CreateLoad(i16_type_, get_memory_pointer(address));
        set_register(instruction.two_sources_encoding.rd, builder_.CreateZExt(value, i32_type_));
    }

    void Translator::LDBUd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        auto value = builder_.CreateLoad(i8_type_, get_memory_pointer(address));
        set_register(instruction.two_sources_encoding.rd, builder_.CreateZExt(value, i32_type_));
    }

    void Translator::LDHd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        auto value = builder_.CreateLoad(i16_type_, get_memory_pointer(address));
        set_register(instruction.two_sources_encoding.rd, builder_.CreateSExt(value, i32_type_));
    }

    void Translator::LDBd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        auto value = builder_.CreateLoad(i8_type_, get_memory_pointer(address));
        set_register(instruction.two_sources_encoding.rd, builder_.CreateSExt(value, i32_type_));
    }

    void Translator::STWd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        builder_.CreateStore(get_register<std::uint32_t>(instruction.two_sources_encoding.rd), get_memory_pointer(address));
    }

    void Translator::STHd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        builder_.CreateStore(get_register<std::uint16_t>(instruction.two_sources_encoding.rd), get_memory_pointer(address));
    }

    void Translator::STBd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        builder_.CreateStore(get_register<std::uint8_t>(instruction.two_sources_encoding.rd), get_memory_pointer(address));
    }

    void Translator::STORE(Instruction instruction)
    {
        auto stack_value = get_register<std::uint32_t>(Register::SP);

        if (instruction.range_reg_encoding.rs == Register::ZR)
        {
            auto stack = get_memory_pointer(builder_.CreateSub(stack_value, builder_.getInt32(4)));

            set_register(Register::RA, builder_.CreateLoad(i32_type_, stack));
            set_register(Register::SP, builder_.CreateSub(stack_value, builder_.getInt32(4)));
        }
        else
        {
            auto stack_store_base = builder_.CreateSub(stack_value, builder_.getInt32(instruction.range_reg_encoding.count));
            auto stack = get_memory_pointer(stack_store_base);
            auto reg_addr = get_register_pointer(instruction.range_reg_encoding.rs);

            builder_.CreateMemCpy(
                    stack,
                    llvm::MaybeAlign(1),
                    reg_addr,
                    llvm::MaybeAlign(4),
                    builder_.getInt32(instruction.range_reg_encoding.count)
            );

            set_register(Register::SP, stack_store_base);
        }
    }

    void Translator::RESTORE(Instruction instruction)
    {
        auto stack_value = get_register<std::uint32_t>(Register::SP);
        auto stack = get_memory_pointer(stack_value);

        if (instruction.range_reg_encoding.rs == Register::ZR)
        {
            set_register(Register::RA, builder_.CreateLoad(i32_type_, stack));
            set_register(Register::SP, builder_.CreateAdd(stack_value, builder_.getInt32(4)));
        }
        else
        {
            auto reg_addr = get_register_pointer(static_cast<Register>(instruction.range_reg_encoding.rs -
                    instruction.range_reg_encoding.count + 4));

            builder_.CreateMemCpy(
                    reg_addr,
                    llvm::MaybeAlign(4),
                    stack,
                    llvm::MaybeAlign(1),
                    builder_.getInt32(instruction.range_reg_encoding.count)
            );

            set_register(Register::SP, builder_.CreateAdd(stack_value, builder_.getInt32(instruction.range_reg_encoding.count)));
        }
    }
}
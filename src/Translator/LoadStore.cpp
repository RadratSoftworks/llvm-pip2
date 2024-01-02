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
        set_register(instruction.two_sources_encoding.rd, value);
    }

    void Translator::LDBUd(Instruction instruction)
    {
        auto address = builder_.CreateAdd(get_register<std::uint32_t>(instruction.two_sources_encoding.rs), llvm::ConstantInt::get(i32_type_, fetch_immediate()));
        auto value = builder_.CreateLoad(i8_type_, get_memory_pointer(address));
        set_register(instruction.two_sources_encoding.rd, value);
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
        auto stack = get_memory_pointer(stack_value);

        if (instruction.range_reg_encoding.rs == Register::ZR)
        {
            set_register(Register::RA, builder_.CreateLoad(i32_type_, builder_.CreateGEP(i32_type_, stack, { builder_.getInt64(-1) })));
            set_register(Register::SP, builder_.CreateSub(stack_value, builder_.getInt32(4)));
        }
        else
        {
            auto reg_addr = get_register_pointer(instruction.range_reg_encoding.rs);

            builder_.CreateMemCpy(
                    builder_.CreateGEP(i32_type_, stack, { builder_.getInt64(-(instruction.range_reg_encoding.count + 1)) }),
                    llvm::MaybeAlign(4),
                    reg_addr,
                    llvm::MaybeAlign(4),
                    llvm::ConstantInt::get(i64_type_, instruction.range_reg_encoding.count * 4)
            );

            set_register(Register::SP, builder_.CreateSub(stack_value, builder_.getInt32(instruction.range_reg_encoding.count * 4)));
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
                    (instruction.range_reg_encoding.count + 1) * 4));

            builder_.CreateMemCpy(
                    reg_addr,
                    llvm::MaybeAlign(4),
                    stack,
                    llvm::MaybeAlign(4),
                    llvm::ConstantInt::get(i64_type_, instruction.range_reg_encoding.count * 4)
            );

            set_register(Register::SP, builder_.CreateAdd(stack_value, builder_.getInt32(instruction.range_reg_encoding.count * 4)));
        }
    }
}
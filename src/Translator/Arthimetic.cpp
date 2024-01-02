#include <format>
#include "../Translator.h"
#include "../Common.h"

namespace Pip2
{
    void Translator::ADD(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, rhs));
    }

    void Translator::ADDi(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, builder_.getInt32( rhs)));
    }

    void Translator::ADDH(Instruction instruction)
    {
        auto lhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, rhs));
    }

    void Translator::ADDHi(Instruction instruction)
    {
        auto lhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        auto rhs = Common::sign_extend_to_word(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, builder_.getInt16(rhs)));
    }

    void Translator::ADDB(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, rhs));
    }

    void Translator::ADDBi(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = instruction.two_sources_encoding.rt;

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, builder_.getInt8( rhs)));
    }

    void Translator::ADDQ(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = Common::sign_extend(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, builder_.getInt32( rhs)));
    }

    void Translator::SUB(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateSub(lhs, rhs));
    }

    void Translator::SUBi(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateSub(lhs, builder_.getInt32( rhs)));
    }

    void Translator::SUBH(Instruction instruction)
    {
        auto lhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateSub(lhs, rhs));
    }

    void Translator::SUBB(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateSub(lhs, rhs));
    }

    void Translator::AND(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAnd(lhs, rhs));
    }

    void Translator::ANDi(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAnd(lhs, builder_.getInt32( rhs)));
    }

    void Translator::ANDH(Instruction instruction)
    {
        auto lhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAnd(lhs, rhs));
    }

    void Translator::ANDHi(Instruction instruction)
    {
        auto lhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        auto rhs = instruction.two_sources_encoding.rt;

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAnd(lhs, builder_.getInt16( rhs)));
    }

    void Translator::ANDB(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAnd(lhs, rhs));
    }

    void Translator::ANDBi(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = instruction.two_sources_encoding.rt;

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAnd(lhs, builder_.getInt8( rhs)));
    }

    void Translator::OR(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateOr(lhs, rhs));
    }

    void Translator::ORi(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateOr(lhs, builder_.getInt32( rhs)));
    }

    void Translator::ORH(Instruction instruction)
    {
        auto lhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateOr(lhs, rhs));
    }

    void Translator::ORB(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateOr(lhs, rhs));
    }

    void Translator::ORBi(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = instruction.two_sources_encoding.rt;

        set_register(instruction.two_sources_encoding.rd, builder_.CreateOr(lhs, builder_.getInt8( rhs)));
    }

    void Translator::XOR(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateXor(lhs, rhs));
    }

    void Translator::XORi(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateXor(lhs, builder_.getInt32( rhs)));
    }

    void Translator::MUL(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateMul(lhs, rhs));
    }

    void Translator::MULQ(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = instruction.two_sources_encoding.rt;

        set_register(instruction.two_sources_encoding.rd, builder_.CreateMul(lhs, builder_.getInt32( rhs)));
    }

    void Translator::MULi(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateMul(lhs, builder_.getInt32( rhs)));
    }

    void Translator::DIV(Instruction instruction)
    {
        auto rhs = get_register<std::int32_t>(instruction.two_sources_encoding.rt);

        if (options_.divide_by_zero_result_zero)
        {
            auto current_block = builder_.GetInsertBlock();
            auto divide_block = llvm::BasicBlock::Create(context_, std::format("divide_{:08X}", current_addr_), current_function_);
            auto result_block = llvm::BasicBlock::Create(context_, std::format("result_divide_{:08X}", current_addr_), current_function_);

            auto cond = builder_.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, rhs, builder_.getInt32( 0));
            builder_.CreateCondBr(cond, result_block, divide_block);

            builder_.SetInsertPoint(divide_block);
            auto divide_value = builder_.CreateSDiv(get_register<std::int32_t>(instruction.two_sources_encoding.rs), rhs);

            builder_.SetInsertPoint(result_block);
            auto phi = builder_.CreatePHI(i32_type_, 2);

            phi->addIncoming(divide_value, divide_block);
            phi->addIncoming(builder_.getInt32( 0), current_block);

            set_register(instruction.two_sources_encoding.rd, phi);
        }
        else
        {
            set_register(instruction.two_sources_encoding.rd, builder_.CreateSDiv(get_register<std::int32_t>(instruction.two_sources_encoding.rs), rhs));
        }
    }

    void Translator::DIVU(Instruction instruction)
    {
        auto rhs = get_register<std::int32_t>(instruction.two_sources_encoding.rt);

        if (options_.divide_by_zero_result_zero)
        {
            auto current_block = builder_.GetInsertBlock();
            auto divide_block = llvm::BasicBlock::Create(context_, std::format("divide_{:08X}", current_addr_), current_function_);
            auto result_block = llvm::BasicBlock::Create(context_, std::format("result_divide_{:08X}", current_addr_), current_function_);

            auto cond = builder_.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, rhs, builder_.getInt32( 0));
            builder_.CreateCondBr(cond, result_block, divide_block);

            builder_.SetInsertPoint(divide_block);
            auto divide_value = builder_.CreateUDiv(get_register<std::int32_t>(instruction.two_sources_encoding.rs), rhs);

            builder_.SetInsertPoint(result_block);
            auto phi = builder_.CreatePHI(i32_type_, 2);

            phi->addIncoming(divide_value, divide_block);
            phi->addIncoming(builder_.getInt32( 0), current_block);

            set_register(instruction.two_sources_encoding.rd, phi);
        }
        else
        {
            set_register(instruction.two_sources_encoding.rd, builder_.CreateUDiv(get_register<std::int32_t>(instruction.two_sources_encoding.rs), rhs));
        }
    }

    void Translator::DIVi(Instruction instruction)
    {
        auto lhs = get_register<std::int32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateSDiv(lhs, builder_.getInt32( rhs)));
    }

    void Translator::DIVUi(Instruction instruction)
    {
        auto lhs = get_register<std::int32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateUDiv(lhs, builder_.getInt32( rhs)));
    }

    void Translator::MOV(Instruction instruction)
    {
        auto value = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        set_register(instruction.two_sources_encoding.rd, value);
    }

    void Translator::MOVB(Instruction instruction)
    {
        auto value = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        set_register(instruction.two_sources_encoding.rd, value);
    }

    void Translator::MOVH(Instruction instruction)
    {
        auto value = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        set_register(instruction.two_sources_encoding.rd, value);
    }

    void Translator::SLL(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateShl(lhs, rhs));
    }

    void Translator::SLLi(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = instruction.two_sources_encoding.rt;

        set_register(instruction.two_sources_encoding.rd, builder_.CreateShl(lhs, builder_.getInt32( rhs)));
    }

    void Translator::SLLH(Instruction instruction)
    {
        auto lhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateShl(lhs, rhs));
    }

    void Translator::SLLB(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateShl(lhs, rhs));
    }

    void Translator::SRL(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateLShr(lhs, rhs));
    }

    void Translator::SRLi(Instruction instruction)
    {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = instruction.two_sources_encoding.rt;

        set_register(instruction.two_sources_encoding.rd, builder_.CreateLShr(lhs, builder_.getInt32( rhs)));
    }

    void Translator::SRLH(Instruction instruction)
    {
        auto lhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint16_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateLShr(lhs, rhs));
    }

    void Translator::SRLB(Instruction instruction)
    {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateLShr(lhs, rhs));
    }

    void Translator::SRA(Instruction instruction)
    {
        auto lhs = get_register<std::int32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::int32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAShr(lhs, rhs));
    }

    void Translator::SRAi(Instruction instruction)
    {
        auto lhs = get_register<std::int32_t>(instruction.two_sources_encoding.rs);
        auto rhs = instruction.two_sources_encoding.rt;

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAShr(lhs, builder_.getInt32( rhs)));
    }

    void Translator::SRAH(Instruction instruction)
    {
        auto lhs = get_register<std::int16_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::int16_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAShr(lhs, rhs));
    }

    void Translator::SRAB(Instruction instruction)
    {
        auto lhs = get_register<std::int8_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::int8_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAShr(lhs, rhs));
    }
}
#include "../Translator.h"
#include "../Common.h"

namespace Pip2
{
    void Translator::ADD(Instruction instruction) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, rhs));
    }

    void Translator::ADDi(Instruction instruction) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, llvm::ConstantInt::get(i32_type_, rhs)));
    }

    void Translator::ADDQ(Instruction instruction) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = Common::sign_extend(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateAdd(lhs, llvm::ConstantInt::get(i32_type_, rhs)));
    }

    void Translator::SUB(Instruction instruction) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateSub(lhs, rhs));
    }

    void Translator::SUBi(Instruction instruction) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = fetch_immediate();

        set_register(instruction.two_sources_encoding.rd, builder_.CreateSub(lhs, llvm::ConstantInt::get(i32_type_, rhs)));
    }

    void Translator::SUBQ(Instruction instruction) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto rhs = Common::sign_extend(instruction.two_sources_encoding.rt);

        set_register(instruction.two_sources_encoding.rd, builder_.CreateSub(lhs, llvm::ConstantInt::get(i32_type_, rhs)));
    }
}
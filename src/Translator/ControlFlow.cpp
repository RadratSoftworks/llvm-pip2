#include "../Translator.h"

namespace Pip2 {
    void Translator::create_compare_two_registers_branch(Instruction instruction, llvm::CmpInst::Predicate predicate,
                                                         bool offset_in_instruction) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rd);
        auto rhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto branch_offset = offset_in_instruction ? instruction.two_sources_encoding.rt * 4 : fetch_immediate();

        auto cmp = builder_.CreateICmp(predicate, lhs, rhs);
        builder_.CreateCondBr(cmp, blocks_[current_addr_ + branch_offset], blocks_[current_addr_ + 4]);
    }

    void Translator::create_compare_with_8bit_immediate(Instruction instruction, llvm::CmpInst::Predicate predicate) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rd);
        auto rhs = llvm::ConstantInt::get(i8_type_, instruction.two_sources_encoding.rs);

        auto branch_offset = instruction.two_sources_encoding.rt * 4;

        auto cmp = builder_.CreateICmp(predicate, lhs, rhs);
        builder_.CreateCondBr(cmp, blocks_[current_addr_ + branch_offset], blocks_[current_addr_ + 4]);
    }

    void Translator::BEQ(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_EQ, false);
    }

    void Translator::BNE(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_NE, false);
    }

    void Translator::BGE(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SGE, false);
    }

    void Translator::BGEU(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_UGE, false);
    }

    void Translator::BGT(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SGT, false);
    }

    void Translator::BGTU(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_UGT, false);
    }

    void Translator::BLE(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SLE, false);
    }

    void Translator::BLEU(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_ULE, false);
    }

    void Translator::BLT(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SLT, false);
    }

    void Translator::BLTU(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_ULT, false);
    }

    void Translator::BEQi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_EQ, true);
    }

    void Translator::BNEi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_NE, true);
    }

    void Translator::BGEi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SGE, true);
    }

    void Translator::BGEUi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_UGE, true);
    }

    void Translator::BGTi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SGT, true);
    }

    void Translator::BGTUi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_UGT, true);
    }

    void Translator::BLEi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SLE, true);
    }

    void Translator::BLEUi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_ULE, true);
    }

    void Translator::BLTi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SLT, true);
    }

    void Translator::BLTUi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_ULT, true);
    }

    void Translator::BEQIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_EQ);
    }

    void Translator::BNEIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_NE);
    }

    void Translator::BGEIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_SGE);
    }

    void Translator::BGEUIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_UGE);
    }

    void Translator::BGTIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_SGT);
    }

    void Translator::BGTUIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_UGT);
    }

    void Translator::BLEIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_SLE);
    }

    void Translator::BLEUIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_ULE);
    }

    void Translator::BLTIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_SLT);
    }

    void Translator::BLTUIB(Instruction instruction) {
        create_compare_with_8bit_immediate(instruction, llvm::CmpInst::Predicate::ICMP_ULT);
    }
}
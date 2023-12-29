#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <vector>
#include <string>
#include <map>

#include "VMConfig.h"
#include "Function.h"
#include "Register.h"
#include "Instruction.h"

namespace Pip2
{
    class Translator {
    private:
        typedef void (Translator::*InstructionTranslator)(Instruction);

        const VMConfig &config_;
        const PoolItems &pool_items_;

        llvm::LLVMContext &context_;
        llvm::IRBuilder<> builder_;

        llvm::Type *void_type_;
        llvm::Type *i8_type_;
        llvm::Type *i16_type_;
        llvm::Type *i32_type_;
        llvm::StructType *context_type_;
        llvm::FunctionType *function_type_;

        llvm::Value *current_context_;
        std::uint32_t current_addr_;

        // Blocks of the current translating function
        std::map<std::uint32_t, llvm::BasicBlock*> blocks_;
        std::map<std::uint32_t, llvm::Function*> functions_;

    private:
        void initialize_types();

        void translate_function(llvm::Function *function, const Function &function_info);

        llvm::Value *get_register_pointer(Register reg);
        void set_register(Register dest, llvm::Value *value);

        template <typename T>
        llvm::Value *get_register(Register src);

        std::uint32_t fetch_immediate();

    private:
        void create_compare_two_registers_branch(Instruction instruction, llvm::CmpInst::Predicate predicate,
                                                 bool offset_in_instruction);

        void create_compare_with_8bit_immediate(Instruction instruction, llvm::CmpInst::Predicate predicate);

        void ADD(Instruction instruction);
        void ADDi(Instruction instruction);
        void ADDQ(Instruction instruction);
        void SUB(Instruction instruction);
        void SUBi(Instruction instruction);

        void BEQ(Instruction instruction);
        void BNE(Instruction instruction);
        void BGE(Instruction instruction);
        void BGEU(Instruction instruction);
        void BGT(Instruction instruction);
        void BGTU(Instruction instruction);
        void BLE(Instruction instruction);
        void BLEU(Instruction instruction);
        void BLT(Instruction instruction);
        void BLTU(Instruction instruction);

        void BEQi(Instruction instruction);
        void BNEi(Instruction instruction);
        void BGEi(Instruction instruction);
        void BGEUi(Instruction instruction);
        void BGTi(Instruction instruction);
        void BGTUi(Instruction instruction);
        void BLEi(Instruction instruction);
        void BLEUi(Instruction instruction);
        void BLTi(Instruction instruction);
        void BLTUi(Instruction instruction);

        void BEQIB(Instruction instruction);
        void BNEIB(Instruction instruction);
        void BGEIB(Instruction instruction);
        void BGEUIB(Instruction instruction);
        void BGTIB(Instruction instruction);
        void BGTUIB(Instruction instruction);
        void BLEIB(Instruction instruction);
        void BLEUIB(Instruction instruction);
        void BLTIB(Instruction instruction);
        void BLTUIB(Instruction instruction);

    private:
        InstructionTranslator instruction_translators_[Opcode::TotalOpcodes] = {
            nullptr, &Translator::ADD, nullptr, nullptr, nullptr,       // 0x00
            nullptr, nullptr, nullptr, &Translator::SUB, nullptr,       // 0x05
            nullptr, nullptr, nullptr, nullptr, nullptr,            // 0x0A
            nullptr, nullptr, nullptr, nullptr, nullptr,            // 0x0F
            nullptr, nullptr, nullptr, nullptr, nullptr,            // 0x14
            nullptr, nullptr, nullptr, nullptr, nullptr,            // 0x19
            nullptr, &Translator::ADDQ, nullptr, nullptr, nullptr,            // 0x1E
            nullptr, nullptr, nullptr, nullptr, nullptr,            // 0x23
            nullptr, nullptr, nullptr, nullptr, &Translator::BEQi,            // 0x28
            &Translator::BNEi, &Translator::BGEi, &Translator::BGEUi, &Translator::BGTi, &Translator::BGTUi,            // 0x2D
            &Translator::BLEi, &Translator::BLEUi, &Translator::BLTi, &Translator::BLTUi, &Translator::BEQIB,            // 0x32
            &Translator::BNEIB, &Translator::BGEIB, &Translator::BGEUIB, &Translator::BGTIB, &Translator::BGTUIB,            // 0x37
            &Translator::BLEIB, &Translator::BLEUIB, &Translator::BLTIB, &Translator::BLTUIB, nullptr,            // 0x3C
            nullptr, nullptr, nullptr, nullptr, nullptr,            // 0x41
            nullptr, nullptr, nullptr, nullptr, &Translator::ADDi,            // 0x46
            nullptr, nullptr, nullptr, nullptr, nullptr,            // 0x4B
            nullptr, &Translator::SUBi, nullptr, nullptr, nullptr,            // 0x50
            nullptr, nullptr, nullptr, nullptr, nullptr,            // 0x55
            nullptr, nullptr, nullptr, &Translator::BEQ, &Translator::BNE,            // 0x5A
            &Translator::BGE, &Translator::BGEU, &Translator::BGT, &Translator::BGTU, &Translator::BLE,            // 0x5F
            &Translator::BLEU, &Translator::BLT, &Translator::BLTU, nullptr, nullptr,            // 0x64
        };

    public:
        explicit Translator(llvm::LLVMContext &context, const VMConfig &config, const PoolItems &pool_items);

        llvm::Module *translate(const std::string &module_name, const std::vector<Function> &functions);
    };
}
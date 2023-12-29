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
        void SUBQ(Instruction instruction);

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

    public:
        explicit Translator(llvm::LLVMContext &context, const VMConfig &config, const PoolItems &pool_items);

        llvm::Module *translate(const std::string &module_name, const std::vector<Function> &functions);
    };
}
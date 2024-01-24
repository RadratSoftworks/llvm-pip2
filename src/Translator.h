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
#include "VMOptions.h"

namespace Pip2
{
    class Translator
    {
    private:
        struct JumpTableTranslateState {
            llvm::Value *case_value_;

            std::uint32_t case_value_resolved_addr_;
            Register case_value_register_;

            explicit JumpTableTranslateState(std::uint32_t case_value_resolved_addr, Register case_value_register)
                : case_value_(nullptr), case_value_resolved_addr_(case_value_resolved_addr), case_value_register_(case_value_register) {
            }
        };

        typedef void (Translator::*InstructionTranslator)(Instruction);

        const VMConfig &config_;
        const VMOptions &options_;

        llvm::LLVMContext &context_;
        llvm::IRBuilder<> builder_;

        llvm::Type *void_type_;
        llvm::Type *i8_type_;
        llvm::Type *i16_type_;
        llvm::Type *i32_type_;
        llvm::Type *i64_type_;
        llvm::StructType *context_type_;
        llvm::FunctionType *function_type_;
        llvm::FunctionType *hle_handler_function_type_;

        llvm::Value *current_context_;
        llvm::Value *current_memory_base_;
        llvm::Value *current_function_lookup_array_;
        llvm::Value *current_hle_handler_pointer_;
        llvm::Value *current_hle_handler_userdata_;
        llvm::FunctionCallee current_hle_handler_callee_;
        llvm::Function *current_function_;

        std::array<llvm::FunctionType*, 5> std_call_type_;
        std::array<llvm::FunctionType*, 5> std_call_type_with_return_;

        std::map<SpecialPoolFunction, llvm::Function *> special_functions_;
        llvm::FunctionType *wrapper_function_type_;

        std::uint32_t current_addr_;
        const Function *current_function_analysis_;

        // Blocks of the current translating function
        std::map<std::uint32_t, llvm::BasicBlock *> blocks_;
        std::map<std::uint32_t, llvm::Function *> functions_;
        std::map<std::uint32_t, JumpTableTranslateState> current_function_jump_table_translate_state_;

    private:
        void initialize_types();

        void translate_function(llvm::Function *function, const Function &function_info);
        void generate_entry_point_function(std::uint32_t entry_point_addr);
        void generate_hle_handler_trampoline(llvm::Module *module);

        llvm::Value *get_register_pointer(Register reg);
        llvm::Value *get_memory_pointer(llvm::Value *address);

        void set_register(Register dest, llvm::Value *value);
        void update_pc_to_next_instruction();

        template <typename T>
        llvm::Value *get_register(Register src);

        std::uint32_t fetch_immediate();
        llvm::Type *get_pointer_integer_type();

        void call_special_function(SpecialPoolFunction function);

    private:
        void create_compare_two_registers_branch(Instruction instruction, llvm::CmpInst::Predicate predicate,
                                                 bool offset_in_instruction, bool signed_compare);

        void create_compare_with_8bit_immediate(Instruction instruction, llvm::CmpInst::Predicate predicate);

        void ADD(Instruction instruction);
        void ADDi(Instruction instruction);
        void ADDH(Instruction instruction);
        void ADDHi(Instruction instruction);
        void ADDB(Instruction instruction);
        void ADDBi(Instruction instruction);
        void ADDQ(Instruction instruction);

        void SUB(Instruction instruction);
        void SUBi(Instruction instruction);
        void SUBH(Instruction instruction);
        void SUBB(Instruction instruction);

        void MUL(Instruction instruction);
        void MULQ(Instruction instruction);
        void MULi(Instruction instruction);

        void DIV(Instruction instruction);
        void DIVU(Instruction instruction);
        void DIVi(Instruction instruction);
        void DIVUi(Instruction instruction);

        void MOV(Instruction instruction);
        void MOVB(Instruction instruction);
        void MOVH(Instruction instruction);

        void SLL(Instruction instruction);
        void SLLi(Instruction instruction);
        void SLLH(Instruction instruction);
        void SLLB(Instruction instruction);

        void SRL(Instruction instruction);
        void SRLi(Instruction instruction);
        void SRLH(Instruction instruction);
        void SRLB(Instruction instruction);

        void SRA(Instruction instruction);
        void SRAi(Instruction instruction);
        void SRAH(Instruction instruction);
        void SRAB(Instruction instruction);

        void NOT(Instruction instruction);
        void NEG(Instruction instruction);
        void EXSB(Instruction instruction);
        void EXSH(Instruction instruction);
        void NOP(Instruction instruction);

        void AND(Instruction instruction);
        void ANDi(Instruction instruction);
        void ANDH(Instruction instruction);
        void ANDHi(Instruction instruction);
        void ANDB(Instruction instruction);
        void ANDBi(Instruction instruction);

        void OR(Instruction instruction);
        void ORi(Instruction instruction);
        void ORH(Instruction instruction);
        void ORB(Instruction instruction);
        void ORBi(Instruction instruction);

        void XOR(Instruction instruction);
        void XORi(Instruction instruction);

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
        void CALLl(Instruction instruction);
        void JPl(Instruction instruction);
        void JPr(Instruction instruction);
        void CALLr(Instruction instruction);

        void LDQ(Instruction instruction);
        void LDI(Instruction instruction);
        void LDWd(Instruction instruction);
        void LDHUd(Instruction instruction);
        void LDHd(Instruction instruction);
        void LDBUd(Instruction instruction);
        void LDBd(Instruction instruction);

        void STWd(Instruction instruction);
        void STHd(Instruction instruction);
        void STBd(Instruction instruction);

        void STORE(Instruction instruction);
        void RESTORE(Instruction instruction);
        void RET(Instruction instruction);

        void SYSCPY(Instruction instruction);
        void SYSSET(Instruction instruction);

        void SLEEP(Instruction instruction);
        void KILLTASK(Instruction instruction);

    private:
        InstructionTranslator instruction_translators_[Opcode::TotalOpcodes] = {
            &Translator::NOP, &Translator::NOP, &Translator::ADD, &Translator::AND, &Translator::MUL,                                                 // 0x00
            &Translator::DIV, &Translator::DIVU, &Translator::OR, &Translator::XOR, &Translator::SUB,                                                 // 0x05
            &Translator::SLL, &Translator::SRA, &Translator::SRL, &Translator::NOT, &Translator::NEG,                                                          // 0x0A
            &Translator::EXSB, &Translator::EXSH, &Translator::MOV, &Translator::ADDB, &Translator::SUBB,                                                          // 0x0F
            &Translator::ANDB, &Translator::ORB, &Translator::MOVB, &Translator::ADDH, &Translator::SUBH,                                                          // 0x14
            &Translator::ANDH, &Translator::ORH, &Translator::MOVH, &Translator::SLLi, &Translator::SRAi,                                                          // 0x19
            &Translator::SRLi, &Translator::ADDQ, &Translator::MULQ, &Translator::ADDBi, &Translator::ANDBi,                                                // 0x1E
            &Translator::ORBi, &Translator::SLLB, &Translator::SRLB, &Translator::SRAB, &Translator::ADDHi,                                                          // 0x23
            &Translator::ANDHi, &Translator::SLLH, &Translator::SRLH, &Translator::SRAH, &Translator::BEQi,                                                // 0x28
            &Translator::BNEi, &Translator::BGEi, &Translator::BGEUi, &Translator::BGTi, &Translator::BGTUi,      // 0x2D
            &Translator::BLEi, &Translator::BLEUi, &Translator::BLTi, &Translator::BLTUi, &Translator::BEQIB,     // 0x32
            &Translator::BNEIB, &Translator::BGEIB, &Translator::BGEUIB, &Translator::BGTIB, &Translator::BGTUIB, // 0x37
            &Translator::BLEIB, &Translator::BLEUIB, &Translator::BLTIB, &Translator::BLTUIB, &Translator::LDQ,            // 0x3C
            &Translator::JPr, &Translator::CALLr, &Translator::STORE, &Translator::RESTORE, &Translator::RET,                                                          // 0x41
            &Translator::KILLTASK, &Translator::SLEEP, &Translator::SYSCPY, &Translator::SYSSET, &Translator::ADDi,                                                // 0x46
            &Translator::ANDi, &Translator::MULi, &Translator::DIVi, &Translator::DIVUi, &Translator::ORi,                                                          // 0x4B
            &Translator::XORi, &Translator::SUBi, &Translator::STBd, &Translator::STHd, &Translator::STWd,                                                // 0x50
            &Translator::LDBd, &Translator::LDHd, &Translator::LDWd, &Translator::LDBUd, &Translator::LDHUd,                                                          // 0x55
            &Translator::LDI, &Translator::JPl, &Translator::CALLl, &Translator::BEQ, &Translator::BNE,                                        // 0x5A
            &Translator::BGE, &Translator::BGEU, &Translator::BGT, &Translator::BGTU, &Translator::BLE,           // 0x5F
            &Translator::BLEU, &Translator::BLT, &Translator::BLTU, nullptr, nullptr,                             // 0x64
        };

    public:
        explicit Translator(llvm::LLVMContext &context, const VMConfig &config, const VMOptions &options);

        std::unique_ptr<llvm::Module> translate(const std::string &module_name, const std::vector<Function> &functions);
    };
}
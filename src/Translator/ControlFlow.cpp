#include "../Translator.h"
#include "../Common.h"
#include "Constants.h"

namespace Pip2 {
    void Translator::create_compare_two_registers_branch(Instruction instruction, llvm::CmpInst::Predicate predicate,
                                                         bool offset_in_instruction, bool signed_compare) {
        auto lhs = get_register<std::uint32_t>(instruction.two_sources_encoding.rd);
        auto rhs = offset_in_instruction ?
                (signed_compare ? builder_.getInt32(Common::sign_extend(instruction.two_sources_encoding.rt)) : builder_.getInt32(instruction.two_sources_encoding.rt)) :
                get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto branch_offset = offset_in_instruction ? static_cast<std::int8_t>(instruction.two_sources_encoding.rt) * 4 : (static_cast<std::int32_t>(fetch_immediate()) - 4);

        auto cmp = builder_.CreateICmp(predicate, lhs, rhs);
        builder_.CreateCondBr(cmp, blocks_[current_addr_ + branch_offset], blocks_[current_addr_ + 4]);
    }

    void Translator::create_compare_with_8bit_immediate(Instruction instruction, llvm::CmpInst::Predicate predicate) {
        auto lhs = get_register<std::uint8_t>(instruction.two_sources_encoding.rd);
        auto rhs = builder_.getInt8( instruction.two_sources_encoding.rs);

        auto branch_offset = static_cast<std::int8_t>(instruction.two_sources_encoding.rt) * 4;

        auto cmp = builder_.CreateICmp(predicate, lhs, rhs);
        builder_.CreateCondBr(cmp, blocks_[current_addr_ + branch_offset], blocks_[current_addr_ + 4]);
    }

    void Translator::BEQ(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_EQ, false, false);
    }

    void Translator::BNE(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_NE, false, false);
    }

    void Translator::BGE(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SGE, false, true);
    }

    void Translator::BGEU(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_UGE, false, false);
    }

    void Translator::BGT(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SGT, false, true);
    }

    void Translator::BGTU(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_UGT, false, false);
    }

    void Translator::BLE(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SLE, false, true);
    }

    void Translator::BLEU(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_ULE, false, false);
    }

    void Translator::BLT(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SLT, false, true);
    }

    void Translator::BLTU(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_ULT, false, false);
    }

    void Translator::BEQi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_EQ, true, false);
    }

    void Translator::BNEi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_NE, true, false);
    }

    void Translator::BGEi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SGE, true, true);
    }

    void Translator::BGEUi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_UGE, true, false);
    }

    void Translator::BGTi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SGT, true, true);
    }

    void Translator::BGTUi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_UGT, true, false);
    }

    void Translator::BLEi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SLE, true, true);
    }

    void Translator::BLEUi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_ULE, true, false);
    }

    void Translator::BLTi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_SLT, true, true);
    }

    void Translator::BLTUi(Instruction instruction) {
        create_compare_two_registers_branch(instruction, llvm::CmpInst::Predicate::ICMP_ULT, true, false);
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

    void Translator::JPl(Instruction instruction) {
        auto address = current_addr_ + static_cast<std::int32_t>(fetch_immediate());
        builder_.CreateBr(blocks_[address]);
    }

    void Translator::update_pc_to_next_instruction()
    {
        set_register(Register::PC, builder_.getInt32(current_addr_ + INSTRUCTION_SIZE));
    }

    void Translator::CALLl(Instruction instruction) {
        auto next_word = *reinterpret_cast<std::uint32_t*>(config_.memory_base() + current_addr_ + 4);
        current_addr_ += 4;

        if (std::optional<std::uint32_t> imm_offset = Common::get_immediate_pip_dword(next_word)) {
            auto address = current_addr_ + static_cast<std::int32_t>(imm_offset.value());

            set_register(Register::RA, builder_.getInt32(current_addr_ + INSTRUCTION_SIZE));
            set_register(Register::PC, builder_.getInt32(address));

            builder_.CreateCall(functions_[address], {
                current_context_,
                current_memory_base_,
                current_function_lookup_array_,
                current_hle_handler_pointer_
            });
        } else {
            if (config_.pool_items().is_pool_item_constant(next_word)) {
                auto function_addr = config_.pool_items().get_pool_item_constant(next_word);

                set_register(Register::RA, builder_.getInt32(current_addr_ + INSTRUCTION_SIZE));
                set_register(Register::PC, builder_.getInt32(function_addr));

                builder_.CreateCall(functions_[function_addr], {
                        current_context_,
                        current_memory_base_,
                        current_function_lookup_array_,
                        current_hle_handler_pointer_
                });
            } else {
                update_pc_to_next_instruction();

                builder_.CreateCall(current_hle_handler_callee_, { builder_.getInt32(next_word) });

                if (config_.pool_items().is_pool_item_terminate_function(next_word))
                {
                    builder_.CreateRetVoid();
                }
            }
        }
    }

    void Translator::JPr(Instruction instruction)
    {
        if (instruction.two_sources_encoding.rd == Register::RA)
        {
            set_register(Register::PC, get_register<std::uint32_t>(instruction.two_sources_encoding.rd));
            builder_.CreateRetVoid();
            return;
        }

        auto jump_table = std::find_if(
                current_function_analysis_->jump_tables_.begin(),
                current_function_analysis_->jump_tables_.end(),
                [this](const auto &jt) {
                    return jt.jump_instruction_addr_ == current_addr_;
                });

        if (jump_table == current_function_analysis_->jump_tables_.end())
        {
            auto target = get_register<std::uint32_t>(instruction.two_sources_encoding.rd);

            auto func_ptr_ptr = builder_.CreateGEP(get_pointer_integer_type(), current_function_lookup_array_, {
                    builder_.CreateLShr(target, builder_.getInt32(2))
            });

            auto func_ptr = builder_.CreateLoad(get_pointer_integer_type(), func_ptr_ptr);
            auto func_callee = llvm::FunctionCallee(function_type_, builder_.CreateIntToPtr(func_ptr, function_type_->getPointerTo()));

            set_register(Register::PC, target);

            builder_.CreateCall(func_callee, {
                    current_context_,
                    current_memory_base_,
                    current_function_lookup_array_,
                    current_hle_handler_pointer_
            });

            builder_.CreateRetVoid();
        }
        else
        {
            // Do manual switch. We don't cut off instructions (preserve at least the value accuracy)
            auto switch_addr = get_register<std::uint32_t>(instruction.two_sources_encoding.rd);
            auto switch_value = builder_.CreateLShr(
                    builder_.CreateSub(switch_addr,builder_.getInt32(jump_table->jump_table_base_addr_)),
                    builder_.getInt32(2));

            // Grab a default to satisfy LLVM
            auto default_case = blocks_[jump_table->labels_[0]];
            auto switch_cases = builder_.CreateSwitch(switch_value, default_case, jump_table->labels_.size());
            for (std::size_t i = 0; i < jump_table->labels_.size(); ++i)
            {
                switch_cases->addCase(builder_.getInt32(i), blocks_[jump_table->labels_[i]]);
            }
        }
    }

    void Translator::CALLr(Instruction instruction)
    {
        auto target = get_register<std::uint32_t>(instruction.two_sources_encoding.rd);
        auto func_ptr_ptr = builder_.CreateGEP(get_pointer_integer_type(), current_function_lookup_array_, {
                builder_.CreateLShr(target,builder_.getInt32(2))
        });

        auto func_ptr = builder_.CreateLoad(get_pointer_integer_type(), func_ptr_ptr);
        auto func_callee = llvm::FunctionCallee(function_type_, builder_.CreateIntToPtr(func_ptr, function_type_->getPointerTo()));

        set_register(Register::RA, builder_.getInt32(current_addr_ + INSTRUCTION_SIZE));
        set_register(Register::PC, target);

        builder_.CreateCall(func_callee, {
                current_context_,
                current_memory_base_,
                current_function_lookup_array_,
                current_hle_handler_pointer_
        });
    }

    void Translator::RET(Instruction instruction)
    {
        RESTORE(instruction);

        set_register(Register::PC, get_register<std::uint32_t>(Register::RA));
        builder_.CreateRetVoid();
    }
}
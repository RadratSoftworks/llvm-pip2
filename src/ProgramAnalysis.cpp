#include "ProgramAnalysis.h"
#include "Instruction.h"
#include "Common.h"
#include "Constants.h"

#include <optional>
#include <stdexcept>
#include <algorithm>

/**
 * The analysis process makes some assumptions about the compiled assembly, primarily:
 *
 * 1. Link jump using CALLl is a linked function call, and the function is expected to return, resuming the control flow.
 * 2. Indirect branch is not obfuscated, meaning that the branching address is not computed at runtime, rather it's stored somewhere.
 *
 * Short jump using BL calls will be put into an investigation queue to see if it belongs to the current compiling function or not. If the function
 * ends prematurely (by popping the stack and returning to RA), short jump will be deferred to a analyse queue.
 *
 * The analyser will also try its best to find out where it will jump to (this is not always possible, especially with vtable). If the analyser can't find it,
 * then we will leave it to the runtime to recompile the indirect jump code block. But if we managed to detect it, the same treatment as short jump will be applied.
 *
 * So, overall, the analyser should work OK, if the assembly is:
 *
 * 1. Not obfuscated
 * 2. Compiler emits the assembly
 * 3. Hand-written assembly but people writing it is sane.
 *
 * Some notes:
 *
 * 1. Jump table
 *
 * Mophun executable jump table usually has this pattern:
 *
 * Pattern 1:
 *
 * bgtui $s0,11,36
 * sll	$r0,$s0,2
 * ldw	$r0,$r0,49
 * jp	$r0
 *
 * Pattern 2:
 *
 * bleui	$r0,4,12
 * jp	0xfffffc2c
 * sll	$r0,$r0,2
 * ldw	$r0,$r0,353
 * jp	$r0
 *
 * Where 49/353 is the pool item index containing the jump table base. The size of the jump table lies in either the bgt or the ble argument (which is 12/4 in this case).
 *
 * So far, these twos are the only jump table patterns that I found, they mainly make use of the temporary register r0.
 *
 * 2. Vtable
 *
 * So far, can't see any Vtable in Da Vinci's Code and Carmageddon. These twos are kind of the AAA of Mophun ecosystem already, but let just sees.
 */

namespace Pip2
{
    static bool is_direct_branch(const Instruction &instruction)
    {
        switch (instruction.word_encoding.opcode)
        {
        case Opcode::BEQ:
        case Opcode::BEQI:
        case Opcode::BEQIB:
        case Opcode::BNE:
        case Opcode::BNEI:
        case Opcode::BNEIB:
        case Opcode::BLT:
        case Opcode::BLTI:
        case Opcode::BLTIB:
        case Opcode::BLTU:
        case Opcode::BLTUI:
        case Opcode::BLTUIB:
        case Opcode::BLE:
        case Opcode::BLEI:
        case Opcode::BLEIB:
        case Opcode::BLEU:
        case Opcode::BLEUI:
        case Opcode::BLEUIB:
        case Opcode::BGT:
        case Opcode::BGTI:
        case Opcode::BGTIB:
        case Opcode::BGTU:
        case Opcode::BGTUI:
        case Opcode::BGTUIB:
        case Opcode::BGE:
        case Opcode::BGEI:
        case Opcode::BGEIB:
        case Opcode::BGEU:
        case Opcode::BGEUI:
        case Opcode::BGEUIB:
        case Opcode::JPl:
        case Opcode::CALLl:
            return true;
        default:
            return false;
        }
    }

    static bool is_indirect_branch(const Instruction &instruction)
    {
        switch (instruction.word_encoding.opcode)
        {
        case Opcode::JPr:
        case Opcode::CALLr: // Like ARM BL
        case Opcode::RET:
            return true;
        default:
            return false;
        }
    }

    static bool is_branch_offset_encoded_in_instruction(const Instruction &instruction) {
        switch (instruction.word_encoding.opcode) {
        case Opcode::BEQI:
        case Opcode::BEQIB:
        case Opcode::BNEI:
        case Opcode::BNEIB:
        case Opcode::BLTI:
        case Opcode::BLTIB:
        case Opcode::BLTUI:
        case Opcode::BLTUIB:
        case Opcode::BLEI:
        case Opcode::BLEIB:
        case Opcode::BLEUI:
        case Opcode::BLEUIB:
        case Opcode::BGTI:
        case Opcode::BGTIB:
        case Opcode::BGTUI:
        case Opcode::BGTUIB:
        case Opcode::BGEI:
        case Opcode::BGEIB:
        case Opcode::BGEUI:
        case Opcode::BGEUIB:
            return true;

            break;
        default:
            return false;
        }
    }

    std::optional<std::uint32_t> calculate_direct_jump_target(const PoolItems &pool_items,
                                                              const Instruction &instruction,
                                                              const std::uint32_t addr,
                                                              const std::uint32_t next_word,
                                                              bool &next_word_consumed)
    {
        next_word_consumed = false;

        if (!is_direct_branch(instruction))
        {
            return 0;
        }

        // Calculate the branch target
        if (is_branch_offset_encoded_in_instruction(instruction))
        {
            return addr + instruction.two_sources_encoding.rt * 4;
        }
        else
        {
            next_word_consumed = true;

            auto jump_offset = Common::read_immediate(pool_items, next_word);
            if (!jump_offset.has_value())
            {
                return std::nullopt;
            }

            return addr + jump_offset.value() * 4;
        }
    }

    void ProgramAnalysis::add_to_function_analyse_queue(std::uint32_t addr) {
        if (addr < text_base_) {
            throw std::runtime_error("Address is out of text segment");
        }

        analyse_queue_.push(addr - text_base_);
    }

    Function ProgramAnalysis::sweep_function(std::uint32_t addr) {
        bool is_block_end_reached = false;

        Function result_function;
        std::vector<std::uint32_t> suspecting_to_be_labels;

        while (!is_block_end_reached)
        {
            const auto opcode = memory_base_[addr >> 2];
            const Instruction instruction{opcode};

            // Direct branch, we can follow it!
            if (is_direct_branch(instruction))
            {
                bool is_next_dword_consumed = false;
                auto jump_target = calculate_direct_jump_target(pool_items_,
                                                                instruction,
                                                                addr,
                                                                memory_base_[(addr >> 2) + 1],
                                                                is_next_dword_consumed);

                // If jump target is undefined, there are two possibilities:
                // 1. It's a HLE call (call to Mophun's library), in that case it's fine
                // 2. Failed to calculate jump target, in that case we should throw an exception
                if (!jump_target.has_value() && (instruction.word_encoding.opcode != Opcode::CALLl))
                {
                    throw std::runtime_error("Failed to calculate jump target");
                }

                if (is_next_dword_consumed)
                {
                    addr += INSTRUCTION_SIZE;
                }

                if (jump_target.has_value())
                {
                    if (instruction.word_encoding.opcode == Opcode::CALLl)
                    {
                        // Probably a long call to another function
                        add_to_function_analyse_queue(jump_target.value());
                    }
                    else
                    {
                        // Probably loop, don't analyse it, just add a label
                        // As loop is kind of broad in terms of the condition, we probably will just let LLVM do the job
                        // of detect and optimize potential loop
                        if (jump_target.value() < addr) {
                            if (std::find(result_function.labels_.begin(), result_function.labels_.end(), jump_target.value()) == result_function.labels_.end()) {
                                result_function.labels_.push_back(jump_target.value());
                            }
                        }
                        else {
                            suspecting_to_be_labels.push_back(jump_target.value());
                        }
                    }
                }
            }
            else if (is_indirect_branch(instruction))
            {
                /**
                 * Some assumption:
                 *
                 * 1. CALL instruction seems to mostly be used for vtable call, so we probably will keep going,
                 *    but we are not able to find out which function it's calling to.
                 *
                 * 2. JP/RET instruction with RA register is mostly just a statement to return to the caller. But
                 *    if we were to be safe, we should still detect if it's jump table or not
                 *
                 * This code block will try its best to detect a jump table and store it right away, so that the emitted
                 * switch code will has its jump target detected at compile time.
                 */

                // Confirm if it has a jump table pattern
                Instruction potential_offset_calc_instruction{memory_base_[(addr >> 2) - 2]};
                Instruction potential_table_base_load_instruction{ memory_base_[(addr >> 2) - 1] };

                if (potential_offset_calc_instruction.word_encoding.opcode == Opcode::SLLi &&
                    potential_table_base_load_instruction.word_encoding.opcode == Opcode::LDWd &&
                    potential_offset_calc_instruction.two_sources_encoding.rt == 2)
                {
                    JumpTable jump_table;
                    jump_table.jump_instruction_addr_ = addr;

                }

                is_block_end_reached = true;
            }
            else
            {
                // Keep going with the block
            }

            addr += INSTRUCTION_SIZE;
        }

        return result_function;
    }

    std::vector<Function> ProgramAnalysis::analyze(const std::uint32_t entry_point_addr)
    {
        std::vector<Function> results;

        while (!analyse_queue_.empty())
            analyse_queue_.pop();

        analyse_queue_.push(entry_point_addr);

        while (!analyse_queue_.empty())
        {
            auto addr = text_base_ + analyse_queue_.front();
            analyse_queue_.pop();

            if (addr < text_base_ || addr >= text_base_ + text_size_)
            {
                continue;
            }

            results.push_back(sweep_function(addr));
        }

        return results;
    }
}
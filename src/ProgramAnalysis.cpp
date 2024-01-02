#include "ProgramAnalysis.h"
#include "Instruction.h"
#include "Common.h"
#include "Constants.h"

#include <optional>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <map>

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

    static bool is_branch_offset_encoded_in_instruction(const Instruction &instruction)
    {
        switch (instruction.word_encoding.opcode)
        {
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

        default:
            return false;
        }
    }

    static bool does_non_branch_instruction_consume_dword(const Instruction &instruction)
    {
        switch (instruction.word_encoding.opcode)
        {
        case Opcode::STWd:
        case Opcode::STHd:
        case Opcode::STBd:
        case Opcode::LDI:
        case Opcode::LDWd:
        case Opcode::LDHUd:
        case Opcode::LDHd:
        case Opcode::LDBUd:
        case Opcode::LDBd:
        case Opcode::ADDi:
        case Opcode::SUBi:
        case Opcode::MULi:
        case Opcode::DIVi:
        case Opcode::DIVUi:
        case Opcode::ANDi:
        case Opcode::ORi:
        case Opcode::XORi:
            return true;

        default:
            return false;
        }
    }

    std::optional<std::uint32_t> calculate_direct_jump_target(const PoolItems &pool_items,
                                                              const Instruction &instruction,
                                                              const std::uint32_t addr,
                                                              const std::uint32_t next_word,
                                                              bool &next_word_consumed,
                                                              bool &is_termination)
    {
        next_word_consumed = false;
        is_termination = false;

        if (!is_direct_branch(instruction))
        {
            return 0;
        }

        // Calculate the branch target
        if (is_branch_offset_encoded_in_instruction(instruction))
        {
            return addr + static_cast<std::int8_t>(instruction.two_sources_encoding.rt) * 4;
        }
        else
        {
            next_word_consumed = true;

            if (std::optional<std::uint32_t> immediate = Common::get_immediate_pip_dword(next_word))
            {
                return addr + static_cast<std::int32_t>(immediate.value());
            }

            if (pool_items.is_pool_item_terminate_function(next_word))
            {
                is_termination = true;
                return std::nullopt;
            }

            if (pool_items.is_pool_item_constant(next_word))
            {
                return pool_items.get_pool_item_constant(next_word);
            }

            return std::nullopt;
        }
    }

    void ProgramAnalysis::add_to_function_analyse_queue(std::uint32_t addr)
    {
        if (addr < text_base_)
        {
            throw std::runtime_error("Address is out of text segment");
        }

        if (!found_functions_.contains(addr - text_base_))
        {
            analyse_queue_.push(addr - text_base_);
            found_functions_.emplace(addr - text_base_);
        }
    }

    Function ProgramAnalysis::sweep_function(std::uint32_t addr)
    {
        Function result_function;
        result_function.addr_ = addr;

        std::set<std::uint32_t> suspecting_to_be_labels;
        std::vector<std::uint32_t> unfinished_blocks_left;

        unfinished_blocks_left.push_back(addr);
        result_function.labels_.insert(addr);

        std::uint32_t current_going_through_block = addr;

        auto mark_block_finished = [&unfinished_blocks_left, &current_going_through_block]()
        {
            auto unfinished_block_ite = std::lower_bound(unfinished_blocks_left.begin(), unfinished_blocks_left.end(), current_going_through_block);

            if (unfinished_block_ite != unfinished_blocks_left.end() && *unfinished_block_ite == current_going_through_block)
            {
                unfinished_blocks_left.erase(unfinished_block_ite);
            }
            else
            {
                throw std::runtime_error("Unfinished block not found in list");
            }
        };

        while (true)
        {
            auto existing_suspecting_label = suspecting_to_be_labels.find(addr);
            if (existing_suspecting_label != suspecting_to_be_labels.end())
            {
                // Found a label, add it to the result function
                result_function.labels_.insert(addr);
                suspecting_to_be_labels.erase(existing_suspecting_label);

                if (!unfinished_blocks_left.empty())
                {
                    mark_block_finished();
                }

                current_going_through_block = addr;
                unfinished_blocks_left.push_back(addr);
            }
            else
            {
                // No more block to be expected
                if (unfinished_blocks_left.empty())
                {
                    result_function.length_ = addr - result_function.addr_;
                    break;
                }
            }

            const auto opcode = memory_base_[addr >> 2];
            const Instruction instruction{opcode};

            // Direct branch, we can follow it!
            if (is_direct_branch(instruction))
            {
                bool is_next_dword_consumed = false;
                bool is_terminate_function = false;

                auto jump_target = calculate_direct_jump_target(pool_items_,
                                                                instruction,
                                                                addr,
                                                                memory_base_[(addr >> 2) + 1],
                                                                is_next_dword_consumed,
                                                                is_terminate_function);

                if (is_terminate_function)
                {
                    // Terminate function ends the function prematurely!
                    mark_block_finished();
                }
                else
                {
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
                            // Take a risk and not cut off the function after call (hopefully they are all neutral)
                            // When a jp ra is found, the function will do a normal return instead
                            add_to_function_analyse_queue(jump_target.value());
                        }
                        else
                        {
                            // Probably loop, don't analyse it, just add a label
                            // As loop is kind of broad in terms of the condition, we probably will just let LLVM do the job
                            // of detect and optimize potential loop
                            if (jump_target.value() < addr)
                            {
                                if (std::find(result_function.labels_.begin(), result_function.labels_.end(), jump_target.value()) == result_function.labels_.end())
                                {
                                    result_function.labels_.insert(jump_target.value());
                                }
                            }
                            else
                            {
                                suspecting_to_be_labels.insert(jump_target.value());
                            }

                            mark_block_finished();

                            // Non conditional block won't continue the current flow
                            if (instruction.word_encoding.opcode != Opcode::JPl && instruction.word_encoding.opcode != Opcode::CALLl)
                            {
                                current_going_through_block = addr + INSTRUCTION_SIZE;

                                unfinished_blocks_left.push_back(addr + INSTRUCTION_SIZE);
                                result_function.labels_.insert(addr + INSTRUCTION_SIZE);
                            }
                        }
                    }
                }
            }
            else if (is_indirect_branch(instruction))
            {
                /**
                 * Some assumption:
                 *
                 * 1. CALL instruction seems to mostly be used for vtable call, so we are not able to find out which
                 *    function it's calling to.
                 *
                 * 2. JP/RET instruction with RA register is mostly just a statement to return to the caller. But
                 *    if we were to be safe, we should still detect if it's jump table or not
                 *
                 * This code block will try its best to detect a jump table and store it right away, so that the emitted
                 * switch code will has its jump target detected at compile time.
                 */

                bool block_ending_sign_appear = true;

                // Take a risk and cut off when jump to RA
                if (instruction.two_sources_encoding.opcode == Opcode::JPr && instruction.two_sources_encoding.rd != Register::RA)
                {
                    // Confirm if it has a jump table pattern
                    Instruction potential_offset_calc_instruction{memory_base_[(addr >> 2) - 3]};
                    Instruction potential_table_base_load_instruction{memory_base_[(addr >> 2) - 2]};

                    if (potential_offset_calc_instruction.word_encoding.opcode == Opcode::SLLi &&
                        potential_table_base_load_instruction.word_encoding.opcode == Opcode::LDWd &&
                        potential_offset_calc_instruction.two_sources_encoding.rt == 2)
                    {
                        JumpTable jump_table;
                        jump_table.jump_instruction_addr_ = addr;

                        std::uint8_t indexing_register = potential_offset_calc_instruction.two_sources_encoding.rd;
                        std::uint8_t index_source_register = potential_offset_calc_instruction.two_sources_encoding.rs;
                        std::uint32_t switch_start_addr = addr - INSTRUCTION_SIZE * 3;

                        // There may be an AND instruction before SLL too, to cast down the integer, allow it
                        Instruction potential_and_instruction{memory_base_[(addr >> 2) - 4]};
                        if (potential_and_instruction.two_sources_encoding.opcode == Opcode::ANDi &&
                            potential_and_instruction.two_sources_encoding.rd == indexing_register)
                        {
                            switch_start_addr -= INSTRUCTION_SIZE;
                        }

                        std::size_t total_cases = std::numeric_limits<std::size_t>::max();

                        // Should be a BGT/BGTUI
                        Instruction possible_branch_instruction{memory_base_[(switch_start_addr >> 2) - 1]};
                        if ((possible_branch_instruction.two_sources_encoding.opcode == Opcode::BGTUI) ||
                            (possible_branch_instruction.two_sources_encoding.opcode == Opcode::BGTI) ||
                            (possible_branch_instruction.two_sources_encoding.opcode == Opcode::BGTIB) ||
                            (possible_branch_instruction.two_sources_encoding.opcode == Opcode::BGTUIB))
                        {
                            if (possible_branch_instruction.two_sources_encoding.rd == index_source_register)
                            {
                                total_cases = possible_branch_instruction.two_sources_encoding.rs + 1;
                            }
                        }

                        if (total_cases == std::numeric_limits<std::size_t>::max())
                        {
                            // Probably a BLE instruction that is containing case count
                            static constexpr std::size_t MAX_TRACE_BACK = 50;

                            for (auto i = 0; i < MAX_TRACE_BACK; i++)
                            {
                                Instruction tracing_instruction{memory_base_[(switch_start_addr >> 2) - (i + 1)]};

                                if ((possible_branch_instruction.two_sources_encoding.opcode == Opcode::BLEI) ||
                                    (possible_branch_instruction.two_sources_encoding.opcode == Opcode::BLEIB) ||
                                    (possible_branch_instruction.two_sources_encoding.opcode == Opcode::BLEUI) ||
                                    (possible_branch_instruction.two_sources_encoding.opcode == Opcode::BLEUIB))
                                {
                                    if (possible_branch_instruction.two_sources_encoding.rd == indexing_register)
                                    {
                                        bool temp_word_consumed = false;
                                        bool temp_is_terminate = false;

                                        // Check jump target
                                        std::optional<std::uint32_t> branch_jump_target = calculate_direct_jump_target(
                                            pool_items_,
                                            tracing_instruction,
                                            switch_start_addr - INSTRUCTION_SIZE * (i + 1),
                                            memory_base_[(switch_start_addr >> 2) - (i + 1) + 1],
                                            temp_word_consumed,
                                            temp_is_terminate);

                                        if (branch_jump_target.has_value() &&
                                            (branch_jump_target.value() == switch_start_addr))
                                        {
                                            total_cases = tracing_instruction.two_sources_encoding.rs;
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        if (total_cases != std::numeric_limits<std::size_t>::max())
                        {
                            jump_table.labels_.resize(total_cases);

                            std::uint32_t table_pool_index = memory_base_[(addr >> 2) - 1];
                            std::uint32_t table_pool_addr = pool_items_.get_pool_item_constant(table_pool_index);

                            jump_table.jump_table_base_addr_ = table_pool_addr;

                            for (std::size_t i = 0; i < total_cases; i++)
                            {
                                std::uint32_t case_addr = memory_base_[(table_pool_addr >> 2) + i];
                                jump_table.labels_[i] = case_addr;
                                suspecting_to_be_labels.insert(case_addr);
                            }

                            // Can detect jump table and probably inline all the case blocks
                            result_function.jump_tables_.push_back(jump_table);
                            block_ending_sign_appear = false;
                        }
                    }
                }

                if (block_ending_sign_appear)
                {
                    mark_block_finished();
                }
            }
            else if (does_non_branch_instruction_consume_dword(instruction))
            {
                addr += sizeof(std::uint32_t);
            }

            addr += INSTRUCTION_SIZE;
        }

        if (!suspecting_to_be_labels.empty())
        {
            throw std::runtime_error("Some labels are not resolved!");
        }

        if (!unfinished_blocks_left.empty())
        {
            throw std::runtime_error("Some blocks are not finished!");
        }

        return result_function;
    }

    std::vector<Function> ProgramAnalysis::analyze(const std::uint32_t entry_point_addr)
    {
        std::vector<Function> results;
        found_functions_.clear();

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

            Function sweeped = sweep_function(addr);
            sweeped.is_entry_point_ = (addr == entry_point_addr + text_base_);

            results.push_back(sweeped);
        }

        return results;
    }
}
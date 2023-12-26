#include "ProgramAnalysis.h"
#include "Instruction.h"

/**
 * The analysis process makes some assumptions about the compiled assembly, primarly:
 *
 * 1. Link jump using CALLl is a linked function call, and the function is expected to return, resuming the control flow.
 * 2. Indirect branch is not obsfucated, meaning that the branching address is not computed at runtime, rather it's stored somewhere.
 *
 * Short jump using BL calls will be put into an investigation queue to see if it belongs to the current compiling code block or not. If the function
 * ends prematurely (by popping the stack and returning to RA), short jump will be deferred to a analyse queue.
 *
 * The analyser will also try its best to find out where it will jump to (this is not always possible, especially with vtable). If the analyser can't find it,
 * then we will leave it to the runtime to recompile the indirect jump code block. But if we managed to detect it, the same treatment as short jump will be applied.
 *
 * So, overall, the analyser should work OK, if the assembly is:
 *
 * 1. Not obsfucated
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
    static bool is_non_conditional_direct_branch(const Instruction &instruction)
    {
        switch (instruction.word_encoding.opcode)
        {
        case Opcode::JPl:
        case Opcode::CALLl:
            return true;
        default:
            return false;
        }
    }

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

    std::vector<CodeBlock> ProgramAnalysis::analyze(const std::uint32_t entry_point_addr)
    {
        std::vector<CodeBlock> results;

        while (!analyse_queue_.empty())
            analyse_queue_.pop();

        analyse_queue_.push(entry_point_addr);

        while (!analyse_queue_.empty())
        {
            const auto addr = text_base_ + analyse_queue_.front();
            analyse_queue_.pop();

            if (addr < text_base_ || addr >= text_base_ + text_size_)
            {
                continue;
            }

            bool is_block_end_reached = false;

            while (!is_block_end_reached)
            {
                const auto opcode = memory_base_[addr];
                const Instruction instruction{opcode};

                // Direct branch, we can follow it!
                if (is_direct_branch(instruction))
                {
                    if (!is_non_conditional_direct_branch(instruction))
                    {
                        // The execution may continue
                    }
                }
                else if (is_indirect_branch(instruction))
                {
                    // Do more analysis to see if we can follow it
                    // Don't want to try to cut off the analysis too early
                    // It could be a jump table
                    is_block_end_reached = true;
                }
                else
                {
                    // Keep going with the block
                }
            }
        }

        return results;
    }
}
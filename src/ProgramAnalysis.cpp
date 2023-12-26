#include "ProgramAnalysis.h"
#include "Instruction.h"

namespace Pip2 {
    static bool is_direct_branch(const Instruction& instruction) {
        switch (instruction.word_encoding.opcode) {
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

    static bool is_indirect_branch(const Instruction& instruction) {
        switch (instruction.word_encoding.opcode) {
            case Opcode::JPr:
            case Opcode::CALLr: // Like ARM BL
            case Opcode::RET:
                return true;
            default:
                return false;
        }
    }

    std::vector<CodeBlock> ProgramAnalysis::analyze(const std::uint32_t entry_point_addr) {
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

            while (!is_block_end_reached) {
                const auto opcode = memory_base_[addr];
                const Instruction instruction { opcode };

                // Direct branch, we can follow it!
                if (is_direct_branch(instruction)) {

                } else if (is_indirect_branch(instruction)) {
                    // Do more analysis to see if we can follow it
                    // Don't want to try to cut off the analysis too early
                    // It could be a jump table
                    is_block_end_reached = true;
                }
                else {
                    // Keep going with the block
                }
            }
        }

        return results;
    }
}
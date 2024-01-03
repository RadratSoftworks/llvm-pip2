#pragma once

#include <cstdint>

#include "Opcode.h"
#include "Register.h"

namespace Pip2
{
    union Instruction
    {
        std::uint32_t value;

        struct
        {
            Opcode opcode;
            Register rd;
            std::uint16_t imm;
        } word_encoding;

        struct
        {
            Opcode opcode;
            Register rs;
            std::uint8_t count;
        } range_reg_encoding;

        struct
        {
            Opcode opcode;
            Register rd;
            Register rs;
            Register rt;
        } two_sources_encoding;

        struct
        {
            Opcode opcode;
            Register rd;
        } dest_only_encoding;
    };

    inline Instruction make_word_instruction(Opcode opcode, Register rd, std::uint16_t imm)
    {
        return Instruction {
            .word_encoding = {
                .opcode = opcode,
                .rd = rd,
                .imm = imm
            }
        };
    }

    inline Instruction make_range_reg_instruction(Opcode opcode, Register rs, std::uint8_t count)
    {
        return Instruction {
            .range_reg_encoding = {
                .opcode = opcode,
                .rs = rs,
                .count = count
            }
        };
    }

    inline Instruction make_binary_instruction(Opcode opcode, Register rd, Register rs, Register rt)
    {
        return Instruction {
            .two_sources_encoding = {
                .opcode = opcode,
                .rd = rd,
                .rs = rs,
                .rt = rt
            }
        };
    }

    inline Instruction make_unary_instruction(Opcode opcode, Register rd, Register rs)
    {
        return Instruction {
                .two_sources_encoding = {
                        .opcode = opcode,
                        .rd = rd,
                        .rs = rs
                }
        };
    }

    inline Instruction make_single_argument_instruction(Opcode opcode, Register rd)
    {
        return Instruction {
            .dest_only_encoding = {
                .opcode = opcode,
                .rd = rd
            }
        };
    }

    inline Instruction make_pool_ref(std::uint32_t value)
    {
        return Instruction {
            .value = value
        };
    }

    inline Instruction make_constant(std::uint32_t value)
    {
        return Instruction {
                .value = value | 0x80000000
        };
    }
}

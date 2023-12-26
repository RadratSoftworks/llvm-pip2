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
}

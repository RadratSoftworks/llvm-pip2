#pragma once

#include "Register.h"

#include <cstdint>
#include <vector>
#include <set>

namespace Pip2 {
    struct JumpTable
    {
        std::uint32_t jump_instruction_addr_;
        std::uint32_t jump_table_base_addr_;
        std::uint32_t switch_value_resolved_addr_;
        Register switch_value_register_;
        std::vector<std::size_t> labels_;
    };

    struct Function
    {
        std::uint32_t addr_;
        std::size_t length_;

        // Label separate function into code blocks
        std::set<std::uint32_t> labels_;
        std::vector<JumpTable> jump_tables_;

        bool is_entry_point_;
    };
}
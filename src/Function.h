#include <cstdint>
#include <vector>
#include <set>

namespace Pip2 {
    struct JumpTable
    {
        std::uint32_t jump_instruction_addr_;
        std::vector<std::size_t> labels_;
    };

    struct Function
    {
        std::uint32_t addr_;
        std::size_t length_;

        // Label separate function into code blocks
        std::set<std::uint32_t> labels_;
        std::vector<JumpTable> jump_tables_;
    };
}
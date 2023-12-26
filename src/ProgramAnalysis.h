#pragma once

#include "PoolItems.h"

#include <cstdint>
#include <vector>
#include <queue>

namespace Pip2 {
    enum CodeBlockFlags {
        /// Code block that returned to the caller.
        CodeBlockFlag_Routine = 1
    };

    struct CodeBlock {
        std::uint32_t addr_;
        std::size_t length_;
        CodeBlockFlags flags_;
    };

    class ProgramAnalysis {
    private:
        const std::uint32_t *memory_base_;
        std::size_t text_base_;
        std::size_t text_size_;

        std::queue<std::uint32_t> analyse_queue_;
        const PoolItems &pool_items_;

    public:
        explicit ProgramAnalysis(const std::uint32_t *memory_base, std::size_t text_base, std::size_t text_size,
                                 const PoolItems& pool_items)
            : memory_base_(memory_base), text_base_(text_base), text_size_(text_size), pool_items_(pool_items) {
        }

        std::vector<CodeBlock> analyze(std::uint32_t entry_point_addr);
    };
}
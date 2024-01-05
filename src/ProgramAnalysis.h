#pragma once

#include "PoolItems.h"
#include "Function.h"

#include <cstdint>
#include <vector>
#include <queue>
#include <map>
#include <set>

namespace Pip2
{
    class ProgramAnalysis
    {
    private:
        const std::uint32_t *memory_base_;
        std::size_t text_base_;
        std::size_t text_size_;

        std::queue<std::uint32_t> analyse_queue_;
        std::set<std::uint32_t> found_functions_;
        std::set<std::uint32_t> found_table_labels_;

        const PoolItems &pool_items_;

        Function sweep_function(std::uint32_t addr);
        void add_to_function_analyse_queue(std::uint32_t addr);

    public:
        explicit ProgramAnalysis(const std::uint32_t *memory_base, std::size_t text_base, std::size_t text_size,
                                 const PoolItems &pool_items)
            : memory_base_(memory_base), text_base_(text_base), text_size_(text_size), pool_items_(pool_items)
        {
        }

        std::vector<Function> analyze(std::uint32_t entry_point_addr);
    };
}
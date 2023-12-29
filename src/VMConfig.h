#pragma once

#include <cstdint>
#include "PoolItems.h"

namespace Pip2 {
    struct VMConfig {
    private:
        std::uint8_t *memory_base_;
        std::size_t memory_size_;

        PoolItems pool_items_;

    public:
        explicit VMConfig(std::uint8_t *memory_base, std::size_t memory_size, const std::uint32_t *pool_items_base, std::size_t pool_item_count)
            : memory_base_(memory_base), memory_size_(memory_size), pool_items_(pool_items_base, pool_item_count) {
        }

        [[nodiscard]] std::uint8_t *memory_base() const { return memory_base_; }
        [[nodiscard]] std::size_t memory_size() const { return memory_size_; }
        [[nodiscard]] const PoolItems &pool_items() const { return pool_items_; }
    };
}
#pragma once

#include <cstdint>

namespace Pip2 {
    struct VMConfigParameters {
        std::uint8_t *memory_base_;
        std::uint64_t memory_size_;
        std::uint64_t *pool_items_base_;
        std::uint64_t pool_item_count_;
    };
}
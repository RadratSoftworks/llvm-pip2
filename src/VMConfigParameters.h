#pragma once

#include "Common.h"
#include <cstdint>

namespace Pip2 {
    struct VMConfigParameters {
        std::uint8_t *memory_base_;
        std::uint64_t memory_size_;
        std::uint64_t *pool_items_base_;
        std::uint64_t pool_item_count_;
        Common::TaskStackCreateFunc stack_create_func_;
        Common::TaskStackFreeFunc stack_free_func_;
    };
}
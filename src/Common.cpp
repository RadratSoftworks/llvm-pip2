#include "Common.h"

namespace Pip2::Common {
    std::optional<std::uint32_t> read_immediate(const PoolItems &pool_items, std::uint32_t dword) {
        if (std::optional<std::uint32_t> immediate = get_immediate_pip_dword(dword)) {
            return immediate.value();
        }

        if (pool_items.is_pool_item_constant(dword)) {
            return pool_items.get_pool_item_constant(dword);
        }

        return std::nullopt;
    }
}
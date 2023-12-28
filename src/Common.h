#pragma once

#include <cstdint>
#include <optional>

#include "PoolItems.h"

namespace Pip2::Common {
    inline std::optional<std::uint32_t> get_immediate_pip_dword(const std::uint32_t dword) {
        if ((dword & 0x80000000U) != 0) {
            return static_cast<std::uint32_t>((dword & 0x7FFFFFFF) | ((dword << 1) & 0x80000000));
        }

        return std::nullopt;
    }
}
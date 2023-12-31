#pragma once

#include <cstdint>
#include <optional>

#include "PoolItems.h"

namespace Pip2::Common
{
    inline std::optional<std::uint32_t> get_immediate_pip_dword(const std::uint32_t dword)
    {
        if ((dword & 0x80000000U) != 0)
        {
            return static_cast<std::uint32_t>((dword & 0x7FFFFFFF) | ((dword << 1) & 0x80000000));
        }

        return std::nullopt;
    }

    inline std::uint32_t sign_extend(std::uint8_t value)
    {
        return static_cast<std::uint32_t>(static_cast<std::int8_t>(value));
    }

    inline std::uint32_t sign_extend(std::uint16_t value)
    {
        return static_cast<std::uint32_t>(static_cast<std::int16_t>(value));
    }

    inline std::uint16_t sign_extend_to_word(std::uint8_t value)
    {
        return static_cast<std::uint16_t>(static_cast<std::int8_t>(value));
    }
}
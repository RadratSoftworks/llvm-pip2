#pragma once

#include <cstdint>
#include <cstring>

#include "Register.h"

namespace Pip2 {
    struct VMContext {
        std::uint32_t regs_[Register::TotalCount];

        VMContext() {
            std::memset(regs_, 0, sizeof(regs_));
        }
    };

    static constexpr std::size_t CONTEXT_REG_OFFSET = offsetof(VMContext, regs_);
}
#pragma once

#include <cstdint>
#include "Register.h"

namespace Pip2 {
    struct VMContext {
        std::uint32_t regs_[Register::TotalCount];
    };

    static constexpr std::size_t CONTEXT_REG_OFFSET = offsetof(VMContext, regs_);
}
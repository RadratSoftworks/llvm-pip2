#pragma once

#include <cstdint>

namespace Pip2
{
    struct VMContext;

    typedef std::uint32_t (*TaskStackCreateFunc)(void*, std::int64_t);
    typedef void (*TaskStackFreeFunc)(void*, std::uint32_t);

    typedef void (*HleHandler)(void *userdata, int hleFunctionCode);
    typedef void (*RuntimeFunction)(VMContext &context, std::uint32_t *memory_base, void **runtime_function_lookup,
                                    HleHandler hle_handler, void *userdata);
}
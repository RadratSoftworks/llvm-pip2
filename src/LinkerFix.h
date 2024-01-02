#pragma once

#include <malloc.h>

#ifdef _alloca
#undef _alloca
extern "C" {
void* _alloca(std::size_t _Size) {
    return __builtin_alloca(_Size);
}
}
#endif

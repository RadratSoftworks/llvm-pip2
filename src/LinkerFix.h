#pragma once

#include <malloc.h>
#include <cstring>

#ifdef _alloca
#undef _alloca
#define ALLOCA_FIX

extern "C" {
void *_alloca(std::size_t _Size);
}
#endif

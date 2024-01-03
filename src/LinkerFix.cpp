#include "LinkerFix.h"
#include <cstring>

#ifdef ALLOCA_FIX
void *_alloca(std::size_t _Size) {
    return __builtin_alloca(_Size);
}
#endif
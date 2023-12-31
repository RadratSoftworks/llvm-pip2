#include "../Translator.h"

namespace Pip2 {
    void Translator::SYSCPY(Instruction instruction)
    {
        auto src = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        auto dst = get_register<std::uint32_t>(instruction.two_sources_encoding.rd);
        auto size = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        auto src_ptr = get_memory_pointer(src);
        auto dst_ptr = get_memory_pointer(dst);

        builder_.CreateMemCpy(dst_ptr, llvm::MaybeAlign(1), src_ptr, llvm::MaybeAlign(1), size);
    }

    void Translator::SYSSET(Instruction instruction)
    {
        auto dst = get_register<std::uint32_t>(instruction.two_sources_encoding.rd);
        auto value = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        auto size = get_register<std::uint32_t>(instruction.two_sources_encoding.rt);

        auto dst_ptr = get_memory_pointer(dst);

        builder_.CreateMemSet(dst_ptr, value, size, llvm::MaybeAlign(1));
    }
}
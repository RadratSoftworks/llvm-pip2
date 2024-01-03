#include "../Translator.h"

namespace Pip2
{
    void Translator::NOT(Instruction instruction)
    {
        auto value = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        set_register(instruction.two_sources_encoding.rd, builder_.CreateNot(value));
    }

    void Translator::NEG(Instruction instruction)
    {
        auto value = get_register<std::uint32_t>(instruction.two_sources_encoding.rs);
        set_register(instruction.two_sources_encoding.rd, builder_.CreateNeg(value));
    }

    void Translator::EXSB(Instruction instruction)
    {
        auto value = get_register<std::uint8_t>(instruction.two_sources_encoding.rs);
        set_register(instruction.two_sources_encoding.rd, builder_.CreateSExt(value, i32_type_));
    }

    void Translator::EXSH(Instruction instruction)
    {
        auto value = get_register<std::uint16_t>(instruction.two_sources_encoding.rs);
        set_register(instruction.two_sources_encoding.rd, builder_.CreateSExt(value, i32_type_));
    }

    void Translator::NOP(Instruction instruction)
    {
        // Do nothing
    }
}
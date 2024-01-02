#include "VMEngine.h"
#include "Instruction.h"
#include "Opcode.h"
#include "VMConfig.h"
#include "VMOptions.h"

using namespace Pip2;

#ifdef _alloca
#undef _alloca
extern "C" {
void* _alloca(std::size_t _Size) {
    return __builtin_alloca(_Size);
}
}
#endif

int main(int argc, char **argv) {
    Instruction instructions[] = {
        Instruction {
            .two_sources_encoding = {
                    .opcode = Opcode::ADD,
                    .rd = Register::P0,
                    .rs = Register::P1,
                    .rt = Register::P2
            }
        },

        Instruction {
            .two_sources_encoding = {
                    .opcode = Opcode::SUB,
                    .rd = Register::P3,
                    .rs = Register::P0,
                    .rt = Register::P2
            }
        },

        Instruction {
            .two_sources_encoding = {
                    .opcode = Opcode::ADDi,
                    .rd = Register::P3,
                    .rs = Register::P3,
            }
        },

        Instruction {
            .value = 0x80000512
        },

        Instruction {
            .two_sources_encoding = {
                    .opcode = Opcode::ADDBi,
                    .rd = Register::P3,
                    .rs = Register::P3,
                    .rt = (Register)130
            }
        },

        Instruction {
            .two_sources_encoding = {
                    .opcode = Opcode::JPr,
                    .rd = Register::RA
            }
        },
    };

    auto *memory_base = reinterpret_cast<std::uint32_t*>(instructions);
    std::size_t memory_size = sizeof(instructions);

    std::uint32_t pool_base = 0;

    VMConfig vm_config(reinterpret_cast<std::uint8_t*>(memory_base), memory_size, &pool_base, 0);
    VMOptions vm_options {
        .divide_by_zero_result_zero = true,
        .cache_ = false,
        .text_base_ = 0,
        .entry_point_ = 0
    };

    VMEngine vm_engine("TestJitBasic", vm_config, vm_options);
    vm_engine.context().regs_[Register::P1 >> 2] = 10;
    vm_engine.context().regs_[Register::P2 >> 2] = 5;
    vm_engine.execute();

    return 0;
}
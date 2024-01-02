#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include <Instruction.h>
#include <VMEngine.h>
#include <VMOptions.h>
#include <VMConfig.h>

#include "ModifiablePoolItems.h"

namespace Pip2::Test {
    class TestEnvironment {
    private:
        std::vector<std::uint8_t> memory_;
        std::uint32_t text_size_;
        std::uint32_t stack_size_;

        std::unique_ptr<VMConfig> vm_config_;
        std::unique_ptr<Pip2::VMEngine> engine_;

        ModifiablePoolItems pool_items_;

        std::vector<std::uint32_t> pool_items_built_;

        VMOptions vm_options_;

    public:
        explicit TestEnvironment(const std::string &case_name, std::vector<Pip2::Instruction> instructions,
                                 ModifiablePoolItems &&pool_items,
                                 std::uint32_t stack_size);

        ~TestEnvironment() = default;

        void reg(Pip2::Register reg, std::uint32_t value);
        std::uint32_t reg(Pip2::Register reg) const;

        std::uint8_t *stack();

        void run();
    };
}

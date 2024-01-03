#include "TestEnvironment.h"

namespace Pip2::Test {
    TestEnvironment::TestEnvironment(const std::string &case_name, std::vector<Pip2::Instruction> instructions,
                                     ModifiablePoolItems &&pool_items,
                                     std::uint32_t stack_size,
                                     std::uint32_t heap_size)
         : pool_items_(pool_items)
         , engine_(nullptr)
         , stack_size_(stack_size)
         , heap_size_(heap_size)
     {
        pool_items_built_ = pool_items.build();

        text_size_ = instructions.size() * sizeof(Pip2::Instruction);
        stack_size_ = stack_size;

        memory_.resize(text_size_ + stack_size_ + heap_size_);

        std::memset(memory_.data(), 0, memory_.size());
        std::memcpy(memory_.data(), instructions.data(), text_size_);

        vm_options_ = VMOptions {
             .divide_by_zero_result_zero = true,
             .cache_ = false,
             .text_base_ = 0,
             .entry_point_ = 0
        };

        vm_config_ = std::make_unique<VMConfig>(reinterpret_cast<std::uint8_t*>(memory_.data()),
                                                memory_.size(), &pool_items_built_[0], pool_items_built_.size());

        engine_ = std::make_unique<Pip2::VMEngine>(case_name, *vm_config_, vm_options_);
    }

    void TestEnvironment::reg(Pip2::Register reg, std::uint32_t value)
    {
        engine_->reg(reg, value);
    }

    std::uint32_t TestEnvironment::reg(Pip2::Register reg) const
    {
        return engine_->reg(reg);
    }

    std::uint8_t *TestEnvironment::stack()
    {
        return memory_.data() + text_size_;
    }

    std::uint8_t *TestEnvironment::heap() {
        return memory_.data() + text_size_ + stack_size_;
    }

    void TestEnvironment::run()
    {
        engine_->execute();
    }
}

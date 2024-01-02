#pragma once

#include <functional>
#include <vector>
#include <map>

namespace Pip2::Test {
    typedef void (*ModifiablePoolFunction)(void*);

    class ModifiablePoolItems {
    private:
        struct ModifiablePoolItem {
            ModifiablePoolFunction func_;
            void *func_userdata_;

            std::uint32_t value_;
        };

        std::vector<ModifiablePoolItem> pool_items_;

    public:
        ModifiablePoolItems() = default;
        ~ModifiablePoolItems() = default;

        std::uint32_t get(std::uint32_t value);
        std::uint32_t get(ModifiablePoolFunction func, void *func_data);

        std::vector<std::uint32_t> build();
    };
}
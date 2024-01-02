#include "ModifiablePoolItems.h"

namespace Pip2::Test {
    std::uint32_t ModifiablePoolItems::get(const std::uint32_t value)
    {
        auto existing_item = std::find_if(pool_items_.begin(), pool_items_.end(), [value](const ModifiablePoolItem &item) {
            return item.value_ == value;
        });

        if (existing_item != pool_items_.end()) {
            return std::distance(pool_items_.begin(), existing_item) + 1;
        }

        ModifiablePoolItem item{};
        item.value_ = value;

        pool_items_.push_back(item);

        return pool_items_.size();
    }

    std::uint32_t ModifiablePoolItems::get(ModifiablePoolFunction func, void *func_data)
    {
        auto existing_item = std::find_if(pool_items_.begin(), pool_items_.end(), [func, func_data](const ModifiablePoolItem &item) {
            return item.func_ == func && item.func_userdata_ == func_data;
        });

        if (existing_item != pool_items_.end()) {
            return std::distance(pool_items_.begin(), existing_item) + 1;
        }

        ModifiablePoolItem item{};
        item.func_ = func;
        item.func_userdata_ = func_data;

        pool_items_.push_back(item);

        return pool_items_.size();
    }

    std::vector<std::uint32_t> ModifiablePoolItems::build()
    {
        std::vector<std::uint32_t> pool_items;

        for (const auto &item : pool_items_) {
            if (item.func_ != nullptr) {
                pool_items.push_back(0x80000000);
            } else {
                pool_items.push_back(item.value_);
            }
        }

        return pool_items;
    }
}

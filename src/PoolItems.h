#pragma once

#include <cstdint>
#include <cstring>

namespace Pip2
{
   class PoolItems
   {
   private:
      const std::uint32_t *pool_items_;
      const std::size_t pool_item_count_;

   public:
      explicit PoolItems(const std::uint32_t *pool_items, std::size_t pool_item_count)
         : pool_items_(pool_items), pool_item_count_(pool_item_count)
      {
      }

      std::size_t pool_item_count() const { return pool_item_count_; }
      bool is_pool_item_constant(const std::size_t number) const;
      bool is_pool_item_terminate_function(const std::size_t number) const;
      std::uint32_t get_pool_item_constant(const std::size_t number) const;
   };
}

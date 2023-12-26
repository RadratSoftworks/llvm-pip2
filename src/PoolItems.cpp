#include "PoolItems.h"

namespace Pip2
{
   bool PoolItems::is_pool_item_constant(const std::size_t index) const
   {
      if (index > pool_item_count_ || index == 0)
      {
         return false;
      }

      return (pool_items_[index - 1] & 0x80000000) == 0;
   }


   std::uint32_t PoolItems::get_pool_item_constant(const std::size_t index) const
   {
      if (!is_pool_item_constant(index))
      {
         return 0;
      }

      return pool_items_[index - 1];
   }
}

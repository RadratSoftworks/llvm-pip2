#include "PoolItems.h"

namespace Pip2
{
   bool PoolItems::is_pool_item_constant(const std::size_t index) const
   {
      if (index > pool_item_count_ || index == 0)
      {
         return false;
      }

      return (pool_items_[index - 1] & 0x80000000'00000000) == 0;
   }

   bool PoolItems::is_pool_item_in_text(const std::size_t index) const
   {
       if (index > pool_item_count_ || index == 0)
       {
           return false;
       }

       return (pool_items_[index - 1] & 0x40000000'00000000) == 0x40000000'00000000;
   }

   bool PoolItems::is_pool_item_function_table_list(const std::size_t index) const {
         if (index > pool_item_count_ || index == 0)
         {
              return false;
         }

         return (pool_items_[index - 1] & 0x20000000'00000000) == 0x20000000'00000000;
   }

   bool PoolItems::is_pool_item_function_in_table(const std::size_t index) const {
         if (index > pool_item_count_ || index == 0)
         {
              return false;
         }

         return (pool_items_[index - 1] & 0x10000000'00000000) == 0x10000000'00000000;
   }

   bool PoolItems::is_pool_item_terminate_function(const std::size_t index) const
   {
       if (index > pool_item_count_ || index == 0)
       {
           return false;
       }

       return (pool_items_[index - 1] & 0x80000001'00000000) == 0x80000001'00000000;
   }

   std::uint32_t PoolItems::get_pool_item_constant(const std::size_t index) const
   {
      if (!is_pool_item_constant(index))
      {
         return 0;
      }

      return static_cast<std::uint32_t>(pool_items_[index - 1] & ~(0x10000000'00000000 | 0x20000000'00000000 | 0x40000000'00000000));
   }
}

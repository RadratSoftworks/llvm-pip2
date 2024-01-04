#pragma once

namespace Pip2
{
    struct VMOptions
    {
        /**
         * @brief When this is set to true, divide by zero will result in zero.
         */
        bool divide_by_zero_result_zero;

        /**
         * @brief When this is set to true, the VM will cache the JIT result and reuse it next time.
         */
        bool cache_;

        /**
         * @brief The path to the cache directory. Used when cache is enabled.
         */
        const char *cache_root_path_;

        /**
         * @brief The base address of the text segment.
         */
        std::uint32_t text_base_;

        /**
         * @brief The entry point of the program.
         */
        std::uint32_t entry_point_;
    };
}
#pragma once

namespace Pip2
{
    struct VMOptions
    {
        /**
         * @brief When this is set to true, divide by zero will result in zero.
         */
        bool divide_by_zero_result_zero;
    };
}
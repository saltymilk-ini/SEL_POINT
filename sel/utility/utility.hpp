#pragma once

#include "../global/global_def.hpp"

#include <type_traits>
#include <cmath>

SEL_NAMESPACE_BEGIN

template<typename T, typename... Rest>
inline constexpr auto max(T first, Rest... rest) noexcept/*-> std::common_type_t<T, Rest...>*/ 
{
    if constexpr (sizeof...(Rest) == 0)
        return first;
    else
    {
        constexpr auto max_of_rest = max(rest...);

        if constexpr (max_of_rest > first)
            return max_of_rest;

        return first;
    }
}

template<typename T, typename... Rest>
inline constexpr auto min(T first, Rest... rest) noexcept/* -> std::common_type_t<T, Rest...>*/
{
    if constexpr (sizeof...(Rest) == 0)
        return first;
    else
    {
        auto min_of_rest = min(rest...);

        return first < min_of_rest ? first : min_of_rest;
    }
}

template<typename... Args>
inline constexpr auto sum(Args... args) noexcept -> std::common_type_t<Args...>
{
    return (args + ...);
}

/**
 * @brief The function to check if a value is NaN regardless of whether
 * it's an integral or a floating-point number.
 */
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
inline constexpr bool is_nan(T val) noexcept
{
    if constexpr (std::is_integral_v<T>)
        return false;
    else
        return std::isnan(val);
}

SEL_NAMESPACE_END

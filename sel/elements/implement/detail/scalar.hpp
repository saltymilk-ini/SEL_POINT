/*
MIT License
Copyright (c) 2026 saltymilk-ini
The full license is in LICENSE file included with this distribution at https://github.com/saltymilk-ini/SEL_POINT
*/

#include "../../../global/global_def.hpp"

#include <type_traits>
#include <limits>

SEL_NAMESPACE_BEGIN

SEL_DETAIL_BEGIN

/** @brief Alias template only valid with arithmetic types.*/
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>>
using scalar = T;

template<typename T>
inline constexpr scalar<T> scalar_default = scalar<T>{};

template<typename T>
inline constexpr scalar<T> scalar_max = std::numeric_limits<scalar<T>>::max();

template<typename T>
inline constexpr scalar<T> scalar_min = std::numeric_limits<scalar<T>>::min();

template<typename T>
inline constexpr scalar<T> scalar_lowest = std::numeric_limits<scalar<T>>::lowest();

template<typename To, typename From>
inline constexpr scalar<To> convert(scalar<From> from)
{
    return static_cast<To>(from);
}

SEL_DETAIL_END

SEL_NAMESPACE_END
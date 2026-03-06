#include "../../../global/global_def.hpp"

#include <type_traits>
#include <limits>

SEL_NAMESPACE_BEGIN

SEL_DETAIL_BEGIN

/**
* @file This file defines arithmetic safe conversion check methods.
* @brief When conversions are taken place between two elements of different arithmetic types, the default
* behaviour is to prevent unsafe implicit arithmetic conversions from happening. That is to say, if the 
* underlying arithmetic conversion may cause numeric truncation, rounding, or precision loss(as previously
* mentioned, unsafe), it is not allowed to implicitly take place by default. You must use cast(member function
* of all element classes) to perform an explicit conversion.<br>
* If the macro SEL_NO_SAFE_CONVERSION_CHECK is defined, the conversion safety check between elements is disabled.
* @note Arithmetic operations between element and scalar(built-in arithmetic type) is safety check free, but the
* compiler may emit warnings.
*/

template<typename From, typename To>
class is_safe_conversion
{
private:
    static constexpr bool same_type = std::is_same_v<From, To>;

    static constexpr bool same_signedness =
        std::is_signed_v<From> == std::is_signed_v<To>;

    static constexpr bool both_integral =
        std::is_integral_v<From> && std::is_integral_v<To>;

    static constexpr bool both_floating_point =
        std::is_floating_point_v<From> && std::is_floating_point_v<To>;

    static constexpr bool integral_to_floating_point =
        std::is_integral_v<From> && std::is_floating_point_v<To>;

    static constexpr bool floating_point_to_integral =
        std::is_floating_point_v<From> && std::is_integral_v<To>;

    static constexpr bool safe_integral_conversion()
    {
        if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>)
            return false;
        else if constexpr (std::is_unsigned_v<From> && std::is_signed_v<To>)
            return std::numeric_limits<To>::max() >= std::numeric_limits<From>::max();
        else if constexpr (same_signedness)
        {
            return sizeof(To) >= sizeof(From) ||
                (std::numeric_limits<To>::max() >= std::numeric_limits<From>::max() &&
                std::numeric_limits<To>::lowest() <= std::numeric_limits<From>::lowest());
        }

        return false;
    }

    static constexpr bool safe_floating_point_conversion=
        std::numeric_limits<To>::digits >= std::numeric_limits<From>::digits &&
        std::numeric_limits<To>::max_exponent >= std::numeric_limits<From>::max_exponent &&
        std::numeric_limits<To>::min_exponent <= std::numeric_limits<From>::min_exponent;

    static constexpr bool safe_integral_to_floating_point =
        std::numeric_limits<To>::digits >= std::numeric_limits<From>::digits;

    static constexpr bool safe_floating_point_to_integral = false;

    static constexpr bool check()
    {
        if constexpr (same_type)
            return true;
        else if constexpr (both_integral)
            return safe_integral_conversion();
        else if constexpr (both_floating_point)
            return safe_floating_point_conversion;
        else if constexpr (integral_to_floating_point)
            return safe_integral_to_floating_point;
        else if constexpr (floating_point_to_integral)
            return safe_floating_point_to_integral;

        return false;
    }

public:
    static constexpr bool value = check();
};

template<typename From, typename To>
constexpr bool is_safe_conversion_v = is_safe_conversion<From, To>::value;

template<typename From, typename To>
inline void safe_element_conversion_assert()
{
#ifndef SEL_NO_SAFE_CONVERSION_CHECK
    static_assert(is_safe_conversion_v<typename From::value_type, typename To::value_type>,
        "implicit type conversion is unsafe, use member function 'cast' to make an explicit conversion.");
#endif
}

template<typename From, typename To>
inline void safe_numeric_conversion_assert()
{
#ifndef SEL_NO_SAFE_CONVERSION_CHECK
    static_assert(is_safe_conversion_v<From, To>,
        "implicit type conversion is unsafe, use 'static_cast' to make an explicit conversion.");
#endif
}

SEL_DETAIL_END

SEL_NAMESPACE_END
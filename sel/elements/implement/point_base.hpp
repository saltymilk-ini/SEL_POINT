#pragma once

#include "./detail/point_swizzler.hpp"
#include "detail/arithmetic_progression.hpp"
#include "detail/vector_sequence_operations.hpp"
#include "../../utility/utility.hpp"

#include <cstring>
#include <stdexcept>

SEL_NAMESPACE_BEGIN

/** 
 * @class point_
 * @brief Base class of all kind of points, implements all basic operations.
 */
template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
class point_
{
    static_assert(N > 1, "a point must have at least 2 dimensions.");
    using component_position_list = detail::arithmetic_progression<0, 1, N>;

public:
    enum : std::size_t { dimension = N };
    using value_type = detail::scalar<T>;
    using derived_class = Derived<value_type, N>;
    using length_type = std::conditional_t<std::is_integral_v<value_type>, double, value_type>;

public:
    /** 
     * @brief Explicitly cast to point of other value type.
     * @attention This method will not emit warnings about numeric truncation, rounding, or precision loss.
    */
    template<typename U>
    Derived<U, N> cast() const;

    /**
     * @brief Convert to point of other value type.
     * @attention If the conversion from T to U is unsafe, this conversion is forbidden,
     * use @ref cast to make an explicit conversion.
     */
    template<typename U>
    operator Derived<U, N>() const;

    /** @brief Test if all components not NAN.*/
    bool valid() const;

    /** @brief operator[] to access single component, <b>no boundary check</b>.*/
    value_type &operator[](std::size_t pos);
    const value_type &operator[](std::size_t pos) const;

    /** @brief Another way to access single component, <b>with boundary check</b>.*/
    value_type &at(std::size_t pos);
    const value_type &at(std::size_t pos) const;

    length_type squared_length() const;
    length_type length() const;

    /**
     * @brief Normalize the point so that it has unitized length.
     * @return The length before normalized.
     */
    value_type normalize();

    /** @brief Get a normalized copy of this point.*/
    derived_class normalized() const;

    /** @brief Set all components to the given value.*/
    template<typename U>
    void set_all(detail::scalar<U> val);

    /** @brief Test if all components of this point are greater than or equal to that of rhs's.*/
    bool exceeds(const derived_class &rhs) const;

    /** @brief Binary add operator.*/
    const derived_class operator+(const derived_class &rhs) const;
    /** @brief Binary subtract operator.*/
    const derived_class operator-(const derived_class &rhs) const;
    /** @brief Add assign operator.*/
    derived_class &operator+=(const derived_class &rhs);
    /** @brief Subtract assign operator.*/
    derived_class &operator-=(const derived_class &rhs);

    /** @brief Binary multiply operator.*/
    template<typename U>
    const derived_class operator*(detail::scalar<U> s) const;
    /** @brief Binary divide operator.*/
    template<typename U>
    const derived_class operator/(detail::scalar<U> s) const;
    /** @brief Multiply assign operator.*/
    template<typename U>
    derived_class &operator*=(detail::scalar<U> s);
    /** @brief Divide assign operator.*/
    template<typename U>
    derived_class &operator/=(detail::scalar<U> s);

    /** @brief Unary minus operator.*/
    const derived_class operator-() const;

    /**
     * @brief Interface to mimic GLSL swizzling.
     * @note From a purely functional perspective, swizzle does not require a function parameter list,
     * only with the template parameter list can the component positions be extracted.<br>
     * However, without a function parameter list, some code completion program (such as Clang's)
     * will insert the template parameter list placeholder, which is undesirable in this specific context.
     * @par You can write code like this:
     * @code
     * sel::point4i p4i(1, 2, 3, 4);
     * sel::point4f p4f = p4i.swizzle(y, x, w, z);//use xyzw. p4f is (2.0, 1.0, 4.0, 3.0)
     * p4i.swizzle(r, g, b) = sel::point3f(2.0);//use rgba. now p4i becomes (2, 2, 2, 4)
     * sel::point4d p4d = p4i.swizzle(0, 0, 1, 2);//you can use index(0-255) too
     * p4i = p4i.swizzle(w, z, y, x);//it can be used to reorder the components of a point
     * p4i.swizzle(0, 1) = p4i.swizzle(2, 3);//or copy backward/forward in the same point
     * p4i.swizzle(0, 1) = p4d.swizzle(0, 1);//copying between 2 different points is also supported
     * p4i.swizzle(0, y, 2, a);//define POINT_SWIZZLE_USE_MIXED_INDICATOR to enable mixed indicators
     * @endcode
     */
    template<typename... Args>
    detail::swizzle_exchanger_type<T, Derived, Args...> swizzle(Args &&...) &;

    template<typename... Args>
    detail::const_swizzle_exchanger_type<T, Derived, Args...> swizzle(Args &&...) const &;

    /** @brief Swizzle operator for rvalue references.
     * @note Unlike lvalue points, rvalue points can only be swizzled to create a new point, rather
     * than a reference to the point from which it was created.
     */
    template<typename... Args>
    Derived<T, sizeof...(Args)> swizzle(Args &&... args) &&;

    template<typename... Args>
    Derived<T, sizeof...(Args)> swizzle(Args &&... args) const &&;
};

#define DR(a) static_cast<derived_class &>((a))
#define CDR(a) static_cast<const derived_class &>((a))

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename U>
inline Derived<U, N> point_<T, N, Derived>::cast() const
{
    Derived<U, N> p;

    detail::vector_vector_opt(p._v, component_position_list{}, CDR(*this)._v,
        [](auto, auto rhs) { return detail::convert<U>(rhs); });

    return p;
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename U>
inline point_<T, N, Derived>::operator Derived<U, N>() const
{
    detail::safe_element_conversion_assert<Derived<T, N>, Derived<U, N>>();

    return this->cast<U>();
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline bool point_<T, N, Derived>::valid() const
{
    const T(&v)[N] = CDR(*this)._v;

    return detail::vector_logic_and(v, component_position_list{},
        [](auto a) { return sel::is_nan(a) == false; });
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline typename point_<T, N, Derived>::value_type &
point_<T, N, Derived>::operator[](std::size_t pos)
{
    return DR(*this)._v[pos];
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline const typename point_<T, N, Derived>::value_type &
point_<T, N, Derived>::operator[](std::size_t pos) const
{
    return CDR(*this)._v[pos];
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline typename point_<T, N, Derived>::value_type &
point_<T, N, Derived>::at(std::size_t pos)
{
    if (pos >= N)
        throw std::out_of_range("illegal access of point component.");

    return DR(*this)._v[pos];
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline const typename point_<T, N, Derived>::value_type &
point_<T, N, Derived>::at(std::size_t pos) const
{
    if (pos >= N)
        throw std::out_of_range("illegal access of point component.");

    return CDR(*this)._v[pos];
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline typename point_<T, N, Derived>::length_type
point_<T, N, Derived>::squared_length() const
{
    const T(&v)[N] = CDR(*this)._v;

    length_type squared_length{};

    for (std::size_t i = 0; i != N; ++i)
        squared_length += v[i] * v[i];

    return squared_length;
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline typename point_<T, N, Derived>::length_type
point_<T, N, Derived>::length() const
{
    return sqrt(squared_length());
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline typename point_<T, N, Derived>::value_type point_<T, N, Derived>::normalize()
{
    static_assert(std::is_floating_point_v<value_type>,
                  "normalizing a point of integral value type is illegal");

    T len = length();

    if (len > detail::convert<T>(0.0))
    {
        value_type inv = detail::convert<T>(1.0) / len;

        detail::vector_scalar_opt(DR(*this)._v, component_position_list{}, inv,
            [](auto lhs, auto rhs) { return lhs * detail::convert<T>(rhs); });
    }

    return len;
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline typename point_<T, N, Derived>::derived_class
point_<T, N, Derived>::normalized() const
{
    point_ p(*this);

    p.normalize();

    return p;
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename U>
inline void point_<T, N, Derived>::set_all(U val)
{
    detail::safe_numeric_conversion_assert<U, T>();

    T(&v)[N] = DR(*this)._v;

    T value = detail::convert<T>(val);

    if (value == detail::scalar_default<T>)
        memset(v, 0, sizeof(v));
    else
        detail::vector_scalar_opt(v, component_position_list{}, value,
            [](auto, auto rhs) { return rhs; });
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline bool point_<T, N, Derived>::exceeds(const derived_class &rhs) const
{
    if (this == &rhs)
        return true;

    const T(&lv)[N] = CDR(*this)._v;
    const T(&rv)[N] = CDR(rhs)._v;

    return detail::vector_cmpare_and(lv, component_position_list{}, rv,
        [](auto lhs, auto rhs) { return lhs >= rhs; });
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline const typename point_<T, N, Derived>::derived_class
point_<T, N, Derived>::operator+(const derived_class &rhs) const
{
    derived_class p(CDR(*this));

    detail::vector_combine_opt(p._v, component_position_list{}, CDR(*this)._v, rhs._v,
        [](auto lhs, auto rhs) { return lhs + rhs; });

    return p;
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline const typename point_<T, N, Derived>::derived_class
point_<T, N, Derived>::operator-(const derived_class &rhs) const
{
    derived_class p(CDR(*this));

    detail::vector_combine_opt(p._v, component_position_list{}, CDR(*this)._v, rhs._v,
        [](auto lhs, auto rhs) { return lhs - rhs; });

    return p;
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline typename point_<T, N, Derived>::derived_class &
point_<T, N, Derived>::operator+=(const derived_class &rhs)
{
    detail::vector_vector_opt(DR(*this)._v, component_position_list{}, rhs._v,
        [](auto lhs, auto rhs) { return lhs + rhs; });

    return DR(*this);
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline typename point_<T, N, Derived>::derived_class &
point_<T, N, Derived>::operator-=(const derived_class &rhs)
{
    detail::vector_vector_opt(DR(*this)._v, component_position_list{}, rhs._v,
        [](auto lhs, auto rhs) { return lhs - rhs; });

    return DR(*this);
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename U>
inline const typename point_<T, N, Derived>::derived_class
point_<T, N, Derived>::operator*(detail::scalar<U> s) const
{
    detail::safe_numeric_conversion_assert<U, T>();
    
    derived_class p(CDR(*this));

    detail::vector_scalar_opt(p._v, component_position_list{}, detail::convert<T>(s),
        [](auto lhs, auto rhs) { return lhs * rhs; });

    return p;
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename U>
inline const typename point_<T, N, Derived>::derived_class
point_<T, N, Derived>::operator/(detail::scalar<U> s) const
{
    detail::safe_numeric_conversion_assert<U, T>();

    derived_class p(CDR(*this));

    if constexpr (std::is_integral_v<T>)
        detail::vector_scalar_opt(p._v, component_position_list{}, s,
            [](auto lhs, auto rhs) { return lhs / rhs; });
    else
    {
        value_type inv = detail::convert<T>(1.0) / detail::convert<T>(s);

        detail::vector_scalar_opt(p._v, component_position_list{}, inv,
            [](auto lhs, auto rhs) { return lhs * rhs; });
    }

    return p;
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename U>
inline typename point_<T, N, Derived>::derived_class &
point_<T, N, Derived>::operator*=(detail::scalar<U> s)
{
    return DR(*this) = this->operator*(s);
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename U>
inline typename point_<T, N, Derived>::derived_class &
point_<T, N, Derived>::operator/=(detail::scalar<U> s)
{
    return DR(*this) = this->operator/(s);
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
inline const typename point_<T, N, Derived>::derived_class
point_<T, N, Derived>::operator-() const
{
    static_assert(std::is_signed_v<T>, "unary minus operator applied to a point of unsigned value type");

    return this->operator*(detail::convert<T>(-1.0));
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename... Args>
inline detail::swizzle_exchanger_type<T, Derived, Args...>
point_<T, N, Derived>::swizzle(Args &&...) &
{
    static_assert(sizeof...(Args) >= 2, "swizzle requires at least 2 components.");

    static_assert(detail::is_in_range<Args...>(N), "swizzle component position out of range.");

#ifndef POINT_SWIZZLE_USE_MIXED_INDICATORS
    static_assert(detail::is_uniform_type<Args...>(),
        "define 'POINT_SWIZZLE_USE_MIXED_INDICATOR' to enable mixed swizzle indicator types.");
#endif

    /*URVO, no need for copy or move constructors*/
    return detail::swizzle_exchanger_type<T, Derived, Args...>{DR(*this)._v};
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename... Args>
inline detail::const_swizzle_exchanger_type<T, Derived, Args...>
point_<T, N, Derived>::swizzle(Args &&...) const &
{
    static_assert(sizeof...(Args) >= 2, "swizzle requires at least 2 components.");

    static_assert(detail::is_in_range<Args...>(N), "swizzle component position out of range.");

#ifndef POINT_SWIZZLE_USE_MIXED_INDICATORS
    static_assert(detail::is_uniform_type<Args...>(),
        "define 'POINT_SWIZZLE_USE_MIXED_INDICATOR' to enable mixed swizzle indicator types.");
#endif

    /*URVO, no need for copy or move constructors*/
    return detail::const_swizzle_exchanger_type<T, Derived, Args...>{CDR(*this)._v};
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename... Args>
inline Derived<T, sizeof...(Args)> point_<T, N, Derived>::swizzle(Args &&... args) &&
{
    return CDR(*this).swizzle(std::forward<Args>(args)...);
}

template<typename T, std::size_t N, template<typename, std::size_t> class Derived>
template<typename... Args>
inline Derived<T, sizeof...(Args)> point_<T, N, Derived>::swizzle(Args &&... args) const &&
{
    return CDR(*this).swizzle(std::forward<Args>(args)...);
}

#undef DR
#undef CDR

/** 
 * @brief Macro for replacing swizzle function parameters with the actual version
 * @attention Maximum variadic argument number: 64
 */
#define swizzle(...) swizzle(POINT_SWIZZLE_CONVERT_PARAMETERS(__VA_ARGS__))

SEL_NAMESPACE_END
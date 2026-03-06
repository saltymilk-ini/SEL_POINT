#pragma once

#include "point_base.hpp"

SEL_NAMESPACE_BEGIN

/**
 *@class point<T, N>
 *@brief Point of any dimension and any arithmetic value type.
 */
template<typename T, std::size_t N>
class point : public point_<T, N, point>
{
    using point_base = point_<T, N, point>;

public:
    using typename point_base::value_type;

    value_type _v[N];

public:
    point() : _v{} {}

    /**
     * @brief Construct a point with specified components.
     * @note Use scalar<U> to eliminate the instantiation as the copy constructor.
     * The braced initialization ensures that narrowing conversions are forbidden.
    */
    template<typename... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
    explicit point(detail::scalar<Args>&&... args) : _v{ std::forward<Args>(args)... } {}

    /** 
     * @brief Construct a point with all components equal to val.
     * @note Use scalar<U> to eliminate the instantiation as the copy constructor.
     */
    template<typename U>
    explicit point(detail::scalar<U> val)
    {
        detail::safe_numeric_conversion_assert<U, T>();
        
        value_type value = detail::convert<value_type>(val);

        detail::vector_scalar_opt(_v, typename point_base::component_position_list{}, value,
            [](auto, auto rhs) { return rhs; });
    }

    /**
     * @brief Construct by an arithmetic-type array.
     * @attention If a braced initialization is used to construct a point, any unspecified
     * component is set to 0 by default.
     */
    template<typename U>
    explicit point(const detail::scalar<U>(&arr)[N])
    {
        detail::safe_numeric_conversion_assert<U, T>();

        if constexpr (std::is_same_v<T, U>)
            memcpy(_v, arr, N * sizeof(value_type));
        else
            detail::vector_vector_opt(_v, typename point_base::component_position_list{}, arr,
                [](auto, auto rhs) { return detail::convert<value_type>(rhs); });
    }

    template<typename U, typename V>
    point(const point<U, N - 1> &rhs, detail::scalar<V> last)
    {
        detail::safe_element_conversion_assert<point<U, N - 1>, point<T, N - 1>>();

        if constexpr (std::is_same_v<value_type, U>)
            memcpy(_v, rhs._v, (N - 1) * sizeof(value_type));
        else
            detail::vector_vector_opt(_v, detail::arithmetic_progression<0, 1, N - 1>{}, rhs._v,
                [](auto, auto rhs) { return detail::convert<value_type>(rhs); });

        _v[N - 1] = detail::convert<T>(last);
    }
};

/**
 *@class point<T, 2>
 *@brief 2 dimension point of any arithmetic value type.
 */
template<typename T>
class point<T, 2> : public point_<T, 2, point>
{
    using point_base = point_<T, 2, point>;

public:
    using typename point_base::value_type;

    union
    {
        struct{ value_type x, y; };
        value_type _v[2];
    };

public:
    point() : _v{} {}

    template<typename U>
    point(detail::scalar<U> _x, detail::scalar<U> _y) : x(detail::convert<T>(_x)), y(detail::convert<T>(_y)) {}

    /** 
     * @brief Construct a point with all components equal to val.
     * @note Use scalar<U> to eliminate the instantiation as the copy constructor.
     */
    template<typename U>
    explicit point(detail::scalar<U> val) : point(val, val) {}

    template<typename U>
    inline void set(detail::scalar<U> _x, detail::scalar<U> _y)
    {
        x = detail::convert<T>(_x);
        y = detail::convert<T>(_y);
    }

    /** @brief Cross product.*/
    inline value_type operator^(const point &rhs) const
    {
        return x * rhs.y - y * rhs.x;
    }
};

/**
 *@class point<T, 3>
 *@brief 3 dimension point of any arithmetic value type.
 */
template<typename T>
class point<T, 3> : public point_<T, 3, point>
{
    using point_base = point_<T, 3, point>;

public:
    using typename point_base::value_type;

    union
    {
        struct{ value_type x, y, z; };
        struct{ value_type r, g, b; };
        value_type _v[3];
    };

public:
    point() : _v{} {}

    template<typename U>
    point(detail::scalar<U> _x, detail::scalar<U> _y, detail::scalar<U> _z)
        : x(detail::convert<T>(_x)), y(detail::convert<T>(_y)), z(detail::convert<T>(_z)) {}

    /** 
     * @brief Construct a point with all components equal to val.
     * @note Use scalar<U> to eliminate the instantiation as the copy constructor.
     */
    template<typename U>
    explicit point(detail::scalar<U> val) : point(val, val, val) {}

    /** @brief Construct a 3 dimension point from a 2 dimension point.*/
    template<typename U, typename V>
    point(const point<U, 2> &point2, detail::scalar<V> s) 
        : point(detail::convert<value_type>(point2.x), detail::convert<value_type>(point2.y),
                detail::convert<value_type>(s)) {}

    template<typename U>
    inline void set(detail::scalar<U> _x, detail::scalar<U> _y, detail::scalar<U> _z)
    {
        x = detail::convert<T>(_x);
        y = detail::convert<T>(_y);
        z = detail::convert<T>(_z);
    }

    /** Cross product.*/
    inline const point operator^(const point &rhs) const
    {
        return point(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
    }
};

/**
 *@class point<T, 4>
 *@brief 4 dimension point of any arithmetic value type.
 */
template<typename T>
class point<T, 4> : public point_<T, 4, point>
{
    using point_base = point_<T, 4, point>;

public:
    using typename point_base::value_type;

    union
    {
        struct{ value_type x, y, z, w; };
        struct{ value_type r, g, b, a; };
        value_type _v[4];
    };

public:
    point() : _v{} {}

    template<typename U>
    point(detail::scalar<U> _x, detail::scalar<U> _y, detail::scalar<U> _z, detail::scalar<U> _w)
        : x(detail::convert<T>(_x)), y(detail::convert<T>(_y)), z(detail::convert<T>(_z)), w(detail::convert<T>(_w)) {}

    /** 
     * @brief Construct a point with all components equal to val.
     * @note Use scalar<U> to eliminate the instantiation as the copy constructor.
     */
    template<typename U>
    explicit point(detail::scalar<U> val) : point(val, val, val, val) {}

    /** @brief Construct a 4 dimension point from a 3 dimension point.*/
    template<typename U, typename V>
    point(const point<U, 3> &point3, detail::scalar<V> s) 
        : point(detail::convert<value_type>(point3.x), detail::convert<value_type>(point3.y),
                detail::convert<value_type>(point3.z), detail::convert<value_type>(s)) {}

    template<typename U>
    inline void set(detail::scalar<U> _x, detail::scalar<U> _y, detail::scalar<U> _z, detail::scalar<U> _w)
    {
        x = detail::convert<T>(_x);
        y = detail::convert<T>(_y);
        z = detail::convert<T>(_z);
        w = detail::convert<T>(_w);
    }
};

/** @brief Dot product.*/
template<typename T1, typename T2, std::size_t N>
inline auto operator*(const point<T1, N> &lhs, const point<T2, N> &rhs)
{
    using product_type = std::common_type_t<T1, T2>;

    product_type product = detail::scalar_default<product_type>;

    const T1(&lv)[N] = lhs._v;
    const T2(&rv)[N] = rhs._v;

    for (std::size_t i = 0; i != N; ++i)
        product += detail::convert<product_type>(lv[i]) * detail::convert<product_type>(rv[i]);

    return product;
}

template<typename T1, typename T2, std::size_t N>
inline bool operator<(const point<T1, N> &lhs, const point<T2, N> &rhs)
{
    if (reinterpret_cast<const void *>(&lhs) == reinterpret_cast<const void *>(&rhs))
        return false;

    using compare_type = std::common_type_t<T1, T2>;

    for (std::size_t i = 0; i != N; ++i)
    {
        compare_type l = detail::convert<compare_type>(lhs[i]);
        compare_type r = detail::convert<compare_type>(rhs[i]);

        if (l < r)
            return true;
        else if (l > r)
            return false;
    }

    return false;
}

template<typename T1, typename T2, std::size_t N>
inline bool operator==(const point<T1, N> &lhs, const point<T2, N> &rhs)
{
    if (reinterpret_cast<const void *>(&lhs) == reinterpret_cast<const void *>(&rhs))
        return true;

    using compare_type = std::common_type_t<T1, T2>;

    return detail::vector_cmpare_and(lhs._v, detail::arithmetic_progression<0, 1, N>{}, rhs._v,
        [](auto a, auto b) { return detail::convert<compare_type>(a) == detail::convert<compare_type>(b); });
}

template<typename T1, typename T2, std::size_t N>
inline bool operator!=(const point<T1, N> &lhs, const point<T2, N> &rhs)
{
    return !(lhs == rhs);
}

SEL_NAMESPACE_END
/*
MIT License
Copyright (c) 2026 saltymilk-ini
The full license is in LICENSE file included with this distribution at https://github.com/saltymilk-ini/SEL_POINT
*/

#pragma once

#include "./scalar.hpp"
#include "./safe_conversion.hpp"

#include <utility>

/**
 * @file exchanger
 * @brief The exchanger is a mediator class, it allows data to exchange between elements.
 * @note An exchanger is actually a reference wrapper of the element from which it was created, it stores
 * the pointer of the runtime data, and the compile-time offset information. 
 * @warning Even if the copy and move constructors are explicitly deleted, mandatory copy elision after
 * C++17(https://en.cppreference.com/w/cpp/language/copy_elision.html) allows returning local objects
 * without invoking them. <br>
 * If an exchanger is still accessible after destruction of the element it references, it holds
 * dangling pointers of the already destroyed memory.<br>
 * So do not write code like this to make it happen:
 * @code
 * //#1 function returns exchanger created from local element
 * auto foo()
 * {
 *   sel::a_specific_element element;
 *   return some_way_to_create_an_exchanger(element);
 * }
 * auto ec = foo();//now ec holds dangling pointers, any operation on it is dangerous
 * @endcode
 * @code
 * //#2 a pointer of exchanger created from local element
 * sel::exchanger<the_known_parameters> *pec = nullptr;
 * {
 *   sel::a_specific_element element;
 *   pec = new auto(some_way_to_create_an_exchanger(element));
 * }
 * @endcode
 * This kind of usage is ok:
 * @code
 * sel::a_specific_element element;
 * auto ec = some_way_to_create_an_exchanger(element);
 * if(condition)
 *   ec = some_value;
 * else
 *   ec = other_value;
 * @endcode
 * In brief, always make sure that an exchanger lives no longer than the element from which it was created.
 */

SEL_NAMESPACE_BEGIN

SEL_DETAIL_BEGIN

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
struct const_exchanger
{
    enum : std::size_t { N = sizeof...(Seq) };

    const T *_ptr;

    explicit const_exchanger(const T *ptr) : _ptr(ptr) {}

    template<typename U = T>
    Point<U, N> cast() const noexcept
    {
        return Point<U, N>{ convert<U>(_ptr[Seq])... };
    }

    template<typename U>
    operator Point<U, N>() const noexcept
    {
        safe_numeric_conversion_assert<T, U>();

        return this->cast<U>();
    }

    const_exchanger() = delete;
    const_exchanger(const const_exchanger &) = delete;
    const_exchanger &operator=(const const_exchanger &) = delete;
    const_exchanger(const_exchanger &&) noexcept = delete;
    const_exchanger &operator=(const_exchanger &&) noexcept = delete;
};

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
struct exchanger
{
    enum : std::size_t { N = sizeof...(Seq) };

    T *_ptr;

    explicit exchanger(T *ptr) : _ptr(ptr) {}

    template<typename U = T>
    Point<U, N> cast() const noexcept
    {
        return Point<U, N>{ convert<U>(_ptr[Seq])... };
    }

    template<typename U>
    operator Point<U, N>() const noexcept
    {
        safe_numeric_conversion_assert<T, U>();

        return this->cast<U>();
    }

    template<typename U>
    exchanger &operator=(const Point<U, N> &p) noexcept
    {
        safe_numeric_conversion_assert<U, T>();
        
        const U *src = p._v;

        ((_ptr[Seq] = convert<T>(*src++)), ...);

        return *this;
    }

    exchanger &operator=(const exchanger &rhs) noexcept
    {
        if (this == &rhs)
            return *this;

        // rhs has the same T and Seq as this.
        ((_ptr[Seq] = rhs._ptr[Seq]), ...);

        return *this;
    }

    exchanger &operator=(exchanger &&rhs) noexcept
    {
        if (this == &rhs)
            return *this;

        // rhs has the same T and Seq as this.
        ((_ptr[Seq] = rhs._ptr[Seq]), ...);

        rhs._ptr = nullptr;

        return *this;
    }

    template<typename U, std::size_t... SrcSeq,
            typename = std::enable_if_t<N == sizeof...(SrcSeq)>>
    exchanger &operator=(const exchanger<U, Point, SrcSeq...> &rhs) noexcept
    {
        // no need to check self assignment, since the type of rhs is different from this.
        
        safe_numeric_conversion_assert<U, T>();

        ((_ptr[Seq] = convert<T>(rhs._ptr[SrcSeq])), ...);

        return *this;
    }

    template<typename U, std::size_t... SrcSeq,
            typename = std::enable_if_t<N == sizeof...(SrcSeq)>>
    exchanger &operator=(exchanger<U, Point, SrcSeq...> &&rhs) noexcept
    {
        // no need to check self assignment, since the type of rhs is different from this.

        safe_numeric_conversion_assert<U, T>();

        ((_ptr[Seq] = convert<T>(rhs._ptr[SrcSeq])), ...);

        rhs._ptr = nullptr;

        return *this;
    }

    template<typename U, std::size_t... SrcSeq,
            typename = std::enable_if_t<N == sizeof...(SrcSeq)>>
    exchanger &operator=(const const_exchanger<U, Point, SrcSeq...> &rhs) noexcept
    {
        safe_numeric_conversion_assert<U, T>();

        ((_ptr[Seq] = convert<T>(rhs._ptr[SrcSeq])), ...);

        return *this;
    }

    template<typename U, std::size_t... SrcSeq,
            typename = std::enable_if_t<N == sizeof...(SrcSeq)>>
    exchanger &operator=(const_exchanger<U, Point, SrcSeq...> &&rhs) noexcept
    {
        safe_numeric_conversion_assert<U, T>();

        ((_ptr[Seq] = convert<T>(rhs._ptr[SrcSeq])), ...);

        rhs._ptr = nullptr;

        return *this;
    }

    template<typename U, std::size_t M, typename = std::enable_if_t<M == N>>
    exchanger &operator=(const scalar<U>(&arr)[M]) noexcept
    {
        safe_numeric_conversion_assert<U, T>();

        const U *src = arr;

        ((_ptr[Seq] = convert<T>(*src++)), ...);

        return *this;
    }

    exchanger() = delete;
    exchanger(const exchanger &) = delete;
    exchanger(exchanger &&) noexcept = delete;
};

template<typename, template<typename, std::size_t> class,
        std::size_t, std::size_t, typename>
struct exchanger_type_impl;

template<typename T, template<typename, std::size_t> class Point,
        std::size_t Base, std::size_t Step, std::size_t... Seq>
struct exchanger_type_impl<T, Point, Base, Step, std::index_sequence<Seq...>>
{
    using type = exchanger<T, Point, (Base + Step * Seq)...>;

    using const_type = const_exchanger<T, Point, (Base + Step * Seq)...>;
};

template<typename T, template<typename, std::size_t> class Point,
        std::size_t Base, std::size_t Step, std::size_t Count>
struct exchanger_type
{
    using type = typename exchanger_type_impl<T, Point, Base, Step, std::make_index_sequence<Count>>::type;

    using const_type = typename exchanger_type_impl<T, Point, Base, Step, std::make_index_sequence<Count>>::const_type;
};

SEL_DETAIL_END

SEL_NAMESPACE_END
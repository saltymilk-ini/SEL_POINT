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
 * @file exchanger.hpp
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
class const_exchanger
{
public:
    static constexpr std::size_t N = sizeof...(Seq);

    explicit const_exchanger(const T* ptr) noexcept : _ptr(ptr) { }

    template<typename U = T>
    auto cast() const noexcept -> Point<U, N>;

    /** Using sizeof...(Seq) rather than N is a workaround for MSVC. */
    template<typename U>
    operator Point<U, sizeof...(Seq)>() const noexcept;

    template<typename U>
    bool operator==(const Point<U, N>& p) const noexcept;

    template<typename U>
    bool operator!=(const Point<U, N>& p) const noexcept;

    const_exchanger() = delete;
    const_exchanger(const const_exchanger&) = delete;
    const_exchanger& operator=(const const_exchanger&) = delete;
    const_exchanger(const_exchanger&&) noexcept = delete;
    const_exchanger& operator=(const_exchanger&&) noexcept = delete;

protected:
    const T* _ptr;

    template<typename, template<typename, std::size_t> class, std::size_t...>
    friend class exchanger;
};

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
class exchanger : public const_exchanger<T, Point, Seq...>
{
    using base = const_exchanger<T, Point, Seq...>;

public:
    using base::N;

    exchanger() = delete;
    explicit exchanger(T* ptr) noexcept : base(ptr) { }

    template<typename U>
    exchanger& operator=(const Point<U, N>& p) noexcept;

    template<typename U, std::size_t... SrcSeq, typename = std::enable_if_t<N == sizeof...(SrcSeq)>>
    exchanger& operator=(const const_exchanger<U, Point, SrcSeq...>& rhs) noexcept;

    template<typename U, std::size_t... SrcSeq, typename = std::enable_if_t<N == sizeof...(SrcSeq)>>
    exchanger& operator=(const_exchanger<U, Point, SrcSeq...>&& rhs) noexcept;

    exchanger& operator=(const exchanger& rhs) noexcept;

    exchanger& operator=(exchanger&& rhs) noexcept;

    template<typename U, std::size_t M, typename = std::enable_if_t<M == N>>
    exchanger& operator=(const scalar<U>(&arr)[M]) noexcept;

private:
    template<bool SafeCvtCheck = true, typename U, std::size_t... SrcSeq, typename = std::enable_if_t<N == sizeof...(SrcSeq)>>
    void assign(const const_exchanger<U, Point, SrcSeq...>& rhs) noexcept;
};

template<typename, template<typename, std::size_t> class, std::size_t, std::size_t, typename>
struct exchanger_type_impl;

template<typename T, template<typename, std::size_t> class Point, std::size_t Base, std::size_t Step, std::size_t... Seq>
struct exchanger_type_impl<T, Point, Base, Step, std::index_sequence<Seq...>>
{
    using type = exchanger<T, Point, (Base + Step * Seq)...>;

    using const_type = const_exchanger<T, Point, (Base + Step * Seq)...>;
};

template<typename T, template<typename, std::size_t> class Point, std::size_t Base, std::size_t Step, std::size_t Count>
struct exchanger_type
{
    using type = typename exchanger_type_impl<T, Point, Base, Step, std::make_index_sequence<Count>>::type;

    using const_type = typename exchanger_type_impl<T, Point, Base, Step, std::make_index_sequence<Count>>::const_type;
};

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
template<typename U>
auto const_exchanger<T, Point, Seq...>::cast() const noexcept -> Point<U, N>
{
    return Point<U, N>{ convert<U>(_ptr[Seq])... };
}

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
template<typename U>
const_exchanger<T, Point, Seq...>::operator Point<U, sizeof...(Seq)>() const noexcept
{
    safe_numeric_conversion_assert<T, U>();

    return this->cast<U>();
}

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
template<typename U>
bool const_exchanger<T, Point, Seq...>::operator==(const Point<U, N>& p) const noexcept
{
    using compare_type = std::common_type_t<T, U>;

    const U* v = p._v;

    const bool eq = ((convert<compare_type>(_ptr[Seq]) == convert<compare_type>(*v++))&& ...);
    (void)v;//Silence the "Value assigned is not used" warning.

    return eq;
}

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
template<typename U>
bool const_exchanger<T, Point, Seq...>::operator!=(const Point<U, N>& p) const noexcept
{
    return !(this->operator==(p));
}

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
template<typename U>
exchanger<T, Point, Seq...>& exchanger<T, Point, Seq...>::operator=(const Point<U, N>& p) noexcept
{
    safe_numeric_conversion_assert<U, T>();

    const U* src = p._v;
    T* ptr = const_cast<T*>(this->_ptr);
    ((ptr[Seq] = convert<T>(*src++)), ...);
    (void)src;//Silence the "Value assigned is not used" warning.

    return *this;
}

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
template<typename U, std::size_t... SrcSeq, typename>
exchanger<T, Point, Seq...>& exchanger<T, Point, Seq...>::operator=(const const_exchanger<U, Point, SrcSeq...>& rhs) noexcept
{
    if constexpr (std::is_same_v<T, U> && ((Seq == SrcSeq)&& ...))
    {
        if (this == &rhs)
            return *this;
    }

    assign(rhs);

    return *this;
}

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
template<typename U, std::size_t... SrcSeq, typename>
exchanger<T, Point, Seq...>& exchanger<T, Point, Seq...>::operator=(const_exchanger<U, Point, SrcSeq...>&& rhs) noexcept
{
    if constexpr (std::is_same_v<T, U> && ((Seq == SrcSeq)&& ...))
    {
        if (this == &rhs)
            return *this;
    }

    assign(rhs);

    rhs._ptr = nullptr;

    return *this;
}

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
exchanger<T, Point, Seq...>& exchanger<T, Point, Seq...>::operator=(const exchanger& rhs) noexcept
{
    return this->operator=(static_cast<const base&>(rhs));
}

template<typename T, template<typename, std::size_t> class Point, std::size_t... Seq>
exchanger<T, Point, Seq...>& exchanger<T, Point, Seq...>::operator=(exchanger&& rhs) noexcept
{
    return this->operator=(static_cast<base&&>(rhs));
}

template<typename T, template <typename, std::size_t> class Point, std::size_t... Seq>
template<typename U, std::size_t M, typename>
exchanger<T, Point, Seq...>& exchanger<T, Point, Seq...>::operator=(const scalar<U>(&arr)[M]) noexcept
{
    safe_numeric_conversion_assert<U, T>();

    const U* src = arr;
    T* ptr = const_cast<T*>(this->_ptr);
    ((ptr[Seq] = convert<T>(*src++)), ...);
    (void)src;//Silence the "Value assigned is not used" warning.

    return *this;
}

template<typename T, template <typename, std::size_t> class Point, std::size_t... Seq>
template<bool SafeCvtCheck, typename U, std::size_t... SrcSeq, typename>
void exchanger<T, Point, Seq...>::assign(const const_exchanger<U, Point, SrcSeq...>& rhs) noexcept
{
    if constexpr (SafeCvtCheck == true)
        safe_numeric_conversion_assert<U, T>();

    this->operator=(rhs.template cast<T>());
}

SEL_DETAIL_END

SEL_NAMESPACE_END
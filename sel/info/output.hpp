/*
MIT License
Copyright (c) 2026 saltymilk-ini
The full license is in LICENSE file included with this distribution at https://github.com/saltymilk-ini/SEL_POINT
*/

#pragma once

#include "./info.hpp"

SEL_NAMESPACE_BEGIN

template<typename OS, typename T, std::size_t N>
inline OS &operator<<(OS &os, const point<T, N> &p)
{
    os << info(p) << '\n';

    return os;
}

SEL_NAMESPACE_END
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
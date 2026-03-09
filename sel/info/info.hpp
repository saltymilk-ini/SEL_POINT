/*
MIT License
Copyright (c) 2026 saltymilk-ini
The full license is in LICENSE file included with this distribution at https://github.com/saltymilk-ini/SEL_POINT
*/

#pragma once

#include "../elements/implement/point_def.hpp"

#include <string>
#include <sstream>

SEL_NAMESPACE_BEGIN

template<typename T, std::size_t N>
inline std::string point_string(const point<T, N> &p)
{
    std::ostringstream ss;
    for (std::size_t i = 0; i != N; ++i)
    {
        ss << p[i];

        if (i != N - 1)
            ss << ", ";
    }

    return ss.str();
}

template<typename T, std::size_t N>
inline std::string info(const point<T, N> &p)
{
    std::ostringstream ss;
    ss << "point:{" << point_string(p) << '}';

    return ss.str();
}

SEL_NAMESPACE_END
/*
MIT License
Copyright (c) 2026 saltymilk-ini
The full license is in LICENSE file included with this distribution at https://github.com/saltymilk-ini/SEL_POINT
*/

#pragma once

#include "../config.hpp"

#ifdef _MSC_VER
#define CPP_VERSION _MSVC_LANG
#else
#define CPP_VERSION __cplusplus
#endif

#if CPP_VERSION < 201703L
#error "this library needs to be built on C++17 or later version."
#endif

#define SEL_NAMESPACE_BEGIN \
    namespace sel           \
    {
#define SEL_NAMESPACE_END   }

#define SEL_DETAIL_BEGIN    \
    namespace detail        \
    {
#define SEL_DETAIL_END      }

SEL_NAMESPACE_BEGIN

using uchar = unsigned char;

SEL_NAMESPACE_END
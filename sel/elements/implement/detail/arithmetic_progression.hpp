/*
MIT License
Copyright (c) 2026 saltymilk-ini
The full license is in LICENSE file included with this distribution at https://github.com/saltymilk-ini/SEL_POINT
*/

#pragma once

#include "../../../global/global_def.hpp"

#include <utility>

SEL_NAMESPACE_BEGIN

SEL_DETAIL_BEGIN

template<std::size_t, std::size_t, typename>
struct arithmetic_progression_impl;

template<std::size_t Base, std::size_t Step, std::size_t... Seq>
struct arithmetic_progression_impl<Base, Step, std::index_sequence<Seq...>>
{
    using type = std::index_sequence<(Base + Step * Seq)...>;
};

template<std::size_t Base, std::size_t Step, std::size_t Count>
using arithmetic_progression = typename arithmetic_progression_impl<Base, Step, std::make_index_sequence<Count>>::type;

SEL_DETAIL_END

SEL_NAMESPACE_END
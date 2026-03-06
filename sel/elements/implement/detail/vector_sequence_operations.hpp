#pragma once
#include "../../../global/global_def.hpp"

#include <utility>

SEL_NAMESPACE_BEGIN

SEL_DETAIL_BEGIN

template<typename Opt, typename T, typename U, std::size_t... Seq>
inline void vector_scalar_opt(T *tar, std::index_sequence<Seq...>, U value, Opt &&opt)
{
    ((tar[Seq] = opt(tar[Seq], value)), ...);
}

template<typename Opt, std::size_t... TarSeq, typename T, typename U, std::size_t... SrcSeq>
inline void vector_vector_opt_helper(T *tar, const U *src, std::index_sequence<SrcSeq...>, Opt &&opt)
{
    ((tar[TarSeq] = opt(tar[TarSeq], src[SrcSeq])), ...);
}

template<typename Opt, typename T, typename U,
         std::size_t... TarSeq,
         typename SrcList = std::index_sequence<TarSeq...>,
         typename = std::enable_if_t<sizeof...(TarSeq) == SrcList::size()>>
inline void vector_vector_opt(T *tar, std::index_sequence<TarSeq...>, const U *src,Opt &&opt, SrcList list = SrcList{})
{
    vector_vector_opt_helper<Opt, TarSeq...>(tar, src, list, std::forward<Opt>(opt));
}

template<typename Opt, std::size_t... TarSeq, typename T, typename U, typename V,
         std::size_t... SrcSeq_1, std::size_t... SrcSeq_2>
inline void vector_combine_opt_helper(T *tar, const U *src_1, const V *src_2,
                                      std::index_sequence<SrcSeq_1...>, std::index_sequence<SrcSeq_2...>, Opt &&opt)
{
    ((tar[TarSeq] = opt(src_1[SrcSeq_1], src_2[SrcSeq_2])), ...);
}

template<typename Opt, typename T, typename U, typename V,
         std::size_t... TarSeq,
         typename SrcList_1 = std::index_sequence<TarSeq...>,
         typename SrcList_2 = std::index_sequence<TarSeq...>,
         typename = std::enable_if_t<sizeof...(TarSeq) == SrcList_1::size() && sizeof...(TarSeq) == SrcList_2::size()>>
inline void vector_combine_opt(T *tar, std::index_sequence<TarSeq...>,
                               const U *src_1, const V *src_2, Opt &&opt, SrcList_1 seq_1 = SrcList_1{}, SrcList_2 seq_2 = SrcList_2{})
{
    vector_combine_opt_helper<Opt, TarSeq...>(tar, src_1, src_2, seq_1, seq_2, std::forward<Opt>(opt));
}

template<typename Opt, std::size_t... Seq_1, typename T, std::size_t... Seq_2>
inline constexpr bool vector_cmpare_or_helper(const T *lhs, const T *rhs, std::index_sequence<Seq_2...>, Opt &&opt)
{
    return (opt(lhs[Seq_1], rhs[Seq_2]) || ...);
}

template<typename Opt, typename T, typename U, std::size_t... Seq_1, typename Seq_2 = std::index_sequence<Seq_1...>>
inline bool vector_cmpare_or(const T *lhs, std::index_sequence<Seq_1...>, const U *rhs, Opt &&opt, Seq_2 seq_2 = Seq_2{})
{
    return vector_cmpare_or_helper<Opt, Seq_1...>(lhs, rhs, seq_2, std::forward<Opt>(opt));
}

template<std::size_t... Seq_1, typename Opt, typename T, typename U, std::size_t... Seq_2>
inline constexpr bool vector_cmpare_and_helper(const T *lhs, const U *rhs, std::index_sequence<Seq_2...>, Opt &&opt)
{
    return (opt(lhs[Seq_1], rhs[Seq_2]) && ...);
}

template<typename Opt, typename T, typename U, std::size_t... Seq_1, typename Seq_2 = std::index_sequence<Seq_1...>>
inline bool vector_cmpare_and(const T *lhs, std::index_sequence<Seq_1...>, const U *rhs, Opt &&opt, Seq_2 seq_2 = Seq_2{})
{
    return vector_cmpare_and_helper<Seq_1...>(lhs, rhs, seq_2, std::forward<Opt>(opt));
}

template<typename Opt, typename T, std::size_t... Seq>
inline bool vector_logic_or(const T *v, std::index_sequence<Seq...>, Opt &&opt)
{
    return (opt(v[Seq]) || ...);
}

template<typename Opt, typename T, std::size_t... Seq>
inline bool vector_logic_and(const T *v, std::index_sequence<Seq...>, Opt &&opt)
{
    return (opt(v[Seq]) && ...);
}

SEL_DETAIL_END

SEL_NAMESPACE_END
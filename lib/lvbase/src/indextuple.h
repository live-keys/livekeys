#ifndef LVINDEXTUPLE_H
#define LVINDEXTUPLE_H


namespace lv{ namespace meta{

// Creates indexes from a set of variadic arguments
// ------------------------------------------------

template<int... I> struct IndexTuple{};

template<int I, typename IndexTuple, typename... Types>
struct MakeIndexTuple;

template<int I, int... Indexes, typename T, typename ... Types>
struct MakeIndexTuple<I, IndexTuple<Indexes...>, T, Types...>
{
    typedef typename MakeIndexTuple<I + 1, IndexTuple<Indexes..., I>, Types...>::type type;
};

template<int I, int... Indexes>
struct MakeIndexTuple<I, IndexTuple<Indexes...> >
{
    typedef IndexTuple<Indexes...> type;
};

template<typename ... Types>
struct make_indexes : MakeIndexTuple<0, IndexTuple<>, Types...>
{};

}} // namespace lv, namespace meta



#endif // LVINDEXTUPLE_H

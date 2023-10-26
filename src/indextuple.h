/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVINDEXTUPLE_H
#define LVINDEXTUPLE_H


namespace lv{ namespace meta{

// Creates indexes from a set of variadic arguments
// ------------------------------------------------

template<int... I> struct IndexTuple{};

template<int I, typename IndexTuple, typename... Types>
struct MakeIndexTuple;

/// \private
template<int I, int... Indexes, typename T, typename ... Types>
struct MakeIndexTuple<I, IndexTuple<Indexes...>, T, Types...>
{
    typedef typename MakeIndexTuple<I + 1, IndexTuple<Indexes..., I>, Types...>::type type;
};

/// \private
template<int I, int... Indexes>
struct MakeIndexTuple<I, IndexTuple<Indexes...> >
{
    typedef IndexTuple<Indexes...> type;
};

/// \private
template<typename ... Types>
struct make_indexes : MakeIndexTuple<0, IndexTuple<>, Types...>
{};

}} // namespace lv, namespace meta



#endif // LVINDEXTUPLE_H

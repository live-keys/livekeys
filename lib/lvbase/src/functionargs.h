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

#ifndef LVFUNCTIONARGS_H
#define LVFUNCTIONARGS_H

namespace lv{ namespace meta{

// Returns the number of arguments for a function
// ----------------------------------------------

template <typename R, typename ... Types> constexpr size_t getArgumentCount( R(*)(Types ...)){
   return sizeof...(Types);
}


}} // namespace lv, meta

#endif // LVFUNCTIONARGS_H

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

#include "sourcerange.h"

namespace lv{ namespace el{

Utf8 SourceRange::slice(const Utf8 &str) const{
    if ( length() > 0 )
        return str.substr(m_from, m_length);
    return Utf8();
}

std::string SourceRange::slice(const std::string &str) const{
    if ( length() > 0 )
        return str.substr(m_from, m_length);
    return std::string();
}

}} // namesapce lv, el

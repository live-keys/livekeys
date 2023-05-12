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

#ifndef LVSOURCERANGE_H
#define LVSOURCERANGE_H

#include "live/elements/compiler/lvelcompilerglobal.h"
#include "live/utf8.h"

#include "inttypes.h"

namespace lv{ namespace el{

class LV_ELEMENTS_COMPILER_EXPORT SourceRange{

public:
    SourceRange() : m_from(0), m_length(0){}
    SourceRange(uint32_t from, uint32_t len) : m_from(from), m_length(len){}

    uint32_t from() const{ return m_from; }
    uint32_t length() const{ return m_length; }
    uint32_t to() const{ return m_from + m_length; }
    bool isValid() const{ return m_length > 0; }

    Utf8 slice(const Utf8& str) const;
    std::string slice(const std::string& str) const;

private:
    uint32_t m_from;
    uint32_t m_length;
};

}} // namespace lv, el

#endif // LVSOURCERANGE_H

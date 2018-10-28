/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "live/exception.h"
#include <sstream>

namespace lv{

Exception::Exception(const std::string &message, int code)
    : m_message(message)
    , m_code(code)
    , m_line(0)
    , m_stackTrace(0)
{
}

Exception::Exception(const Exception &other)
    : m_message(other.m_message)
    , m_code(other.m_code)
    , m_line(other.m_line)
    , m_file(other.m_file)
    , m_functionName(other.m_functionName)
    , m_stackTrace(other.m_stackTrace)
{
}

std::string Exception::fileName() const{
    std::string::size_type pos = m_file.find('/');

#ifdef Q_OS_WIN
    if ( pos == std::string::npos ){
        pos = m_file.find('\\');
    }
#endif

    if ( pos != std::string::npos )
        return m_file.substr(pos + 1);

    return m_file;
}

std::string Exception::location() const{
    std::stringstream sstream;
    sstream << m_functionName << "(" << fileName() << ":" << line() << ")";
    return sstream.str();
}


}// namespace

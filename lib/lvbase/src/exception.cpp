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

class ExceptionPrivate{

public:
    ExceptionPrivate(const std::string& pmessage, int pcode)
        : message(pmessage)
        , code(pcode)
        , line(0)
        , stackTrace(nullptr)
    {
    }

    std::string message;
    int         code;
    int         line;
    std::string file;
    std::string functionName;

    StackTrace::Ptr stackTrace;
};

Exception::Exception(const std::string &message, int code)
    : m_d(new ExceptionPrivate(message, code))
{
}

Exception::Exception(const Exception &other)
    : m_d(new ExceptionPrivate(other.message(), other.code()))
{
    m_d->line         = other.line();
    m_d->file         = other.file();
    m_d->functionName = other.functionName();
    m_d->stackTrace   = other.stackTrace();
}

Exception &Exception::operator =(const Exception &other){
    m_d->message      = other.message();
    m_d->code         = other.code();
    m_d->line         = other.line();
    m_d->file         = other.file();
    m_d->functionName = other.functionName();
    m_d->stackTrace   = other.stackTrace();
    return *this;
}

Exception::~Exception(){
    delete m_d;
}

std::string Exception::fileName() const{
    std::string::size_type pos = m_d->file.find('/');

#ifdef Q_OS_WIN
    if ( pos == std::string::npos ){
        pos = m_d->file.find('\\');
    }
#endif

    if ( pos != std::string::npos )
        return m_d->file.substr(pos + 1);

    return m_d->file;
}

std::string Exception::location() const{
    std::stringstream sstream;
    sstream << m_d->functionName << "(" << fileName() << ":" << line() << ")";
    return sstream.str();
}

void Exception::assignSourceLocation(const std::string &file, int line, const std::string &functionName){
    m_d->file = file;
    m_d->line = line;
    m_d->functionName = functionName;
}

void Exception::assignStackTrace(const StackTrace::Ptr &st){
    m_d->stackTrace = st;
}

bool Exception::hasLocation() const{
    return m_d->line != 0;
}

bool Exception::hasStackTrace() const{
    return m_d->stackTrace.get() != 0;
}

const std::string &Exception::message() const{
    return m_d->message;
}

int Exception::code() const{
    return m_d->code;
}

int Exception::line() const{
    return m_d->line;
}

const std::string &Exception::file() const{
    return m_d->file;
}

const std::string &Exception::functionName() const{
    return m_d->functionName;
}

const StackTrace::Ptr &Exception::stackTrace() const{
    return m_d->stackTrace;
}

}// namespace

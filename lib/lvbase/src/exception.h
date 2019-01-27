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

#ifndef LVEXCEPTION_H
#define LVEXCEPTION_H

#include <exception>
#include <string>

#include "live/stacktrace.h"
#include "live/lvbaseglobal.h"

namespace lv{

class ExceptionPrivate;

class LV_BASE_EXPORT Exception : public std::exception{

public:
    typedef unsigned long long Code;

public:
    Exception(const std::string& message = "", Code code = 0);
    Exception(const Exception& other);
    Exception& operator = (const Exception& other);
    virtual ~Exception();

    bool hasLocation() const;
    bool hasStackTrace() const;

    const std::string& message() const;
    Code code() const;
    int line() const;
    const std::string& file() const;
    std::string fileName() const;
    const std::string& functionName() const;
    const StackTrace::Ptr& stackTrace() const;

    std::string location() const;

    static Exception::Code toCode(const std::string& str);
    static std::string fromCode(Exception::Code code);

    template<typename T> static T create(
        const std::string& message,
        Code code,
        const std::string& file = "",
        int line = 0,
        const std::string& functionName = "",
        StackTrace::Ptr stackTrace = StackTrace::Ptr(0)
    );

private:
    void assignSourceLocation(const std::string& file, int line, const std::string& functionName);
    void assignStackTrace(const StackTrace::Ptr& st);

    ExceptionPrivate* m_d;
};

template<typename T> T Exception::create(const std::string &message,
        Code code,
        const std::string &file,
        int line,
        const std::string &functionName,
        StackTrace::Ptr stackTrace)
{
    T e(message, code);
    e.assignSourceLocation(file, line, functionName);
    e.assignStackTrace(stackTrace);

    return e;
}

} // namespace

#define CREATE_EXCEPTION(_type, _message, _code) \
    lv::Exception::create<_type>((_message), (_code), __FILE__, __LINE__, __FUNCTION__, lv::StackTrace::capture())

#define THROW_EXCEPTION(_type, _message, _code) \
    throw CREATE_EXCEPTION(_type, _message, _code)


#endif // LVEXCEPTION_H

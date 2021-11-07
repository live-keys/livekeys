/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef LVEXCEPTION_H
#define LVEXCEPTION_H

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma warning(push)
#pragma warning(disable : 4275)
#endif

#include <exception>
#include <string>

#include "live/lvbaseglobal.h"
#include "live/stacktrace.h"
#include "live/utf8.h"

namespace lv{

class ExceptionPrivate;

class LV_BASE_EXPORT Exception : public std::exception{

public:
    /** unsigned long long */
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
        StackTrace::Ptr stackTrace = StackTrace::Ptr(nullptr)
    );
    template<typename T> static T create(
        const char* message,
        Code code,
        const std::string& file = "",
        int line = 0,
        const std::string& functionName = "",
        StackTrace::Ptr stackTrace = StackTrace::Ptr(nullptr)
    );
    template<typename T> static T create(
        const Utf8& message,
        Code code,
        const std::string& file = "",
        int line = 0,
        const std::string& functionName = "",
        StackTrace::Ptr stackTrace = StackTrace::Ptr(nullptr)
    );

private:
    void assignSourceLocation(const std::string& file, int line, const std::string& functionName);
    void assignStackTrace(const StackTrace::Ptr& st);

    ExceptionPrivate* m_d;
};

/** Create exception of given type */
template<typename T> T Exception::create(
        const std::string &message,
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

/** Create exception of given type */
template<typename T> T Exception::create(
        const char* message,
        Code code,
        const std::string &file,
        int line,
        const std::string &functionName,
        StackTrace::Ptr stackTrace)
{
    T e(std::string(message), code);
    e.assignSourceLocation(file, line, functionName);
    e.assignStackTrace(stackTrace);

    return e;
}

/** Create exception of given type */
template<typename T> T Exception::create(
        const Utf8& message,
        Code code,
        const std::string &file,
        int line,
        const std::string &functionName,
        StackTrace::Ptr stackTrace)
{
    T e(message.data(), code);
    e.assignSourceLocation(file, line, functionName);
    e.assignStackTrace(stackTrace);
    return e;
}




} // namespace

#define CREATE_EXCEPTION(_type, _message, _code) \
    lv::Exception::create<_type>((_message), (_code), __FILE__, __LINE__, __FUNCTION__, lv::StackTrace::capture())

#define THROW_EXCEPTION(_type, _message, _code) \
    throw CREATE_EXCEPTION(_type, _message, _code)


#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma warning(pop)
#endif

#endif // LVEXCEPTION_H

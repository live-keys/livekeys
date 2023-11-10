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
#include "live/sourcelocation.h"
#include "live/utf8.h"

namespace lv{

class SourceLocation;
class ExceptionPrivate;

class LV_BASE_EXPORT Exception : public std::exception{

public:
    class SourceTrace{
    public:
        SourceTrace(const SourceLocation& l = SourceLocation(), StackTrace::Ptr t = nullptr)
            : location(l), trace(t){}
        SourceTrace(int line, const std::string& filePath, const std::string& functionName, StackTrace::Ptr t = nullptr)
            : location(SourceLocation(SourcePoint::createFromLine(line), filePath, functionName)), trace(t){}

        SourceLocation  location;
        StackTrace::Ptr trace;
    };

    typedef unsigned long long Code;

public:
    Exception(const Utf8& message = "", Code code = 0, const SourceTrace& st = SourceTrace());
    Exception(const Exception& other);
    Exception& operator = (const Exception& other);
    virtual ~Exception();

    bool hasLocation() const;
    bool hasStackTrace() const;

    const SourceLocation& location() const;

    const std::string& message() const;
    Code code() const;
    int line() const;
    const std::string& file() const;
    std::string fileName() const;
    const std::string& functionName() const;
    const StackTrace::Ptr& stackTrace() const;

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
    T e(
        message,
        code,
        lv::Exception::SourceTrace(line, file, functionName, stackTrace)
    );
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
    T e(
        std::string(message),
        code,
        lv::Exception::SourceTrace(line, file, functionName, stackTrace)
    );
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
    T e(
        message,
        code,
        lv::Exception::SourceTrace(line, file, functionName, stackTrace)
    );
    return e;
}

} // namespace

#define SOURCE_TRACE() \
    (lv::Exception::SourceTrace(__LINE__, __FILE__, __FUNCTION__, lv::StackTrace::capture()))

#define CREATE_EXCEPTION(_type, _message, _code) \
    (_type(_message, _code, SOURCE_TRACE()))

#define THROW_EXCEPTION(_type, _message, _code) \
    throw CREATE_EXCEPTION(_type, _message, _code)


#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma warning(pop)
#endif

#endif // LVEXCEPTION_H

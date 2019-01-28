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

namespace{

Exception::Code encodeSingle(char c){
    if ( c == '~' )
        return 27;
    else if ( c >= 'A' && c <= 'Z' )
        return static_cast<Exception::Code>(c - 'A' + 1);
    else if ( c >= 'a' && c <= 'z' )
        return static_cast<Exception::Code>(c - 'a' + 1);
    else
        return 0;
}

}


/**
 * \class lv::Exception
 *
 * \brief Standard Exception used throughout LiveCV
 *
 * Contains an internal private object in accordance with the D-pointer design pattern.
 * Currently the structure contains typical Exception parameters such as message, line number,
 * function name, stack trace etc.
 * \ingroup lvbase
 *
 */


/// \private
class ExceptionPrivate{

public:
    ExceptionPrivate(const std::string& pmessage, Exception::Code pcode)
        : message(pmessage)
        , code(pcode)
        , line(0)
        , stackTrace(nullptr)
    {
    }

    std::string     message;
    Exception::Code code;
    int             line;
    std::string     file;
    std::string     functionName;

    StackTrace::Ptr stackTrace;
};

/**
 * \brief Standard exception constructor with message and code parameters
 */
Exception::Exception(const std::string &message, lv::Exception::Code code)
    : m_d(new ExceptionPrivate(message, code))
{
}

/**
 * \brief Copy constructor of the Exception class
 */
Exception::Exception(const Exception &other)
    : m_d(new ExceptionPrivate(other.message(), other.code()))
{
    m_d->line         = other.line();
    m_d->file         = other.file();
    m_d->functionName = other.functionName();
    m_d->stackTrace   = other.stackTrace();
}

/**
 * \brief Assignment operator of Exception, works in a similar way to a copy constructor
 */
Exception &Exception::operator =(const Exception &other){
    m_d->message      = other.message();
    m_d->code         = other.code();
    m_d->line         = other.line();
    m_d->file         = other.file();
    m_d->functionName = other.functionName();
    m_d->stackTrace   = other.stackTrace();
    return *this;
}

/**
 * \brief Simple destructor of Exception object
 *
 * Deletes the private object given by the D-pointer.
 */
Exception::~Exception(){
    delete m_d;
}

/**
 * \brief Extracts filename from given filepath and returns it
 */
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

/**
 * \brief Returns string representation of the place where the exception is thrown
 *
 * Includes function name, file name, line number.
 */
std::string Exception::location() const{
    std::stringstream sstream;
    sstream << m_d->functionName << "(" << fileName() << ":" << line() << ")";
    return sstream.str();
}

/**
 * \brief Encodes a specified string into an error code
 *
 * Useful for generating unique error codes. Note that there is a constraint on the
 * size of the string (<14), due to the number of characters that can be uniquely encoded.
 *
 * Only characters between 'a' to 'z' and 'A' to 'Z' will be converted, all of them as
 * lowercase. Additionaly the not (~) character is available to signify a negation.
 */
Exception::Code Exception::toCode(const std::string &str){
    Exception::Code multiplier = 1;
    Exception::Code result = 0;

    int encodedChars = 14;

    std::string s;
    s.reserve(encodedChars);

    for ( auto it = str.begin(); it != str.end(); ++it ){
        Exception::Code current = encodeSingle(*it);
        if ( current > 0 ){
            encodedChars--;
            s += *it;
        }
        if ( encodedChars == 0 )
            break;
    }

    for (auto rit = s.rbegin(); rit != s.rend(); ++rit){
        Exception::Code current = encodeSingle(*rit);
        if ( current < 28 ){
            result += current * multiplier;
            multiplier = multiplier * 28;
        }
    }
    return result;
}

/**
 * \brief Decodes an error code into a set of characters
 */
std::string Exception::fromCode(Exception::Code code){
    std::string result = "";

    while (code > 0) {
        unsigned long long remainder = code % 28;
        char digit = static_cast<char>(remainder + 97);
        if ( digit == 27 )
          digit = '~';
        result = (--digit) + result;
        code = (code - remainder) / 28;
    }

    return result;
}

void Exception::assignSourceLocation(const std::string &file, int line, const std::string &functionName){
    m_d->file = file;
    m_d->line = line;
    m_d->functionName = functionName;
}

void Exception::assignStackTrace(const StackTrace::Ptr &st){
    m_d->stackTrace = st;
}

/**
 * \brief Returns an indicator if the exception contains a line number where the exception happened.
 */
bool Exception::hasLocation() const{
    return m_d->line != 0;
}


/**
 * \brief Indicates if the stack trace was included in the Exception
 */
bool Exception::hasStackTrace() const{
    return m_d->stackTrace.get() != nullptr;
}

/**
 * \brief Returns the Exception message
 */
const std::string &Exception::message() const{
    return m_d->message;
}

/**
 * \brief Returns the Exception code
 */
Exception::Code Exception::code() const{
    return m_d->code;
}

/**
 * \brief Returns the line where the exception was thrown
 *
 * Should be used together with hasLocation()
 * \sa hasLocation()
 */
int Exception::line() const{
    return m_d->line;
}

/**
 * \brief Returns the filepath of the file where the exception was thrown.
 */
const std::string &Exception::file() const{
    return m_d->file;
}

/**
 * \brief Returns the function name where the exception was thrown
 */
const std::string &Exception::functionName() const{
    return m_d->functionName;
}

/**
 * \brief Returns the full stack trace of the thrown exception
 *
 * Note that this is using our internal StackTrace object!
 */
const StackTrace::Ptr &Exception::stackTrace() const{
    return m_d->stackTrace;
}

}// namespace

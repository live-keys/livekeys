#include "live/exception.h"

namespace lcv{

Exception::Exception(const QString &message, int code)
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

QString Exception::fileName() const{
    int pos = m_file.lastIndexOf('/');

#ifdef Q_OS_WIN
    if ( pos == -1 ){
        pos = m_file.lastIndexOf('\\');
    }
#endif

    if ( pos != -1 )
        return m_file.mid(pos + 1);

    return m_file;
}

QString Exception::location() const{
    return m_functionName + "(" + fileName() + ":" + QString::number(line()) + ")";
}


}// namespace

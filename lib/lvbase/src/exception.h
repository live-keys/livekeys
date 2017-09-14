#ifndef LCVEXCEPTIONS_H
#define LCVEXCEPTIONS_H

#include <exception>
#include <QString>

#include "live/stacktrace.h"
#include "live/lvbaseglobal.h"

namespace lcv{

class LVBASE_EXPORT Exception : public std::exception{

public:
    Exception(const QString& message = "", int code = 0);
    Exception(const Exception& other);

    virtual ~Exception(){}

    bool hasLocation() const;
    bool hasStackTrace() const;

    const QString& message() const;
    int code() const;
    int line() const;
    const QString& file() const;
    QString fileName() const;
    const QString& functionName() const;
    const StackTrace::Ptr& stackTrace() const;

    QString location() const;

    template<typename T> static T create(
        const QString& message,
        int code,
        const QString& file = "",
        int line = 0,
        const QString& functionName = "",
        StackTrace::Ptr stackTrace = StackTrace::Ptr(0)
    );


private:
    QString m_message;
    int     m_code;
    int     m_line;
    QString m_file;
    QString m_functionName;

    StackTrace::Ptr m_stackTrace;
};

inline bool Exception::hasLocation() const{
    return m_line != 0;
}

inline bool Exception::hasStackTrace() const{
    return m_stackTrace.data() != 0;
}

inline const QString &Exception::message() const{
    return m_message;
}

inline int Exception::code() const{
    return m_code;
}

inline int Exception::line() const{
    return m_line;
}

inline const QString &Exception::file() const{
    return m_file;
}

inline const QString &Exception::functionName() const{
    return m_functionName;
}

inline const StackTrace::Ptr &Exception::stackTrace() const{
    return m_stackTrace;
}

template<typename T> T Exception::create(
        const QString &message,
        int code,
        const QString &file,
        int line,
        const QString &functionName,
        StackTrace::Ptr stackTrace)
{
    T e(message, code);
    e.m_line = line;
    e.m_file = file;
    e.m_functionName = functionName;
    e.m_stackTrace = stackTrace;

    return e;
}

} // namespace

#define CREATE_EXCEPTION(_type, _message, _code) \
    lcv::Exception::create<_type>((_message), (_code), __FILE__, __LINE__, __FUNCTION__, lcv::StackTrace::capture())

#define THROW_EXCEPTION(_type, _message, _code) \
    throw CREATE_EXCEPTION(_type, _message, _code)


#endif // LCVEXCEPTIONS_H

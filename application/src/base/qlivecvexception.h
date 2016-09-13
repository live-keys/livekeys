#ifndef QLIVECVEXCEPTION_H
#define QLIVECVEXCEPTION_H

#include <exception>
#include <QString>

class QLiveCVException : public std::exception{

public:
    QLiveCVException(const QString& message, int code = 0);
    virtual ~QLiveCVException() throw();

    const QString& message() const throw();
    int code() const throw();

private:
    QString m_message;
    int     m_code;
};

inline const QString &QLiveCVException::message() const throw(){
    return m_message;
}

inline int QLiveCVException::code() const throw(){
    return m_code;
}

#endif // QLIVECVEXCEPTION_H

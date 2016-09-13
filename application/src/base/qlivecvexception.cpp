#include "qlivecvexception.h"

QLiveCVException::QLiveCVException(const QString &message, int code)
    : m_message(message)
    , m_code(code)
{
}

QLiveCVException::~QLiveCVException(){
}

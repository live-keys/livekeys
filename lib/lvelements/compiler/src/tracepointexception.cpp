#include "tracepointexception.h"

namespace lv{ namespace el{

TracePointException::TracePointException(const Utf8 &message, Code code, const SourceTrace &st)
    : Exception(message, code, st)
{
    m_messagePoints.push_back(message);
}

TracePointException::TracePointException(const TracePointException &e, const Utf8 &newMessage)
    : Exception(Utf8("%\n%").format(e.message(), newMessage), e.code(), Exception::SourceTrace(e.location(), e.stackTrace()))
    , m_messagePoints(e.messagePoints())
{
    m_messagePoints.push_back(newMessage);
}

TracePointException::TracePointException(const Exception &e, const Utf8 &newMessage)
    : Exception(Utf8("%\n%").format(e.message(), newMessage), e.code(), Exception::SourceTrace(e.location(), e.stackTrace()))
{
    m_messagePoints.push_back(Utf8(e.message()));
    m_messagePoints.push_back(newMessage);
}

TracePointException::TracePointException(const Exception &e)
    : Exception(e.message() , e.code(), Exception::SourceTrace(e.location(), e.stackTrace()))
{
    m_messagePoints.push_back(Utf8(e.message()));
}

}}// namespace lv, el

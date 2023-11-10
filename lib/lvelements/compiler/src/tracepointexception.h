#ifndef LVTRACEPOINTEXCEPTION_H
#define LVTRACEPOINTEXCEPTION_H

#include "live/elements/compiler/lvelcompilerglobal.h"
#include "live/exception.h"

namespace lv{ namespace el{

class LV_ELEMENTS_COMPILER_EXPORT TracePointException : public lv::Exception{

public:
    TracePointException(
        const Utf8& message,
        Exception::Code code,
        const Exception::SourceTrace& st = Exception::SourceTrace()
    );
    TracePointException(
        const TracePointException& e,
        const Utf8& pointMessage
    );
    TracePointException(
        const Exception& e,
        const Utf8& pointMessage
    );
    TracePointException(
        const Exception& e
    );

    const std::vector<Utf8>& messagePoints() const{ return m_messagePoints; }

private:
    Utf8 mergeMessagePoints(){ return Utf8::join(m_messagePoints, "\n"); }

    std::vector<Utf8> m_messagePoints;
};

} } // namespace lv, el

#endif // LVTRACEPOINTEXCEPTION_H

#include "stacktrace.h"

#ifdef USE_STACK_TRACE_BFD
#define BACKWARD_HAS_BFD 1
#endif
#include "backward.hpp"

namespace lcv{

StackTrace::Ptr StackTrace::capture(int maxFrames){
    StackTrace::Ptr dest(new StackTrace);

    backward::StackTrace st;
    st.load_here(maxFrames);

    backward::TraceResolver tr; tr.load_stacktrace(st);
    for (size_t i = 2; i < st.size(); ++i) {
        backward::ResolvedTrace trace = tr.resolve(st[i]);
        dest->m_frames->push_back(StackFrame(
            trace.object_function,
            (StackFrame::AddressPtr)trace.addr,
            trace.object_filename,
            trace.source.filename,
            trace.source.line
        ));
    }

    return dest;
}

}// namespace



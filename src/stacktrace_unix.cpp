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

#include "stacktrace.h"

#ifdef USE_STACK_TRACE_BFD
#define BACKWARD_HAS_BFD 1
#endif
#include "backward.hpp"

namespace lv{

/** Capture frames up to a given maximum number */
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



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
#include "live/visuallog.h"

/**
  \class lv::StackTrace
  \brief Captures and holds a stacktrace.
  \ingroup lvbase
 */

namespace lv{

// StackFrame
// -----------------------------------------------------------------


StackFrame::StackFrame(const Utf8 &functionName,
        StackFrame::AddressPtr address,
        const Utf8 &objectPath,
        const Utf8 &filePath,
        int line)
    : m_functionName(functionName)
    , m_address(address)
    , m_objectPath(objectPath)
    , m_filePath(filePath)
    , m_line(line)
{
}

Utf8 StackFrame::fileName() const{
    return Path::name(m_filePath.data());
}

VisualLog &operator <<(VisualLog &vl, const StackFrame &value){
    if ( value.line() ){
        vl << "at " << value.functionName() << "(" << value.fileName() << ":" << value.line()
           << ")" << "[" << Utf8::numberToHex(value.address()) << "]";
    } else {
        vl << "at " << value.functionName() << "[" << Utf8::numberToHex(value.address()) << "]";
    }
    return vl;
}

// StackTrace
// -----------------------------------------------------------------

/** \brief Destructor of StackTrace
 *
 * Simply removes the contained StackFrames.
*/
StackTrace::~StackTrace(){
    delete m_frames;
}

StackTrace::StackTrace()
    : m_frames(new std::vector<StackFrame>)
{
}

VisualLog &operator <<(VisualLog &vl, const StackTrace &value){
    for ( StackTrace::ConstIterator it = value.begin(); it != value.end(); ++it ){
        if ( it != value.begin() )
            vl << "\n";
        vl << *it;
    }
    return vl;
}


}// namespace

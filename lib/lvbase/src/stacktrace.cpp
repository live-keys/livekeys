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

#include "stacktrace.h"
#include "live/visuallog.h"
#include <QString>

namespace lv{


// StackFrame
// -----------------------------------------------------------------

StackFrame::StackFrame(
        const std::string &functionName,
        StackFrame::AddressPtr address,
        const std::string &objectPath,
        const std::string &filePath,
        int line)
    : m_functionName(functionName)
    , m_address(address)
    , m_objectPath(objectPath)
    , m_filePath(filePath)
    , m_line(line)
{
}

StackFrame::~StackFrame(){
}

const std::string &StackFrame::functionName() const{
    return m_functionName;
}

const std::string &StackFrame::filePath() const{
    return m_filePath;
}

std::string StackFrame::fileName() const{
    std::string::size_type pos = m_filePath.rfind('/');

#ifdef Q_OS_WIN
    if ( pos == std::string::npos ){
        pos = m_filePath.rfind('\\');
    }
#endif

    if ( pos != std::string::npos ){
        return m_filePath.substr(pos + 1);
    }

    return m_filePath;
}

int StackFrame::line() const{
    return m_line;
}

StackFrame::AddressPtr StackFrame::address() const{
    return m_address;
}

const std::string &StackFrame::objectPath() const{
    return m_objectPath;
}

bool StackFrame::hasLocation() const{
    return m_line != -1;
}

VisualLog &operator <<(VisualLog &vl, const StackFrame &value){
    if ( value.line() ){
        vl << "at " << value.functionName().c_str() << "(" << value.fileName().c_str() << ":" << value.line()
           << ")" << "[0x" << QString::number(value.address(), 16).toStdString() << "]";
    } else {
        vl << "at " << value.functionName().c_str() << "[0x" << QString::number(value.address(), 16).toStdString() << "]";
    }
    return vl;
}

// StackTrace
// -----------------------------------------------------------------

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

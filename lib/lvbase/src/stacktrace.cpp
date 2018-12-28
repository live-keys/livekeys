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


/**
  \class lv::StackTrace
  \brief Captures and holds a stacktrace.
  \ingroup lvbase
 */

namespace lv{

std::string pathFileName(const std::string &path){
    std::string::size_type pos = path.rfind('/');

#ifdef Q_OS_WIN
    if ( pos == std::string::npos ){
        pos = path.rfind('\\');
    }
#endif

    if ( pos != std::string::npos ){
        return path.substr(pos + 1);
    }

    return path;
}

// StackFrame
// -----------------------------------------------------------------


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

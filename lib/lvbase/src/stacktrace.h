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

#ifndef LVSTACKTRACE_H
#define LVSTACKTRACE_H

#include <string>
#include <vector>
#include <QSharedPointer>

#include "live/lvbaseglobal.h"

namespace lv{

class VisualLog;

// StackFrame
// ----------

class LV_BASE_EXPORT StackFrame{

public:
    typedef unsigned long long AddressPtr;

public:
    StackFrame(
        const std::string& functionName,
        AddressPtr address,
        const std::string& objectPath = "",
        const std::string& filePath = "",
        int line = -1
    );
    ~StackFrame();

    const std::string& functionName() const;
    AddressPtr address() const;

    const std::string& objectPath() const;
    const std::string& filePath() const;
    std::string fileName() const;
    int line() const;

    bool hasLocation() const;

private:
    std::string m_functionName;
    AddressPtr  m_address;
    std::string m_objectPath;
    std::string m_filePath;
    int         m_line;
};

LV_BASE_EXPORT VisualLog &operator <<(VisualLog &vl, const StackFrame &value);

// StackTrace
// ----------

class LV_BASE_EXPORT StackTrace{


public:
    typedef std::vector<StackFrame>                 Container;
    typedef std::vector<StackFrame>::const_iterator ConstIterator;
    typedef QSharedPointer<StackTrace> Ptr;

    friend class StackTraceCapture;

public:
    static Ptr capture(int maxFrames = 128);

    ConstIterator begin() const;
    ConstIterator end() const;
    size_t size() const;

    ~StackTrace();

private:
    StackTrace();
    StackTrace(const StackTrace&);
    StackTrace& operator = (const StackTrace&);

    std::vector<StackFrame>* m_frames;
};

inline StackTrace::ConstIterator StackTrace::begin() const{
    return m_frames->cbegin();
}

inline StackTrace::ConstIterator StackTrace::end() const{
    return m_frames->cend();
}

inline size_t StackTrace::size() const{
    return m_frames->size();
}

LV_BASE_EXPORT VisualLog &operator <<(VisualLog &vl, const StackTrace &value);

}// namespace

#endif // LVSTACKTRACE_H

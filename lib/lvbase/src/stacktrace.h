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
#include <memory>

#include "live/lvbaseglobal.h"

namespace lv{

class VisualLog;

// StackFrame
// ----------

LV_BASE_EXPORT std::string pathFileName(const std::string& path);

class StackFrame{

public:
    typedef unsigned long long AddressPtr;

public:
    StackFrame(
        const std::string& functionName,
        AddressPtr address,
        const std::string& objectPath = "",
        const std::string& filePath = "",
        int line = -1
    )
        : m_functionName(functionName)
        , m_address(address)
        , m_objectPath(objectPath)
        , m_filePath(filePath)
        , m_line(line)
    {
    }

    ~StackFrame(){}

    const std::string& functionName() const{ return m_functionName; }
    AddressPtr address() const{ return m_address; }

    const std::string& objectPath() const{ return m_objectPath; }
    const std::string& filePath() const{ return m_filePath; }
    std::string fileName() const{ return pathFileName(m_filePath); }
    int line() const{ return m_line; }

    bool hasLocation() const{ return m_line != -1; }

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
    typedef std::shared_ptr<StackTrace> Ptr;

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

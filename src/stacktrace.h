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

#ifndef LVSTACKTRACE_H
#define LVSTACKTRACE_H

#include <string>
#include <vector>
#include <memory>

#include "live/lvbaseglobal.h"
#include "live/path.h"
#include "live/utf8.h"

namespace lv{

class VisualLog;

// StackFrame
// ----------

/**
  \class lv::StackFrame
  \brief A frame of debug-related data that gets pushed to the stack trace.

  Very simple structure, containing only getters of its various fields.
  \ingroup lvbase
 */
class LV_BASE_EXPORT StackFrame{

public:
    /**
     * \brief unsigned long long
     */
    typedef unsigned long long AddressPtr;

public:
    /**
      \brief Default constructor, only function name and addr. pointer are mandatory parameters
    */
    StackFrame(
        const Utf8& functionName,
        AddressPtr address,
        const Utf8& objectPath = "",
        const Utf8& filePath = "",
        int line = -1
    );

    /**
      \brief Default (empty) destructor
    */
    ~StackFrame(){}

    /**
      \brief Function name getter
    */
    const Utf8& functionName() const{ return m_functionName; }
    /**
     * \brief Adress pointer getter
     */
    AddressPtr address() const{ return m_address; }

    /**
     * \brief Object path getter
     */
    const Utf8& objectPath() const{ return m_objectPath; }
    /**
     * \brief File path getter
     */
    const Utf8& filePath() const{ return m_filePath; }
    /**
     * \brief File name getter (extracted from file path)
     */
    Utf8 fileName() const;
    /**
     * \brief Line number getter
     */
    int line() const{ return m_line; }

    /**
     * \brief Shows if we have a line number field
     */
    bool hasLocation() const{ return m_line != -1; }

private:
    Utf8       m_functionName;
    AddressPtr m_address;
    Utf8       m_objectPath;
    Utf8       m_filePath;
    int        m_line;
};

LV_BASE_EXPORT VisualLog &operator <<(VisualLog &vl, const StackFrame &value);

// StackTrace
// ----------

class LV_BASE_EXPORT StackTrace{

public:
    /** vector of StackFrames */
    typedef std::vector<StackFrame>                 Container;
    /** Const iterator for a stack frame vector */
    typedef std::vector<StackFrame>::const_iterator ConstIterator;
    /** Shared pointer to StackTrace */
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

/** Start iterator or the stackframes */
inline StackTrace::ConstIterator StackTrace::begin() const{
    return m_frames->cbegin();
}

/** End iterator of the stackframes */
inline StackTrace::ConstIterator StackTrace::end() const{
    return m_frames->cend();
}

/** Number of stackframes */
inline size_t StackTrace::size() const{
    return m_frames->size();
}

LV_BASE_EXPORT VisualLog &operator <<(VisualLog &vl, const StackTrace &value);

}// namespace

#endif // LVSTACKTRACE_H

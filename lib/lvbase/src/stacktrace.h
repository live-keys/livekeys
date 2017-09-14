#ifndef STACKTRACE_H
#define STACKTRACE_H

#include <string>
#include <vector>
#include <QSharedPointer>

#include "lvbaseglobal.h"

namespace lcv{

class VisualLog;

// StackFrame
// ----------

class LVBASE_EXPORT StackFrame{

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

VisualLog &operator <<(VisualLog &vl, const StackFrame &value);


// StackTrace
// ----------

class LVBASE_EXPORT StackTrace{


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

    friend VisualLog& operator << (VisualLog& vl, const StackTrace& value);

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

}// namespace

#endif // STACKTRACE_H

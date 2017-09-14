#include "stacktrace.h"
//#include "live/visuallog.h"
#include <QSharedPointer>

namespace lcv{


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

//TODO
//VisualLog &operator <<(VisualLog &vl, const StackFrame &value){
//    if ( value.line() ){
//        vl << "at " << value.name().c_str() << "(" << value.fileName().c_str() << ":" << value.line()
//           << ")" << "[0x" << QString::number(value.address(), 16) << "]";
//    } else {
//        vl << "at " << value.name().c_str() << "[0x" << QString::number(value.address(), 16) << "]";
//    }
//    return vl;
//}

// StackTrace
// -----------------------------------------------------------------

StackTrace::~StackTrace(){
    delete m_frames;
}

StackTrace::StackTrace()
    : m_frames(new std::vector<StackFrame>)
{
}

//VisualLog &operator <<(VisualLog &vl, const StackTrace &value){
//    for ( StackTrace::ConstIterator it = value.begin(); it != value.end(); ++it ){
//        if ( it != value.begin() )
//            vl << "\n";
//        vl << *it;
//    }
//    return vl;
//}

}// namespace

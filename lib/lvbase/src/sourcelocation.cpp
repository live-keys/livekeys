#include "sourcelocation.h"

namespace lv{

SourcePoint::SourcePoint(int line, int column, int offset)
    : m_line(line), m_column(column), m_offset(offset)
{}

SourcePoint SourcePoint::createFromLine(int line){
    return SourcePoint(line, 0);
}

SourceLocation::SourceLocation(
        const SourcePoint &point, const std::string &filePath, const std::string &functionName)
    : m_point(point)
    , m_filePath(filePath)
    , m_functionName(functionName)
{
}

std::string SourceLocation::fileName() const{
    std::string::size_type pos = m_filePath.find('/');

#ifdef PLATFORM_OS_WIN
    if ( pos == std::string::npos ){
        pos = m_filePath.find('\\');
    }
#endif

    if ( pos != std::string::npos )
        return m_filePath.data().substr(pos + 1);

    return m_filePath.data();
}

std::string SourceLocation::toString() const{
    std::stringstream sstream;
    sstream << m_functionName << "(" << fileName() << ":" << m_point.line() << ")";
    return sstream.str();
}

SourceRange::SourceRange(const SourcePoint &start, const SourcePoint &end)
    : m_start(start), m_end(end)
{}

}// namespace

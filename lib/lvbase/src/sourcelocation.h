#ifndef LVSOURCELOCATION_H
#define LVSOURCELOCATION_H

#include "live/lvbaseglobal.h"
#include "live/utf8.h"

namespace lv{

class LV_BASE_EXPORT SourcePoint{

public:
    SourcePoint(int line, int column, int offset);
    SourcePoint(int line, int column): m_line(line), m_column(column), m_offset(-1){}
    SourcePoint(int offset): m_line(0), m_column(0), m_offset(offset){}
    SourcePoint() : m_line(0), m_column(0), m_offset(-1){}

    bool hasLine() const{ return m_line > 0; }
    bool hasColumn() const{ return m_column > 0; }
    bool hasOffset() const{ return m_offset > 0; }

    int line() const{ return m_line; }
    int column() const{ return m_column; }
    int offset() const{ return m_offset; }

    static SourcePoint createFromLine(int line);

private:
    int m_line;
    int m_column;
    int m_offset;
};

class LV_BASE_EXPORT SourceLocation{

public:
    SourceLocation(
        const SourcePoint& point = SourcePoint(),
        const std::string& filePath = "",
        const std::string& functionName = ""
    );

    const SourcePoint& point() const{ return m_point; }
    const std::string& filePath() const{ return m_filePath.data(); }
    const std::string& functionName() const{ return m_functionName.data(); }

    std::string fileName() const;
    std::string toString() const;

private:
    SourcePoint m_point;
    Utf8        m_filePath;
    Utf8        m_functionName;
};

class LV_BASE_EXPORT SourceRange{

public:
    SourceRange(const SourcePoint& start, const SourcePoint& end);

    const SourcePoint& start(){ return m_start; }
    const SourcePoint& end(){ return m_end; }

private:
    SourcePoint m_start;
    SourcePoint m_end;
};

#define SOURCE_LOCATION() \
    (lv::SourceLocation(lv::SourcePoint::fromLine(__LINE__), __FILE__, __FUNCTION__))

}

#endif // LVSOURCELOCATION_H

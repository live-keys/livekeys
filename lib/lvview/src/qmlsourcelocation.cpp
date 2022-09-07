#include "qmlsourcelocation.h"

namespace lv{

QmlSourceLocation::QmlSourceLocation(const QString &file, int line, int column, int offset)
    : m_file(file)
    , m_line(line)
    , m_column(column)
    , m_offset(offset)
{
}


}// namespace

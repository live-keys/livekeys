#ifndef LVQMLSOURCELOCATION_H
#define LVQMLSOURCELOCATION_H

#include "live/lvviewglobal.h"

#include <QString>
#include <QJSValue>

namespace lv{

class LV_VIEW_EXPORT QmlSourceLocation{

public:
    QmlSourceLocation(const QString& file, int line = -1, int column = -1, int offset = -1);

    const QString& file() const;
    int line() const;
    int column() const;
    int offset() const;

private:
    QString m_file;
    int     m_line;
    int     m_column;
    int     m_offset;
};

inline const QString &QmlSourceLocation::file() const{
    return m_file;
}

inline int QmlSourceLocation::line() const{
    return m_line;
}

inline int QmlSourceLocation::column() const{
    return m_column;
}

inline int lv::QmlSourceLocation::offset() const{
    return m_offset;
}

}// namespace

#endif // LVQMLSOURCELOCATION_H

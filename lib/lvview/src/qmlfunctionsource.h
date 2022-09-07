#ifndef LVQMLFUNCTIONSOURCE_H
#define LVQMLFUNCTIONSOURCE_H

#include <QString>
#include "live/lvviewglobal.h"
#include "live/viewengine.h"

namespace lv{

class LV_VIEW_EXPORT QmlFunctionSource{

public:
    QmlFunctionSource(
        const QJSValue& val,
        const QString& source,
        const QByteArray& imports,
        const QmlSourceLocation& location);
    ~QmlFunctionSource();

    QJSValue& fn();
    const QString& source() const;
    const QByteArray& imports() const;
    const QString& path() const;
    int line() const;
    int column() const;
    int offset() const;

    static QmlFunctionSource* createCompiled(ViewEngine* ve, const QByteArray& imports, const QString& source, const QmlSourceLocation& location);

private:
    QJSValue m_fn;
    QString m_source;
    QByteArray m_imports;
    QmlSourceLocation m_location;
};

inline QJSValue &QmlFunctionSource::fn(){
    return m_fn;
}

inline const QString &QmlFunctionSource::source() const{
    return m_source;
}

inline const QByteArray &QmlFunctionSource::imports() const{
    return m_imports;
}

inline const QString &QmlFunctionSource::path() const{
    return m_location.file();
}

inline int QmlFunctionSource::line() const{
    return m_location.line();
}

inline int QmlFunctionSource::column() const{
    return m_location.column();
}

inline int QmlFunctionSource::offset() const{
    return m_location.offset();
}

}// namespace

#endif // LVQMLFUNCTIONSOURCE_H



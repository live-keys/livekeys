#ifndef QQMLLIBRARYDEPENDENCY_H
#define QQMLLIBRARYDEPENDENCY_H

#include "qqmljsparserglobal.h"
#include <QString>

namespace lcv{

class Q_QMLJSPARSER_EXPORT QQmlLibraryDependency{

public:
    QQmlLibraryDependency(const QString& uri, int versionMajor, int versionMinor);
    ~QQmlLibraryDependency();

    const QString& uri() const;
    QString path() const;
    int versionMajor() const;
    int versionMinor() const;

    static QQmlLibraryDependency parse(const QString& import);

    static int parseInt(const QStringRef &str, bool *ok);
    static bool parseVersion(const QString &str, int *major, int *minor);

    bool isValid() const;

    QString toString() const;

private:
    QString m_uri;
    int     m_versionMajor;
    int     m_versionMinor;
};

inline int QQmlLibraryDependency::versionMajor() const{
    return m_versionMajor;
}

inline int QQmlLibraryDependency::versionMinor() const{
    return m_versionMinor;
}

inline bool QQmlLibraryDependency::isValid() const{
    return !m_uri.isEmpty() && m_versionMajor > -1 && m_versionMinor > -1;
}

inline QString QQmlLibraryDependency::toString() const{
    return m_uri + " " + QString::number(m_versionMajor) + "." + QString::number(m_versionMinor);
}

inline const QString& QQmlLibraryDependency::uri() const{
    return m_uri;
}

}// namespace

#endif // QQMLLIBRARYDEPENDENCY_H

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

#ifndef LVQMLLIBRARYDEPENDENCY_H
#define LVQMLLIBRARYDEPENDENCY_H

#include "live/lveditqmljsglobal.h"
#include <QString>

namespace lv{

/// \private
class QmlLibraryDependency{

public:
    QmlLibraryDependency(const QString& uri, int versionMajor, int versionMinor);
    ~QmlLibraryDependency();

    const QString& uri() const;
    QString path() const;
    int versionMajor() const;
    int versionMinor() const;

    static QmlLibraryDependency parse(const QString& import);

    static int parseInt(const QStringRef &str, bool *ok);
    static bool parseVersion(const QString &str, int *major, int *minor);

    bool isValid() const;

    QString toString() const;

private:
    QString m_uri;
    int     m_versionMajor;
    int     m_versionMinor;
};

inline int QmlLibraryDependency::versionMajor() const{
    return m_versionMajor;
}

inline int QmlLibraryDependency::versionMinor() const{
    return m_versionMinor;
}

inline bool QmlLibraryDependency::isValid() const{
    return !m_uri.isEmpty() && m_versionMajor > -1 && m_versionMinor > -1;
}

inline QString QmlLibraryDependency::toString() const{
    return m_uri + " " + QString::number(m_versionMajor) + "." + QString::number(m_versionMinor);
}

inline const QString& QmlLibraryDependency::uri() const{
    return m_uri;
}

}// namespace

#endif // LVQMLLIBRARYDEPENDENCY_H

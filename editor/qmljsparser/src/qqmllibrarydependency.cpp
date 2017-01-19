/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "qqmllibrarydependency.h"

#include <QDebug>

namespace lcv{

QQmlLibraryDependency::QQmlLibraryDependency(const QString &uri, int versionMajor, int versionMinor)
    : m_uri(uri)
    , m_versionMajor(versionMajor)
    , m_versionMinor(versionMinor)
{
}

QQmlLibraryDependency::~QQmlLibraryDependency(){
}

QQmlLibraryDependency QQmlLibraryDependency::parse(const QString &import){
    int position = import.lastIndexOf(' ');
    if ( position == -1 )
        return QQmlLibraryDependency("", -1, -1);

    int major, minor;

    bool isVersion = parseVersion(import.mid(position + 1).trimmed(), &major, &minor);
    if ( !isVersion )
        return QQmlLibraryDependency("", -1, -1);

    return QQmlLibraryDependency(import.mid(0, position).trimmed(), major, minor);
}

int QQmlLibraryDependency::parseInt(const QStringRef &str, bool *ok){
    int pos = 0;
    int number = 0;
    while (pos < str.length() && str.at(pos).isDigit()) {
        if (pos != 0)
            number *= 10;
        number += str.at(pos).unicode() - '0';
        ++pos;
    }
    if (pos != str.length())
        *ok = false;
    else
        *ok = true;
    return number;
}

bool QQmlLibraryDependency::parseVersion(const QString &str, int *major, int *minor){
    const int dotIndex = str.indexOf(QLatin1Char('.'));
    if (dotIndex != -1 && str.indexOf(QLatin1Char('.'), dotIndex + 1) == -1) {
        bool ok = false;
        *major = parseInt(QStringRef(&str, 0, dotIndex), &ok);
        if (ok)
            *minor = parseInt(QStringRef(&str, dotIndex + 1, str.length() - dotIndex - 1), &ok);
        return ok;
    }
    return false;
}

QString QQmlLibraryDependency::path() const{
    QStringList splitPath = m_uri.split('.');
    QString res = splitPath.join(QLatin1Char('/'));
    if (res.isEmpty() && !splitPath.isEmpty())
        return QLatin1String("/");
    return res;
}

}// namespace

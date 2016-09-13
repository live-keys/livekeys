/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#ifndef QLIVECV_H
#define QLIVECV_H

#include <QString>
#include <QLibrary>

// Versioning
// ----------

#define LIVECV_VERSION_MAJOR 1
#define LIVECV_VERSION_MINOR 2
#define LIVECV_VERSION_PATCH 0

// Forward declarations
// --------------------

class QGuiApplication;
class QQmlApplicationEngine;
class QCodeDocument;
class QUrl;

// class QLiveCV
// -------------

class QLiveCV{

public:
    QLiveCV(const QStringList &arguments);
    ~QLiveCV();

    void loadLibrary(const QString& library);
    void loadQml(const QUrl& url);

    static int versionMajor();
    static int versionMinor();
    static int versionPatch();
    static QString versionString();

    const QString& dir() const;

private:
    QLiveCV(const QLiveCV&);
    QLiveCV& operator = (const QLiveCV&);

    void parseArguments(const QStringList& arguments);
    void solveImportPaths();

    QQmlApplicationEngine* m_engine;

    QCodeDocument* m_document;
    QString m_dir;
    QLibrary m_lcvlib;

};

inline int QLiveCV::versionMajor(){
    return LIVECV_VERSION_MAJOR;
}

inline int QLiveCV::versionMinor(){
    return LIVECV_VERSION_MINOR;
}

inline int QLiveCV::versionPatch(){
    return LIVECV_VERSION_PATCH;
}

inline QString QLiveCV::versionString(){
    return
        QString::number(versionMajor()) + "." +
        QString::number(versionMinor()) + "." +
        QString::number(versionPatch());
}

inline const QString &QLiveCV::dir() const{
    return m_dir;
}

#endif // QLIVECV_H

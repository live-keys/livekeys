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

#ifndef QLIVECV_H
#define QLIVECV_H

#include <QString>
#include <QLibrary>

// Versioning
// ----------

#define LIVECV_VERSION_MAJOR 1
#define LIVECV_VERSION_MINOR 3
#define LIVECV_VERSION_PATCH 0

// Forward declarations
// --------------------

class QGuiApplication;
class QQmlApplicationEngine;
class QStaticContainer;
class QUrl;

namespace lcv{

class QLiveCVEngine;
class QLiveCVArguments;
class QLiveCVSettings;
class QProject;
class QDocumentCodeInterface;
class QDocumentQmlHandler;

// class QLiveCV
// -------------

class QLiveCV{

public:
    QLiveCV(int argc, const char* const argv[]);
    ~QLiveCV();

    void loadLibrary(const QString& library);
    void loadQml(const QUrl& url);

    static int versionMajor();
    static int versionMinor();
    static int versionPatch();
    static QString versionString();

    const QString& dir() const;

    static void registerTypes();

    const QLiveCVArguments* arguments() const;

    QByteArray extractPluginInfo(const QString& import) const;

private:
    QLiveCV(const QLiveCV&);
    QLiveCV& operator = (const QLiveCV&);

    void parseArguments(const QStringList& arguments);
    void solveImportPaths();

    QLiveCVEngine* m_engine;
    QLiveCVArguments* m_arguments;
    QStaticContainer* m_staticContainer;

    lcv::QDocumentCodeInterface* m_codeInterface;
    QString  m_dir;
    QLibrary m_lcvlib;

    lcv::QProject*        m_project;
    lcv::QLiveCVSettings* m_settings;

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

inline const QLiveCVArguments *QLiveCV::arguments() const{
    return m_arguments;
}

}// namespace

#endif // QLIVECV_H

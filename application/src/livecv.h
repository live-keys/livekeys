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
#define LIVECV_VERSION_MINOR 4
#define LIVECV_VERSION_PATCH 1

// Forward declarations
// --------------------

class QGuiApplication;
class QQmlApplicationEngine;
class QUrl;

namespace lcv{

class Engine;
class LiveCVArguments;
class Settings;
class LiveCVScript;
class QProject;
class Commands;
class QDocumentHandler;
class QCodeQmlHandler;

// class QLiveCV
// -------------

class LiveCV : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::Settings* settings READ settings CONSTANT)
    Q_PROPERTY(lcv::Engine*   engine   READ engine   CONSTANT)
    Q_PROPERTY(lcv::QProject* project  READ project  CONSTANT)
    Q_PROPERTY(lcv::Commands* commands READ commands CONSTANT)

public:
    LiveCV(int argc, const char* const argv[], QObject* parent = 0);
    ~LiveCV();

    void loadLibrary(const QString& library);
    void loadQml(const QUrl& url);

    static int versionMajor();
    static int versionMinor();
    static int versionPatch();
    static QString versionString();

    const QString& dir() const;

    static void registerTypes();

    const LiveCVArguments* arguments() const;

    QByteArray extractPluginInfo(const QString& import) const;

    Settings* settings();
    Engine*   engine();
    QProject* project();
    Commands* commands();

private:
    LiveCV(const LiveCV&);
    LiveCV& operator = (const LiveCV&);

    void parseArguments(const QStringList& arguments);
    void solveImportPaths();

    Engine* m_engine;
    LiveCVArguments* m_arguments;

    lcv::QDocumentHandler* m_codeInterface;
    QString  m_dir;
    QLibrary m_lcvlib;

    lcv::QProject*        m_project;
    lcv::Settings*        m_settings;
    lcv::LiveCVScript*   m_script;
    lcv::Commands*        m_commands;

};

inline int LiveCV::versionMajor(){
    return LIVECV_VERSION_MAJOR;
}

inline int LiveCV::versionMinor(){
    return LIVECV_VERSION_MINOR;
}

inline int LiveCV::versionPatch(){
    return LIVECV_VERSION_PATCH;
}

inline QString LiveCV::versionString(){
    return
        QString::number(versionMajor()) + "." +
        QString::number(versionMinor()) + "." +
        QString::number(versionPatch());
}

inline const QString &LiveCV::dir() const{
    return m_dir;
}

inline const LiveCVArguments *LiveCV::arguments() const{
    return m_arguments;
}

inline Settings *LiveCV::settings(){
    return m_settings;
}

inline Engine *LiveCV::engine(){
    return m_engine;
}

inline QProject *LiveCV::project(){
    return m_project;
}

inline Commands *LiveCV::commands(){
    return m_commands;
}

}// namespace

#endif // QLIVECV_H

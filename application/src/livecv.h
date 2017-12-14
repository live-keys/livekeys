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

#ifndef LIVECV_H
#define LIVECV_H

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

namespace lv{

class LiveCVArguments;
class LiveCVScript;
class Engine;
class Settings;
class Commands;
class KeyMap;
class VisualLogModel;
class VisualLogJsObject;
class Project;
class DocumentHandler;
class CodeQmlHandler;

// class LiveCV
// ------------

class LiveCV : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::Settings*       settings READ settings CONSTANT)
    Q_PROPERTY(lv::Engine*         engine         READ engine   CONSTANT)
    Q_PROPERTY(lv::Commands*       commands       READ commands       CONSTANT)
    Q_PROPERTY(lv::VisualLogModel* log            READ log            CONSTANT)
    Q_PROPERTY(lv::KeyMap*         keymap         READ keymap         CONSTANT)

public:
    typedef QSharedPointer<LiveCV>       Ptr;
    typedef QSharedPointer<const LiveCV> ConstPtr;

public:
    ~LiveCV();

    static LiveCV::Ptr create(int argc, const char* const argv[], QObject* parent = 0);

    void loadQml(const QUrl& url);

    static int versionMajor();
    static int versionMinor();
    static int versionPatch();
    static QString versionString();
    static QString header();

    const QString& dir() const;

    void loadInternalPlugins();

    const LiveCVArguments* arguments() const;

    QByteArray extractPluginInfo(const QString& import) const;

    Settings* settings();
    Engine*   engine();
    Project* project();
    Commands* commands();
    KeyMap* keymap();
    VisualLogModel* log();

public slots:
    QObject *windowControls() const;

private:
    LiveCV(QObject* parent = 0);
    LiveCV(const LiveCV&);
    LiveCV& operator = (const LiveCV&);

    void parseArguments(const QStringList& arguments);
    void solveImportPaths();

    Engine*          m_engine;
    LiveCVArguments* m_arguments;

    lv::DocumentHandler* m_codeInterface;
    QString  m_dir;

    lv::Project*           m_project;
    lv::Settings*          m_settings;
    lv::LiveCVScript*      m_script;
    lv::Commands*          m_commands;
    lv::KeyMap*            m_keymap;
    lv::VisualLogModel*    m_log;
    lv::VisualLogJsObject* m_vlog;
    mutable QObject*       m_windowControls;
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

inline Project *LiveCV::project(){
    return m_project;
}

inline Commands *LiveCV::commands(){
    return m_commands;
}

inline KeyMap *LiveCV::keymap(){
    return m_keymap;
}

inline VisualLogModel *LiveCV::log(){
    return m_log;
}

inline QString LiveCV::header(){
    return " Live CV v" + versionString() + "\n"
           " --------------------------------------------------- ";
}

}// namespace

#endif // LIVECV_H

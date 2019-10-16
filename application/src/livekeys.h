/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef LVLIVEKEYS_H
#define LVLIVEKEYS_H

#include <QString>
#include <QLibrary>
#include <QQmlPropertyMap>
#include <QJSValue>

#include <functional>

#include "live/package.h"

// Versioning
// ----------

#define LIVEKEYS_VERSION_MAJOR 1
#define LIVEKEYS_VERSION_MINOR 6
#define LIVEKEYS_VERSION_PATCH 0

// Forward declarations
// --------------------

class QGuiApplication;
class QQmlApplicationEngine;
class QUrl;

namespace lv{

class LivekeysArguments;
class QmlScript;
class ViewEngine;
class Settings;
class Memory;
class Commands;
class KeyMap;
class VisualLogModel;
class VisualLogQmlObject;
class Project;
class DocumentHandler;
class CodeQmlHandler;
class Extensions;
class PackageGraph;
class Layer;

// class Livekeys
// --------------

class Livekeys : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::Settings*       settings       READ settings   CONSTANT)
    Q_PROPERTY(lv::ViewEngine*     engine         READ engine     CONSTANT)
    Q_PROPERTY(lv::VisualLogModel* log            READ log        CONSTANT)
    Q_PROPERTY(lv::Memory*         mem            READ memory     CONSTANT)
    Q_PROPERTY(QQmlPropertyMap*    layers         READ layers     CONSTANT)

public:
    typedef QSharedPointer<Livekeys>       Ptr;
    typedef QSharedPointer<const Livekeys> ConstPtr;

public:
    ~Livekeys();

    static Livekeys::Ptr create(int argc, const char* const argv[], QObject* parent = nullptr);

    void loadQml(const QUrl& url);
    void loadProject();

    static int versionMajor();
    static int versionMinor();
    static int versionPatch();
    static QString versionString();
    static QString header();

    const QString& dir() const;

    void addDefaultLayers();
    void addLayer(const QString& name, const QString &layer);
    void loadLayer(const QString& layerName, std::function<void(Layer *)> onReady = nullptr);
    void loadLayers(const QStringList& layers, std::function<void(Layer*)> onReady = nullptr);
    void loadDefaultLayers();

    void loadInternals();
    void loadInternalPlugins();
    void loadInternalPackages();

    void initializeProject();

    std::vector<std::string> packageImportPaths() const;

    const LivekeysArguments* arguments() const;

    QByteArray extractPluginInfo(const QString& import) const;

    Settings*   settings();
    ViewEngine* engine();
    Project*    project();
    Memory*     memory();
    VisualLogModel* log();
    QQmlPropertyMap* layers();

public slots:
    QObject* layerPlaceholder() const;
    void engineError(QJSValue error);
    void projectChanged(const QString& path);

signals:
    void layerReady(lv::Layer* layer);
    void missingPackages();

private:
    Livekeys(QObject* parent = nullptr);
    Livekeys(const Livekeys&);
    Livekeys& operator = (const Livekeys&);

    void parseArguments(const QStringList& arguments);
    void solveImportPaths();

    ViewEngine*      m_engine;
    LivekeysArguments* m_arguments;

    QString              m_dir;
    QStringList          m_engineImportPaths;

    lv::Project*           m_project;
    lv::Settings*          m_settings;
    lv::QmlScript*         m_script;
    lv::VisualLogModel*    m_log;
    lv::VisualLogQmlObject*m_vlog;
    lv::PackageGraph*      m_packageGraph;
    lv::Memory*            m_memory;

    QQmlPropertyMap*       m_layers;
    QMap<QString, QString> m_storedLayers;
    Layer*                 m_lastLayer;
    mutable QObject*       m_layerPlaceholder;
};

inline int Livekeys::versionMajor(){
    return LIVEKEYS_VERSION_MAJOR;
}

inline int Livekeys::versionMinor(){
    return LIVEKEYS_VERSION_MINOR;
}

inline int Livekeys::versionPatch(){
    return LIVEKEYS_VERSION_PATCH;
}

inline QString Livekeys::versionString(){
    return
        QString::number(versionMajor()) + "." +
        QString::number(versionMinor()) + "." +
        QString::number(versionPatch());
}

inline const QString &Livekeys::dir() const{
    return m_dir;
}

inline const LivekeysArguments *Livekeys::arguments() const{
    return m_arguments;
}

inline Settings *Livekeys::settings(){
    return m_settings;
}

inline ViewEngine *Livekeys::engine(){
    return m_engine;
}

inline Project *Livekeys::project(){
    return m_project;
}

inline VisualLogModel *Livekeys::log(){
    return m_log;
}

inline Memory *Livekeys::memory(){
    return m_memory;
}

inline QString Livekeys::header(){
    return " Livekeys v" + versionString() + "\n"
           " --------------------------------------------------- ";
}

}// namespace

#endif // LVLIVEKEYS_H

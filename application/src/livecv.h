/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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
#include <QQmlPropertyMap>
#include <QJSValue>
#include "live/package.h"

// Versioning
// ----------

#define LIVECV_VERSION_MAJOR 1
#define LIVECV_VERSION_MINOR 5
#define LIVECV_VERSION_PATCH 0

// Forward declarations
// --------------------

class QGuiApplication;
class QQmlApplicationEngine;
class QUrl;

namespace lv{

class LiveCVArguments;
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

// class LiveCV
// ------------

class LiveCV : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::Settings*       settings       READ settings   CONSTANT)
    Q_PROPERTY(lv::ViewEngine*     engine         READ engine     CONSTANT)
    Q_PROPERTY(lv::VisualLogModel* log            READ log        CONSTANT)
    Q_PROPERTY(lv::Memory*         mem            READ memory     CONSTANT)
    Q_PROPERTY(QQmlPropertyMap*    layers         READ layers     CONSTANT)

public:
    typedef QSharedPointer<LiveCV>       Ptr;
    typedef QSharedPointer<const LiveCV> ConstPtr;

public:
    ~LiveCV();

    static LiveCV::Ptr create(int argc, const char* const argv[], QObject* parent = 0);

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

    const LiveCVArguments* arguments() const;

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
    LiveCV(QObject* parent = 0);
    LiveCV(const LiveCV&);
    LiveCV& operator = (const LiveCV&);

    void parseArguments(const QStringList& arguments);
    void solveImportPaths();

    ViewEngine*      m_engine;
    LiveCVArguments* m_arguments;

    QString              m_dir;
    QStringList          m_engineImportPaths;

    lv::Project*           m_project;
    lv::Settings*          m_settings;
    lv::QmlScript*      m_script;
    lv::VisualLogModel*    m_log;
    lv::VisualLogQmlObject*m_vlog;
    lv::PackageGraph*      m_packageGraph;
    lv::Memory*            m_memory;

    QQmlPropertyMap*       m_layers;
    QMap<QString, QString> m_storedLayers;
    Layer*                 m_lastLayer;
    mutable QObject*       m_layerPlaceholder;
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

inline ViewEngine *LiveCV::engine(){
    return m_engine;
}

inline Project *LiveCV::project(){
    return m_project;
}

inline VisualLogModel *LiveCV::log(){
    return m_log;
}

inline Memory *LiveCV::memory(){
    return m_memory;
}

inline QString LiveCV::header(){
    return " Live CV v" + versionString() + "\n"
           " --------------------------------------------------- ";
}

}// namespace

#endif // LIVECV_H

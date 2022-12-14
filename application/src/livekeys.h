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

#ifndef LVLIVEKEYS_H
#define LVLIVEKEYS_H

#include <QString>
#include <QLibrary>
#include <QQmlPropertyMap>
#include <QJSValue>

#include <functional>

#include "live/package.h"
#include "live/lockedfileiosession.h"
#include "live/programholder.h"
#include "live/settings.h"
#include "live/visuallogmodel.h"

// Versioning
// ----------

#define LIVEKEYS_VERSION_MAJOR 1
#define LIVEKEYS_VERSION_MINOR 9
#define LIVEKEYS_VERSION_PATCH 1

// Forward declarations
// --------------------

class QGuiApplication;
class QQmlApplicationEngine;
class QUrl;

namespace lv{

class LivekeysArguments;
class ViewEngine;
class Memory;
class PackageGraph;
class Layer;

namespace el{ class Engine; }

// class Livekeys
// --------------

class Livekeys : public QObject, public ProgramHolder{

    Q_OBJECT
    Q_PROPERTY(lv::Settings*       settings  READ settings   CONSTANT)
    Q_PROPERTY(lv::VisualLogModel* log       READ log        CONSTANT)
    Q_PROPERTY(QQmlPropertyMap*    layers    READ layers     CONSTANT)

public:
    typedef QSharedPointer<Livekeys>       Ptr;
    typedef QSharedPointer<const Livekeys> ConstPtr;

public:
    ~Livekeys();

    static Livekeys::Ptr create(int argc, const char* const argv[], QObject* parent = nullptr);

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
    int exec(const QGuiApplication& app);

    void setMain(Program* program) override;
    Program* main() override;
    QString scriptPath() const;

    void loadInternals();
    void loadInternalPlugins();
    void loadInternalPackages();

    int run(const QGuiApplication& app);

    std::vector<std::string> packageImportPaths() const;

    const LivekeysArguments* arguments() const;

    Settings*   settings();
    ViewEngine* engine();
    VisualLogModel* log();
    QQmlPropertyMap* layers();

    static const MLNode& startupConfiguration();

public slots:
    QObject* layerPlaceholder() const;
    void engineError(QJSValue error);

signals:
    void layerReady(lv::Layer* layer);
    void missingPackages();

private:
    Livekeys(QObject* parent = nullptr);
    Livekeys(const Livekeys&);
    Livekeys& operator = (const Livekeys&);

    void loadConfiguredLayers();
    int execElements(const QGuiApplication& app);
    void parseArguments(const QStringList& arguments);
    void solveImportPaths();

    LockedFileIOSession::Ptr m_lockedFileIO;

    ViewEngine*        m_viewEngine;
    LivekeysArguments* m_arguments;

    QString              m_dir;
    QStringList          m_engineImportPaths;

    lv::el::Engine*        m_engine;

    lv::Program*           m_main;
    lv::Settings*          m_settings;
    lv::VisualLogModel*    m_log;
    lv::PackageGraph*      m_packageGraph;

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
    return m_viewEngine;
}

inline VisualLogModel *Livekeys::log(){
    return m_log;
}

inline QString Livekeys::header(){
    return " Livekeys v" + versionString() + "\n"
           " --------------------------------------------------- ";
}

}// namespace

#endif // LVLIVEKEYS_H

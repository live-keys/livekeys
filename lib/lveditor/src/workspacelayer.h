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

#ifndef LVWORKSPACELAYER_H
#define LVWORKSPACELAYER_H

#include <QObject>
#include <QQuickItem>
#include <QQmlPropertyMap>

#include "live/lvviewglobal.h"
#include "live/layer.h"
#include "live/keymap.h"

#include "workspacemessagestack.h"
#include "themecontainer.h"
#include "commands.h"
#include "extensions.h"
#include "documentation.h"
#include "startupmodel.h"
#include "projectfileindexer.h"

class QQuickCloseEvent;

namespace lv{

class FileFormatTypes;
class Project;
class Workspace;
class ProjectWorkspace;

/// \private
class WorkspaceLayer : public Layer{

    Q_OBJECT
    Q_PROPERTY(QObject* environment                 READ environment   NOTIFY environmentChanged)
    Q_PROPERTY(QObject* wizards                     READ wizards       NOTIFY wizardsChanged)
    Q_PROPERTY(QObject* panes                       READ panes         NOTIFY panesChanged)
    Q_PROPERTY(QObject* startup                     READ startup       NOTIFY startupChanged)
    Q_PROPERTY(lv::Project* project                 READ project       CONSTANT)
    Q_PROPERTY(lv::WorkspaceMessageStack* messages  READ messages      CONSTANT)
    Q_PROPERTY(lv::Commands* commands               READ commands      CONSTANT)
    Q_PROPERTY(lv::KeyMap* keymap                   READ keymap        CONSTANT)
    Q_PROPERTY(lv::ThemeContainer* themes           READ themes        CONSTANT)
    Q_PROPERTY(lv::FileFormatTypes* fileFormats     READ fileFormats   CONSTANT)
    Q_PROPERTY(lv::Documentation* documentation     READ documentation CONSTANT)
    Q_PROPERTY(lv::ProjectFileIndexer* fileIndexer  READ fileIndexer   CONSTANT)
    Q_PROPERTY(QQmlPropertyMap* extensions          READ extensions    CONSTANT)
    Q_PROPERTY(lv::StartupModel* recents            READ recents       CONSTANT)
    Q_PROPERTY(lv::StartupModel* tutorials          READ tutorials     CONSTANT)
    Q_PROPERTY(lv::StartupModel* samples            READ samples       CONSTANT)

public:
    explicit WorkspaceLayer(QObject *parent = nullptr);
    ~WorkspaceLayer() override;

    void initialize(const MLNode& config) override;
    void loadView(ViewEngine *engine, QObject *parent) override;
    QObject* nextViewParent() override;
    QObject * viewRoot() override;

    QObject* environment() const;
    QObject* wizards() const;
    QObject* panes() const;
    QObject* startup() const;
    lv::WorkspaceMessageStack* messages() const;

    lv::Project* project() const;
    lv::Commands* commands() const;
    lv::KeyMap* keymap() const;
    lv::FileFormatTypes* fileFormats() const;
    QQmlPropertyMap* extensions() const;
    lv::ThemeContainer* themes() const;
    lv::Documentation* documentation() const;
    Workspace* workspace() const;
    lv::StartupModel* recents() const;
    lv::StartupModel* tutorials() const;
    lv::StartupModel* samples() const;

    ProjectFileIndexer* fileIndexer() const;

public slots:
    QJSValue interceptMenu(QJSValue pane, QJSValue item);
    QJSValue interceptFile(const QString& path, const QString& format, int mode);

    QString docsPath() const;
    QString pluginsPath() const;

    void triggerTooltip(QObject* tooltip);
    void cancelTooltip(QObject* tooltip);

    bool wasRecentsFileFound() const;
    void saveRecentsToFile();

    void addPane(QQuickItem* pane, QQuickWindow* window, const QVariantList& position);
    void removePane(QQuickItem* pane);
    void addWindow(QQuickWindow* window);

    void whenProjectOpen(const QString& path, ProjectWorkspace* workspace);
    void whenProjectClose();

    void newProjectInstance();
    void openProjectInstance(const QUrl& path);

    void __mainWindowClosing();
    void __mainWindowActiveChanged();

    void __tooltipDestroyed();
    void __tooltipTimeout();

signals:
    void environmentChanged();
    void wizardsChanged();
    void panesChanged();
    void startupChanged();

private:
    void initializePanes(ProjectWorkspace* workspace, QJSValue panes);
    void initializePanesAndWindows(ProjectWorkspace* workspace, QJSValue panesAndWindows);

    void loadConfigurations();
    WorkspaceExtension* loadPackageExtension(const std::string& path, const std::string& component);
    WorkspaceExtension* loadPackageExtension(const Package::Ptr& package, const std::string& component);

    QObject* m_nextViewParent;

    QObject* m_environment;
    QObject* m_wizards;
    QObject* m_panes;
    QObject* m_startup;
    QObject* m_viewRoot;
    QObject* m_runSpace;

    WorkspaceMessageStack* m_messageStack;
    Commands* m_commands;
    KeyMap*   m_keymap;
    ThemeContainer* m_themes;
    FileFormatTypes* m_fileFormats;

    Project*       m_project;
    ViewEngine*    m_engine;
    QQuickWindow*  m_mainWindow;

    Extensions*    m_extensions;
    Workspace*     m_workspace;
    Documentation* m_documentation;

    StartupModel*  m_tutorials;
    StartupModel*  m_samples;

    QTimer*        m_tooltipTimer;
    QObject*       m_tooltip;

    lv::ProjectFileIndexer* m_fileIndexer;
};

inline QObject *WorkspaceLayer::environment() const{
    return m_environment;
}

inline QObject *WorkspaceLayer::wizards() const{
    return m_wizards;
}

inline QObject *WorkspaceLayer::panes() const{
    return m_panes;
}

inline QObject *WorkspaceLayer::startup() const{
    return m_startup;
}

inline Project *WorkspaceLayer::project() const{
    return m_project;
}

inline Commands *WorkspaceLayer::commands() const{
    return m_commands;
}

inline KeyMap *WorkspaceLayer::keymap() const{
    return m_keymap;
}

inline FileFormatTypes *WorkspaceLayer::fileFormats() const{
    return m_fileFormats;
}

inline QQmlPropertyMap *WorkspaceLayer::extensions() const{
    if ( m_extensions )
        return m_extensions->globals();
    return nullptr;
}

inline ThemeContainer *WorkspaceLayer::themes() const{
    return m_themes;
}

inline Documentation *WorkspaceLayer::documentation() const{
    return m_documentation;
}

inline Workspace *WorkspaceLayer::workspace() const{
    return m_workspace;
}

inline ProjectFileIndexer *WorkspaceLayer::fileIndexer() const{
    return m_fileIndexer;
}

}// namespace

#endif // LVWORKSPACELAYER_H

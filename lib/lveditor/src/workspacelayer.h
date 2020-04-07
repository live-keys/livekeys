#ifndef LVWORKSPACELAYER_H
#define LVWORKSPACELAYER_H

#include <QObject>
#include <QQuickItem>
#include <QQmlPropertyMap>

#include "live/lvviewglobal.h"
#include "live/layer.h"
#include "live/keymap.h"

#include "themecontainer.h"
#include "commands.h"
#include "extensions.h"
#include "documentation.h"
#include "startupmodel.h"

class QQuickCloseEvent;

namespace lv{

class Project;
class Workspace;
class ProjectWorkspace;

/// \private
class WorkspaceLayer : public Layer{

    Q_OBJECT
    Q_PROPERTY(QObject* project                 READ project       NOTIFY projectChanged)
    Q_PROPERTY(QObject* panes                   READ panes         NOTIFY panesChanged)
    Q_PROPERTY(lv::Commands* commands           READ commands      CONSTANT)
    Q_PROPERTY(lv::KeyMap* keymap               READ keymap        CONSTANT)
    Q_PROPERTY(lv::ThemeContainer* themes       READ themes        CONSTANT)
    Q_PROPERTY(lv::Documentation* documentation READ documentation CONSTANT)
    Q_PROPERTY(QQmlPropertyMap* extensions      READ extensions    CONSTANT)
    Q_PROPERTY(lv::StartupModel* recents        READ recents       CONSTANT)
    Q_PROPERTY(lv::StartupModel* tutorials      READ tutorials     CONSTANT)
    Q_PROPERTY(lv::StartupModel* samples        READ samples       CONSTANT)

public:
    explicit WorkspaceLayer(QObject *parent = nullptr);
    ~WorkspaceLayer() override;

    void loadView(ViewEngine *engine, QObject *parent) override;
    QObject* nextViewParent() override;
    QObject * viewRoot() override;

    QObject* project() const;
    QObject* panes() const;

    lv::Commands* commands() const;
    lv::KeyMap* keymap() const;
    QQmlPropertyMap* extensions() const;
    lv::ThemeContainer* themes() const;
    lv::Documentation* documentation() const;
    Workspace* workspace() const;
    lv::StartupModel* recents() const;
    lv::StartupModel* tutorials() const;
    lv::StartupModel* samples() const;
public slots:
    QJSValue interceptMenu(QJSValue context);
    QJSValue interceptFile(const QString& path, int mode);
    void addPane(QQuickItem* pane, QQuickWindow* window, const QVariantList& position);
    void removePane(QQuickItem* pane);

    void addWindow(QQuickWindow* window);
    void whenMainWindowClose();

    void whenProjectOpen(const QString& path, ProjectWorkspace* workspace);
    void whenProjectClose();

    QString docsPath() const;

    bool wasRecentsFileFound() const;
    QString pluginsPath() const;

signals:
    void projectChanged();
    void panesChanged();

private:
    void initializePanes(ProjectWorkspace* workspace, QJSValue panes);
    void initializePanesAndWindows(ProjectWorkspace* workspace, QJSValue panesAndWindows);

    void loadConfigurations();
    WorkspaceExtension* loadPackageExtension(const std::string& path, const std::string& component);
    WorkspaceExtension* loadPackageExtension(const Package::Ptr& package, const std::string& component);

    QObject* m_nextViewParent;

    QObject* m_projectEnvironment;
    QObject* m_panes;
    QObject* m_viewRoot;

    Commands* m_commands;
    KeyMap*   m_keymap;
    ThemeContainer* m_themes;

    Project*       m_project;
    ViewEngine*    m_engine;

    Extensions*    m_extensions;
    Workspace*     m_workspace;
    Documentation* m_documentation;

    StartupModel*  m_tutorials;
    StartupModel*  m_samples;
};

inline QObject *WorkspaceLayer::project() const{
    return m_projectEnvironment;
}

inline QObject *WorkspaceLayer::panes() const{
    return m_panes;
}

inline Commands *WorkspaceLayer::commands() const{
    return m_commands;
}

inline KeyMap *WorkspaceLayer::keymap() const{
    return m_keymap;
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

}// namespace

#endif // LVWORKSPACELAYER_H

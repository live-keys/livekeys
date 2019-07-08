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

class QQuickCloseEvent;

namespace lv{

class Project;
class Workspace;
class ProjectWorkspace;

/// \private
class WorkspaceLayer : public Layer{

    Q_OBJECT
    Q_PROPERTY(QObject* project            READ project    NOTIFY projectChanged)
    Q_PROPERTY(QObject* panes              READ panes      NOTIFY panesChanged)
    Q_PROPERTY(lv::Commands* commands      READ commands   CONSTANT)
    Q_PROPERTY(lv::KeyMap* keymap          READ keymap     CONSTANT)
    Q_PROPERTY(lv::ThemeContainer* themes  READ themes     CONSTANT)
    Q_PROPERTY(QQmlPropertyMap* extensions READ extensions CONSTANT)

public:
    explicit WorkspaceLayer(QObject *parent = nullptr);
    ~WorkspaceLayer();

    void loadView(ViewEngine *engine, QObject *parent) Q_DECL_OVERRIDE;
    QObject* nextViewParent() Q_DECL_OVERRIDE;

    QObject* project() const;
    QObject* panes() const;

    lv::Commands* commands() const;
    lv::KeyMap* keymap() const;
    QQmlPropertyMap* extensions() const;
    lv::ThemeContainer* themes() const;

public slots:
    QJSValue interceptMenu(QJSValue context);
    QJSValue interceptFile(const QString& path, int mode);
    void addPane(QQuickItem* pane, QQuickWindow* window, const QVariantList& position);
    void removePane(QQuickItem* pane);

    void addWindow(QQuickWindow* window);
    void whenMainWindowClose();

    void whenProjectOpen(const QString& path, ProjectWorkspace* workspace);
    void whenProjectClose();

signals:
    void projectChanged();
    void panesChanged();

private:
    void initializePanes(ProjectWorkspace* workspace, QJSValue panes);
    void initializePanesAndWindows(ProjectWorkspace* workspace, QJSValue panesAndWindows);

    QObject* m_nextViewParent;

    QObject* m_projectEnvironment;
    QObject* m_panes;

    Commands* m_commands;
    KeyMap*   m_keymap;
    ThemeContainer* m_themes;

    Project*  m_project;

    Extensions* m_extensions;
    Workspace*  m_workspace;
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

}// namespace

#endif // LVWORKSPACELAYER_H

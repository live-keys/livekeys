#ifndef LVPROJECTWORKSPACE_H
#define LVPROJECTWORKSPACE_H

#include <QObject>
#include <QQuickWindow>
#include "lveditorglobal.h"
#include "live/mlnode.h"
#include "live/project.h"
#include "live/qmlpropertywatcher.h"

namespace lv{

class LV_EDITOR_EXPORT ProjectWorkspace : public QObject{

    Q_OBJECT

    class State;

public:
    static ProjectWorkspace* create(Project* project, QObject* parent = nullptr);

    explicit ProjectWorkspace(Project* project, QObject *parent = nullptr);
    ~ProjectWorkspace();

    const QString id() const{ return m_id; }

    void save() const;

    const MLNode& currentLayout() const;

signals:
    void projectActiveChange(ProjectDocument* active);

    void documentOpen(ProjectDocument* document);
    void documentClose(ProjectDocument* document);
    void documentContentsChange(ProjectDocument* document, int from, int to, const QString& addedText);
    void documentMonitorStateChange(ProjectDocument* document, bool isMonitored);
    void documentSave(ProjectDocument* document);

    void windowRectChanged(QWindow* window, const QRect& r);
    void windowVisibilityChanged(QWindow* window, QWindow::Visibility visibility);

    void paneAdded(QQuickItem* pane, QWindow* window, const QVariantList& position);
    void paneSizeChanged(QQuickItem* pane, const QSize& r);
    void paneStateChanged(QQuickItem* pane, const QVariant& state);
    void paneMoved(QQuickItem* pane, QWindow* window, const QVariantList& position);
    void paneRemoved(QQuickItem* pane);

public slots:
    void whenProjectActiveChange(ProjectDocument* document);

    void whenDocumentOpen(ProjectDocument* document);
    void whenDocumentClose(ProjectDocument* document);
    void whenDocumentContentsChanged(int p, int a, int r);
    void whenDocumentIsMonitoredChanged();
    void whenDocumentSaved();

    void whenWindowRectChanged();
    void whenWindowVisibilityChanged();

    void whenPaneInitialized(QQuickItem* pane, QWindow* window);
    void whenPaneAdded(QQuickItem* pane, QWindow* window, const QVariantList& position);
    void whenPaneMoved(QQuickItem* pane, QWindow* window, const QVariantList& position);
    void whenPaneRemoved(QQuickItem* pane);

private:
    void initializeFromId();
    void initializeDefaults();
    void initialize();

    Project*        m_project;
    QString         m_id;
    QQuickWindow*   m_window;
    State*          m_state;
    QString         m_currentWorkspaceId;

    QList<QmlPropertyWatcher*> m_watchers;
};

}// namespace

#endif // PROJECTWORKSPACE_H
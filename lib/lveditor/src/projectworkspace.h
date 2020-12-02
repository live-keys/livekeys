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

    QString projectPath() const;

    void documentWasRemoved(const std::string& path);

signals:
    void projectActiveChange(Runnable* active);

    void documentOpen(Document* document);
    void documentClose(Document* document);
    void documentContentsChange(ProjectDocument* document, int from, int to, const QString& addedText);
    void documentMonitorStateChange(ProjectDocument* document, bool isMonitored);
    void documentSave(ProjectDocument* document);

    void windowOpen(QQuickWindow* window);
    void windowClose(QQuickWindow* window);
    void windowRectChanged(QQuickWindow* window, const QRect& r);
    void windowVisibilityChanged(QQuickWindow* window, QWindow::Visibility visibility);

    void paneAdded(QQuickItem* pane, QWindow* window, const QVariantList& position);
    void paneSizeChanged(QQuickItem* pane, const QSize& r);
    void paneStateChanged(QQuickItem* pane, const QVariant& state);
    void paneRemoved(QQuickItem* pane);

public slots:
    void whenProjectActiveChange(Runnable* runnable);
    void whenAboutToClose();

    void whenDocumentOpen(Document *document);
    void whenDocumentClose(Document* document);
    void whenDocumentContentsChanged(int p, int a, int r);
    void whenDocumentIsMonitoredChanged();
    void whenDocumentSaved();

    void whenWindowInitialized(QQuickWindow* window);
    void whenWindowOpen(QQuickWindow* window);
    void whenWindowClose();
    void whenWindowRectChanged();
    void whenWindowVisibilityChanged();

    void whenPaneInitialized(QQuickItem* pane);
    void whenPaneAdded(QQuickItem* pane, QQuickWindow* window, const QVariantList& position);
    void whenPaneSizeChanged();
    void whenPaneRemoved(QQuickItem* pane);

private:
    void initializeFromId();
    void initializeDefaults();
    void initialize();
    void createLayoutNodes();

    lv::Exception captureContents(const lv::Exception& e);

    Project*        m_project;
    QString         m_id;
    State*          m_state;
    QString         m_currentWorkspaceId;

    QSet<QQuickItem*>          m_panes;
    QList<QmlPropertyWatcher*> m_watchers;
};

}// namespace

#endif // PROJECTWORKSPACE_H

#ifndef QPROJECTQMLSCANNER_H
#define QPROJECTQMLSCANNER_H

#include <QObject>
#include "qdocumentqmlscope.h"
#include "qprojectqmlscope.h"
#include "qlockedfileiosession.h"

class QThread;
class QTimer;

namespace lcv{

class QProjectQmlScanner : public QObject{

    Q_OBJECT

public:
    QProjectQmlScanner(QLockedFileIOSession::Ptr lockedFileIO, QObject* parent = 0);
    ~QProjectQmlScanner();

public:
    void setProjectScope(QProjectQmlScope::Ptr scope);
    void queueDocumentScopeScan(const QString& path, const QString& content, QProjectQmlScope* projectScope);

    QDocumentQmlScope::Ptr lastDocumentScope();

public slots:
    void scanDocumentScope(const QString& path, const QString& content, QProjectQmlScope* projectScope);
    void scanProjectScope();

signals:
    void queueDocumentScan(const QString& path, const QString& content, QProjectQmlScope* projectScope);
    void queueProjectScan();
    void documentScopeReady();
    void projectScopeReady();

private:
    void scanProjectScopeRecurse(int limit = 10);

    QProjectQmlScope::Ptr m_project;
    QDocumentQmlScope::Ptr m_lastDocumentScope;
    QLockedFileIOSession::Ptr m_lockedFileIO;
    QThread* m_thread;
    QTimer*  m_timer;
};

inline QDocumentQmlScope::Ptr QProjectQmlScanner::lastDocumentScope(){
    return m_lastDocumentScope;
}

}// namespace

#endif // QPROJECTQMLSCANNER_H

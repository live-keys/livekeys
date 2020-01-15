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

#ifndef LVPROJECTQMLSCANNER_H
#define LVPROJECTQMLSCANNER_H

#include <QObject>
#include "live/documentqmlscope.h"
#include "live/projectqmlscope.h"
#include "live/lockedfileiosession.h"
#include "projectqmlscanmonitor_p.h"

#include <functional>
#include <QMutex>

class QThread;
class QTimer;

namespace lv{

class CodeQmlHandler;

/// \private
class ProjectQmlScanner : public QObject{

    Q_OBJECT

public:
    /// \private
    class TypeLoadRequest{
    public:
        TypeLoadRequest(const QString& uri, const QString& path)
            : importUri(uri), libraryPath(path), object(nullptr), isError(false)
        {}
        ~TypeLoadRequest(){ delete object; }

        QString importUri;
        QString libraryPath;
        QObject* object;
        bool isError;
    };

public:
    ProjectQmlScanner(
        QQmlEngine* engine,
        QMutex* engineMutex,
        LockedFileIOSession::Ptr lockedFileIO,
        QObject* parent = 0
    );
    ~ProjectQmlScanner();

public:
    void setProjectScope(ProjectQmlScope::Ptr scope);
    void queueDocumentScopeScan(const QString& path, const QString& content, ProjectQmlScope* projectScope, CodeQmlHandler *codeHandler);

    void updateLoadRequest(const QString& uri, QObject* object, bool isError);
    void removeLoadRequest(const QString &path);
    void addLoadRequest(const TypeLoadRequest& request);
    bool requestErrorStatus(const QString& path);
    QObject* requestObject(const QString& path);
    bool hasRequest(const QString& path) const;

    DocumentQmlScope::Ptr lastDocumentScope();

    bool tryToExtractPluginInfo(const QString& path, QByteArray* stream);
    void updatePluginInfo(const QString& libraryPath, const QByteArray& libInfo);

public slots:
    void scanDocumentScope(const QString& path, const QString& content, ProjectQmlScope* projectScope, CodeQmlHandler* codeHandler);
    void scanProjectScope();

signals:
    void queueDocumentScan(const QString& path, const QString& content, ProjectQmlScope* projectScope, CodeQmlHandler* codeHandler);
    void queueProjectScan();
    void documentScopeReady(ProjectQmlScanMonitor::DocumentQmlScopeTransport* dstravel);
    void projectScopeReady();
    void requestObjectLoad(const QString& uri);

private:
    void scanProjectScopeRecurse(int limit = 10);
    void updatePrototypeList();


    ProjectQmlScope::Ptr     m_project;
    DocumentQmlScope::Ptr    m_lastDocumentScope;
    LockedFileIOSession::Ptr m_lockedFileIO;
    QThread*    m_thread;
    QTimer*     m_timer;

    QQmlEngine* m_engine;
    QMutex*     m_engineMutex;

    //TODO: Switch to pointer
    QList<TypeLoadRequest> m_loadRequests;
};

inline DocumentQmlScope::Ptr ProjectQmlScanner::lastDocumentScope(){
    return m_lastDocumentScope;
}

}// namespace

#endif // LVPROJECTQMLSCANNER_H

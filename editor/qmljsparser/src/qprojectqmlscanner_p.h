/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QPROJECTQMLSCANNER_H
#define QPROJECTQMLSCANNER_H

#include <QObject>
#include "qdocumentqmlscope.h"
#include "qprojectqmlscope.h"
#include "live/lockedfileiosession.h"

#include <functional>

class QThread;
class QTimer;

namespace lcv{

class QProjectQmlScanner : public QObject{

    Q_OBJECT

public:
    class TypeLoadRequest{
    public:
        TypeLoadRequest(const QString& uri, const QString& path)
            : importUri(uri), libraryPath(path), object(0), isError(false)
        {}
        ~TypeLoadRequest(){ delete object; }

        QString importUri;
        QString libraryPath;
        QObject* object;
        bool isError;
    };

public:
    QProjectQmlScanner(
        QQmlEngine* engine,
        QMutex* engineMutex,
        LockedFileIOSession::Ptr lockedFileIO,
        QObject* parent = 0
    );
    ~QProjectQmlScanner();

public:
    void setProjectScope(QProjectQmlScope::Ptr scope);
    void queueDocumentScopeScan(const QString& path, const QString& content, QProjectQmlScope* projectScope);

    void updateLoadRequest(const QString& uri, QObject* object, bool isError);
    void removeLoadRequest(const QString &path);
    void addLoadRequest(const TypeLoadRequest& request);
    bool requestErrorStatus(const QString& path);
    QObject* requestObject(const QString& path);
    bool hasRequest(const QString& path) const;

    QDocumentQmlScope::Ptr lastDocumentScope();

    bool tryToExtractPluginInfo(const QString& path, QByteArray* stream);
    void updatePluginInfo(const QString& libraryPath, const QByteArray& libInfo);

public slots:
    void scanDocumentScope(const QString& path, const QString& content, QProjectQmlScope* projectScope);
    void scanProjectScope();

signals:
    void queueDocumentScan(const QString& path, const QString& content, QProjectQmlScope* projectScope);
    void queueProjectScan();
    void documentScopeReady();
    void projectScopeReady();
    void requestObjectLoad(const QString& uri);

private:
    void scanProjectScopeRecurse(int limit = 10);
    void updatePrototypeList();


    QProjectQmlScope::Ptr     m_project;
    QDocumentQmlScope::Ptr    m_lastDocumentScope;
    LockedFileIOSession::Ptr  m_lockedFileIO;
    QThread*    m_thread;
    QTimer*     m_timer;

    QQmlEngine* m_engine;
    QMutex*     m_engineMutex;

    //TODO: Switch to pointer
    QList<TypeLoadRequest> m_loadRequests;
};

inline QDocumentQmlScope::Ptr QProjectQmlScanner::lastDocumentScope(){
    return m_lastDocumentScope;
}

}// namespace

#endif // QPROJECTQMLSCANNER_H

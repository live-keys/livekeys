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

#ifndef LVPROJECTQMLSCANMONITOR_H
#define LVPROJECTQMLSCANMONITOR_H

#include <QObject>
#include <QSet>

#include "live/projectqmlscope.h"
#include "live/qmllanguagescanner.h"
#include "live/workspace.h"

namespace lv{

class ViewEngine;
class Project;
class LanguageQmlHandler;
class ProjectQmlScanner;
class ProjectQmlExtension;

/// \private
class QmlProjectMonitor : public QObject{

    Q_OBJECT

    friend class ProjectQmlExtension;

public:

public:
    explicit QmlProjectMonitor(
        ProjectQmlExtension* projectHandler,
        Project* project,
        ViewEngine*  engine,
        Workspace* workspace,
        QObject* parent = nullptr
    );
    ~QmlProjectMonitor();

    void queueDocumentScan(const QString& path, const QString& content, LanguageQmlHandler* codeHandler);

    void removeScopeListener(LanguageQmlHandler* handler);
    void addScopeListener(LanguageQmlHandler* handler);

    ProjectQmlScope::Ptr projectScope();
    bool hasProjectScope();

signals:
    void requestDocumentScan(const QString& path, const QString& content, LanguageQmlHandler* handler);
    void libraryScanQueueCleared();

public slots:
    void newDocumentScan(QmlLanguageScanner::DocumentTransport* dstransport);

    void newProject(const QString& path);
    void directoryChanged(const QString& path);
    void fileChanged(const QString& path);

private:
    ProjectQmlExtension*  m_projectHandler;
    Project*              m_project;
    ViewEngine*           m_engine;
    QSet<LanguageQmlHandler*> m_scopeListeners;
    ProjectQmlScope::Ptr  m_projectScope;
};

inline ProjectQmlScope::Ptr QmlProjectMonitor::projectScope(){
    return m_projectScope;
}

inline bool QmlProjectMonitor::hasProjectScope(){
    return !m_projectScope.isNull();
}

}// namespace

#endif // LVPROJECTQMLSCANMONITOR_H

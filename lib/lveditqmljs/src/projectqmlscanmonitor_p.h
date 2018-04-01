/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef LVPROJECTQMLSCANMONITOR_H
#define LVPROJECTQMLSCANMONITOR_H

#include <QObject>
#include <QSet>

#include "live/projectqmlscope.h"
#include "live/documentqmlscope.h"

namespace lv{

class Engine;
class Project;
class CodeQmlHandler;
class ProjectQmlScanner;
class ProjectQmlExtension;
class PluginInfoExtractor;
class ProjectQmlScanMonitor : public QObject{

    Q_OBJECT

    friend class ProjectQmlExtension;

public:
    class DocumentQmlScopeTransport{
    public:
        QString path;
        CodeQmlHandler* codeHandler;
        DocumentQmlScope::Ptr documentScope;
    };

public:
    explicit ProjectQmlScanMonitor(
        ProjectQmlExtension* projectHandler,
        Project* project,
        Engine*  engine,
        QObject* parent = 0
    );
    ~ProjectQmlScanMonitor();

    void scanNewDocumentScope(const QString& path, const QString& content, CodeQmlHandler* codeHandler);
    void queueNewDocumentScope(const QString& path, const QString& content, CodeQmlHandler* codeHandler);

    void removeScopeListener(CodeQmlHandler* handler);
    void addScopeListener(CodeQmlHandler* handler);

    PluginInfoExtractor *getPluginInfoExtractor(const QString& import);

    ProjectQmlScope::Ptr projectScope();
    bool hasProjectScope();

signals:

public slots:
    void newDocumentScope(ProjectQmlScanMonitor::DocumentQmlScopeTransport* dstransport);
    void newProjectScope();
    void newProject(const QString& path);
    void directoryChanged(const QString& path);
    void fileChanged(const QString& path);
    void loadImport(const QString& import);

private:
    ProjectQmlExtension*  m_projectHandler;
    ProjectQmlScanner*    m_scanner;
    Project*              m_project;
    Engine*               m_engine;
    QSet<CodeQmlHandler*> m_scopeListeners;
    ProjectQmlScope::Ptr  m_projectScope;
};

inline ProjectQmlScope::Ptr ProjectQmlScanMonitor::projectScope(){
    return m_projectScope;
}

inline bool ProjectQmlScanMonitor::hasProjectScope(){
    return !m_projectScope.isNull();
}

}// namespace

#endif // LVPROJECTQMLSCANMONITOR_H

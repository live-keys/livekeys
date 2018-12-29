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

#include "live/project.h"
#include "live/projectfile.h"
#include "live/projectentry.h"
#include "live/projectfilemodel.h"
#include "live/projectdocument.h"
#include "live/projectnavigationmodel.h"
#include "live/projectdocumentmodel.h"

#include <QFileInfo>
#include <QUrl>
#include <QDebug>

namespace lv{

Project::Project(QObject *parent)
    : QObject(parent)
    , m_fileModel(new ProjectFileModel(this))
    , m_navigationModel(new ProjectNavigationModel(this))
    , m_documentModel(new ProjectDocumentModel(this))
    , m_lockedFileIO(LockedFileIOSession::createInstance())
    , m_active(0)
{
}

Project::~Project(){
    delete m_fileModel;
    delete m_navigationModel;
    delete m_documentModel;
}

void Project::newProject(){
    m_fileModel->createProject();
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        ProjectDocument* document = new ProjectDocument(
            qobject_cast<ProjectFile*>(m_fileModel->root()->child(0)), false, this
        );
        document->resetContent("import QtQuick 2.3\n\nGrid{\n}");
        m_documentModel->openDocument("", document);
        m_active = document;
        m_path   = "";
        emit pathChanged("");
        emit activeChanged(m_active);
    }
}

void Project::openProject(const QString &path){
    QFileInfo pathInfo(path);
    if ( !pathInfo.exists() ){
        qCritical("Path does not exist: %s", qPrintable(path));
        return;
    }
    closeProject();
    QString absolutePath = QFileInfo(path).absoluteFilePath();
    m_fileModel->openProject(absolutePath);

    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        ProjectDocument* document = new ProjectDocument(
            qobject_cast<ProjectFile*>(m_fileModel->root()->child(0)),
            false,
            this
        );
        m_documentModel->openDocument(document->file()->path(), document);
        m_active = document;
        m_path   = absolutePath;
        emit pathChanged(absolutePath);
        emit activeChanged(document);
    } else if ( m_fileModel->root()->childCount() > 0 ){
        m_path = absolutePath;
        emit pathChanged(absolutePath);

        ProjectFile* bestFocus = lookupBestFocus(m_fileModel->root()->child(0));
        if( bestFocus ){
            ProjectDocument* document = new ProjectDocument(
                bestFocus,
                false,
                this
            );
            m_documentModel->openDocument(document->file()->path(), document);
            m_active = document;
            emit activeChanged(document);
        }
    }
}

void Project::openProject(const QUrl &url){
    openProject(url.toLocalFile());
}

QString Project::dir() const{
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile())
        return QFileInfo(m_path).path();
    else
        return m_path;
}

ProjectFile *Project::relocateDocument(const QString &path, const QString& newPath, ProjectDocument* document){
    m_documentModel->relocateDocument(path, newPath, document);
    QString absoluteNewPath = QFileInfo(newPath).absoluteFilePath();
    if (absoluteNewPath.indexOf(m_path) == 0 )
        m_fileModel->rescanEntries();
    return m_fileModel->openFile(newPath);
}

void Project::closeProject(){
    setActive((ProjectDocument*)0);
    m_documentModel->closeDocuments();
    m_fileModel->closeProject();
    m_path = "";
    emit pathChanged("");
}

ProjectDocument *Project::openFile(const QUrl &path, int mode){
    return openFile(path.toLocalFile(), mode);
}

ProjectDocument *Project::openFile(const QString &path, int mode){
    ProjectDocument* document = isOpened(path);
    if (!document){
        return openFile(m_fileModel->openFile(path), mode);
    } else if ( document->isMonitored() && mode == ProjectDocument::Edit ){
        m_documentModel->updateDocumeMonitoring(document, false);
    } else if ( !document->isMonitored() && mode == ProjectDocument::Monitor ){
        document->readContent();
        m_documentModel->updateDocumeMonitoring(document, true);
    }

    return document;
}

ProjectDocument *Project::openFile(ProjectFile *file, int mode){
    if (!file)
        return 0;

    ProjectDocument* document = isOpened(file->path());

    if ( !document && m_active != 0 && m_active->file() == file ){
        document = m_active;
        m_documentModel->openDocument(file->path(), document);
    } else if (!document){
        document = new ProjectDocument(file, (mode == ProjectDocument::Monitor), this);
        m_documentModel->openDocument(file->path(), document);
    } else if ( document->isMonitored() && mode == ProjectDocument::Edit ){
        m_documentModel->updateDocumeMonitoring(document, false);
    } else if ( !document->isMonitored() && mode == ProjectDocument::Monitor ){
        document->readContent();
        m_documentModel->updateDocumeMonitoring(document, true);
    }

    return document;
}

void Project::setActive(ProjectFile* file){
    if (!file)
        return;

    ProjectDocument* document = isOpened(file->path());
    if (!document){
        document = new ProjectDocument(file, false, this);
        m_documentModel->openDocument(file->path(), document);
    }
    setActive(document);
}

bool Project::isDirProject() const{
    return m_fileModel->root()->childCount() > 0 && !m_fileModel->root()->child(0)->isFile();
}

bool Project::isFileInProject(const QUrl &path) const{
    return isFileInProject(path.toLocalFile());
}

bool Project::isFileInProject(const QString &path) const{
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile() )
        return path == m_path;
    else
        return !path.isEmpty() && path.startsWith(m_path);
}

void Project::setActive(const QString &path){
    ProjectDocument* document = isOpened(path);
    if ( !document ){
        document = new ProjectDocument(m_fileModel->openFile(path), false, this);
    }
    if ( document ){
        setActive(document);
    }
}

ProjectFile *Project::lookupBestFocus(ProjectEntry *entry){
    QList<ProjectEntry*> entriesToScan;
    if ( entry->lastCheckTime().isNull() )
        m_fileModel->expandEntry(entry);

    ProjectFile* bestEntry = 0;

    foreach( ProjectEntry* childEntry, entry->entries() ){
        if ( childEntry->isFile() ){
            if ( !childEntry->name().isEmpty() ){
                if ( childEntry->name() == "main.qml" ){
                    return qobject_cast<ProjectFile*>(childEntry);
                } else if ( childEntry->name().endsWith(".qml") ) {
                    if ( !bestEntry ) {
                        bestEntry = qobject_cast<ProjectFile*>(childEntry);
                    } else if ( childEntry->name().at(0).isLower() && !bestEntry->name().at(0).isLower() ){
                        bestEntry = qobject_cast<ProjectFile*>(childEntry);
                    }
                }
            }
        } else {
            entriesToScan.append(childEntry);
        }
    }

    foreach( ProjectEntry* entry, entriesToScan ){
        ProjectFile* file = lookupBestFocus(entry);
        if ( file ){
            if ( file->name() == "main.qml" ){
                return qobject_cast<ProjectFile*>(file);
            } else if ( file->name().endsWith(".qml") ) {
                if ( !bestEntry ) {
                    bestEntry = file;
                } else if ( file->name().at(0).isLower() && !bestEntry->name().at(0).isLower() ){
                    bestEntry = file;
                }
            }
        }
    }

    return bestEntry;
}

ProjectDocument *Project::isOpened(const QString &path){
    return m_documentModel->isOpened(path);
}

void Project::closeFile(const QString &path){
    m_documentModel->closeDocument(path);
}

void Project::setActive(ProjectDocument *document){
    if ( m_active != document ){
        if ( m_active && !m_documentModel->isOpened(m_active->file()->path()) )
            delete m_active;
        m_active = document;
        emit activeChanged(document);
    }
}

QString Project::path(const QString &relative) const{
    return dir() + '/' + relative;
}

}// namespace

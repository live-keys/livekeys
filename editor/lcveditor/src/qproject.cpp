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

#include "qproject.h"
#include "qprojectfile.h"
#include "qprojectentry.h"
#include "qprojectfilemodel.h"
#include "qprojectdocument.h"
#include "qprojectnavigationmodel.h"
#include "qprojectdocumentmodel.h"

#include <QFileInfo>
#include <QUrl>
#include <QDebug>

namespace lcv{

QProject::QProject(QObject *parent)
    : QObject(parent)
    , m_fileModel(new QProjectFileModel(this))
    , m_navigationModel(new QProjectNavigationModel(this))
    , m_documentModel(new QProjectDocumentModel(this))
    , m_lockedFileIO(QLockedFileIOSession::createInstance())
    , m_active(0)
    , m_focus(0)
{
}

QProject::~QProject(){
    delete m_fileModel;
    delete m_navigationModel;
    delete m_documentModel;
}

void QProject::newProject(){
    m_fileModel->createProject();
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        QProjectDocument* document = new QProjectDocument(
            qobject_cast<QProjectFile*>(m_fileModel->root()->child(0)), false, this
        );
        document->dumpContent("import QtQuick 2.3\n\nGrid{\n}");
        m_documentModel->openDocument("", document);
        m_active = document;
        m_focus  = document;
        m_path   = "";
        emit pathChanged("");
        emit activeChanged(m_active);
        emit inFocusChanged(document);
    }
}

void QProject::openProject(const QString &path){
    QFileInfo pathInfo(path);
    if ( !pathInfo.exists() ){
        qCritical("Path does not exist: %s", qPrintable(path));
        return;
    }
    closeProject();
    QString absolutePath = QFileInfo(path).absoluteFilePath();
    m_fileModel->openProject(absolutePath);

    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        QProjectDocument* document = new QProjectDocument(
            qobject_cast<QProjectFile*>(m_fileModel->root()->child(0)),
            false,
            this
        );
        m_documentModel->openDocument(document->file()->path(), document);
        m_active = document;
        m_focus  = document;
        m_path   = absolutePath;
        emit pathChanged(absolutePath);
        emit inFocusChanged(document);
        emit activeChanged(document);
    } else if ( m_fileModel->root()->childCount() > 0 ){

        QProjectFile* bestFocus = lookupBestFocus(m_fileModel->root()->child(0));
        if( bestFocus ){
            QProjectDocument* document = new QProjectDocument(
                bestFocus,
                false,
                this
            );
            m_documentModel->openDocument(document->file()->path(), document);
            m_active = document;
            m_focus = document;
            emit inFocusChanged(document);
            emit activeChanged(document);
        }

        m_path = absolutePath;
        emit pathChanged(absolutePath);
    }
}

void QProject::openProject(const QUrl &url){
    openProject(url.toLocalFile());
}

QString QProject::dir() const{
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile())
        return QFileInfo(m_path).path();
    else
        return m_path;
}

QProjectFile *QProject::relocateDocument(const QString &path, const QString& newPath, QProjectDocument* document){
    m_documentModel->relocateDocument(path, newPath, document);
    QString absoluteNewPath = QFileInfo(newPath).absoluteFilePath();
    if (absoluteNewPath.indexOf(m_path) == 0 )
        m_fileModel->rescanEntries();
    return m_fileModel->openFile(newPath);
}

void QProject::closeProject(){
    setInFocus(0);
    setActive((QProjectDocument*)0);
    m_documentModel->closeDocuments();
    m_fileModel->closeProject();
    m_path = "";
    emit pathChanged("");
}

void QProject::openFile(const QUrl &path, int mode){
    openFile(path.toLocalFile(), mode);
}

void QProject::openFile(const QString &path, int mode){
    QProjectDocument* document = isOpened(path);
    if (!document){
        openFile(m_fileModel->openFile(path), mode);
    } else if ( document->isMonitored() && mode == QProjectDocument::Edit ){
        m_documentModel->updateDocumeMonitoring(document, false);
    } else if ( !document->isMonitored() && mode == QProjectDocument::Monitor ){
        document->readContent();
        m_documentModel->updateDocumeMonitoring(document, true);
    } else
        setInFocus(document);
}

void QProject::openFile(QProjectFile *file, int mode){
    if (!file)
        return;

    QProjectDocument* document = isOpened(file->path());

    if ( !document && m_active != 0 && m_active->file() == file ){
        document = m_active;
        m_documentModel->openDocument(file->path(), document);
    } else if (!document){
        document = new QProjectDocument(file, (mode == QProjectDocument::Monitor), this);
        m_documentModel->openDocument(file->path(), document);
    } else if ( document->isMonitored() && mode == QProjectDocument::Edit ){
        m_documentModel->updateDocumeMonitoring(document, false);
    } else if ( !document->isMonitored() && mode == QProjectDocument::Monitor ){
        document->readContent();
        m_documentModel->updateDocumeMonitoring(document, true);
    } else
        setInFocus(document);
    if(document)
        setInFocus(document);
}

void QProject::setActive(QProjectFile* file){
    if (!file)
        return;

    QProjectDocument* document = isOpened(file->path());
    if (!document){
        document = new QProjectDocument(file, false, this);
        m_documentModel->openDocument(file->path(), document);
    }
    setActive(document);
}

bool QProject::isDirProject() const{
    return m_fileModel->root()->childCount() > 0 && !m_fileModel->root()->child(0)->isFile();
}

bool QProject::isFileInProject(const QUrl &path) const{
    return isFileInProject(path.toLocalFile());
}

bool QProject::isFileInProject(const QString &path) const{
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile() )
        return path == m_path;
    else
        return !path.isEmpty() && path.startsWith(m_path);
}

void QProject::setActive(const QString &path){
    QProjectDocument* document = isOpened(path);
    if ( !document ){
        document = new QProjectDocument(m_fileModel->openFile(path), false, this);
    }
    if ( document ){
        setActive(document);
    }
}

QProjectFile *QProject::lookupBestFocus(QProjectEntry *entry){
    QList<QProjectEntry*> entriesToScan;
    if ( entry->lastCheckTime().isNull() )
        m_fileModel->expandEntry(entry);

    QProjectFile* bestEntry = 0;

    foreach( QProjectEntry* childEntry, entry->entries() ){
        if ( childEntry->isFile() ){
            if ( !childEntry->name().isEmpty() ){
                if ( childEntry->name() == "main.qml" ){
                    return qobject_cast<QProjectFile*>(childEntry);
                } else if ( childEntry->name().endsWith(".qml") ) {
                    if ( !bestEntry ) {
                        bestEntry = qobject_cast<QProjectFile*>(childEntry);
                    } else if ( childEntry->name().at(0).isLower() && !bestEntry->name().at(0).isLower() ){
                        bestEntry = qobject_cast<QProjectFile*>(childEntry);
                    }
                }
            }
        } else {
            entriesToScan.append(childEntry);
        }
    }

    foreach( QProjectEntry* entry, entriesToScan ){
        QProjectFile* file = lookupBestFocus(entry);
        if ( file ){
            if ( file->name() == "main.qml" ){
                return qobject_cast<QProjectFile*>(file);
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

QProjectDocument *QProject::isOpened(const QString &path){
    return m_documentModel->isOpened(path);
}

void QProject::closeFile(const QString &path){
    m_documentModel->closeDocument(path);
}

void QProject::closeFocusedFile(){
    if ( m_focus ){
        m_documentModel->closeDocument(m_focus->file()->path());
    }
}

void QProject::setInFocus(QProjectDocument *document){
    if ( m_focus != document ){
        m_focus = document;
        emit inFocusChanged(document);
    }
}

void QProject::setActive(QProjectDocument *document){
    if ( m_active != document ){
        if ( m_active && !m_documentModel->isOpened(m_active->file()->path()) )
            delete m_active;
        m_active = document;
        emit activeChanged(document);
    }
}

}// namespace

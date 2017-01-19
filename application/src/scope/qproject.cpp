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
#include "qprojectfilemodel.h"
#include "qprojectdocument.h"

#include <QUrl>
#include <QDebug>

namespace lcv{

QProject::QProject(QObject *parent)
    : QObject(parent)
    , m_fileModel(new QProjectFileModel(this))
    , m_focus(0)
    , m_active(0)
{
    newProject();
}

QProject::~QProject(){
}

void QProject::newProject(){
    m_fileModel->createProject();
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        QProjectDocument* document = new QProjectDocument(qobject_cast<QProjectFile*>(m_fileModel->root()->child(0)));
        m_openedFiles[""] = document;
        m_active = document;
        m_focus  = document;
    }
}

void QProject::openProject(const QString &path){
    closeProject();
    m_fileModel->openProject(path);
}

void QProject::closeProject(){
    m_focus = 0;
    m_active = 0;
    emit inFocusChanged(0);
    emit activeChanged(0);
    for( QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin(); it != m_openedFiles.end(); ++it ){
        delete it.value();
    }
    m_openedFiles.clear();
}

void QProject::openFile(const QString &path){
    QProjectDocument* document = 0;
    document = isOpened(path);
    if (!document){
        document = new QProjectDocument(m_fileModel->openFile(path));
        m_openedFiles[path] = document;
    }
    if (document)
        setInFocus(document);
}

void QProject::openFile(QProjectFile *file){
    if (!file)
        return;

    QProjectDocument* document = isOpened(file->path());
    if (!document){
        document = new QProjectDocument(file);
        file->setIsOpen(true);
        m_openedFiles[file->path()] = document;
    }
    if(document)
        setInFocus(document);
}

void QProject::setActive(QProjectFile* file){
    if (!file)
        return;

    QProjectDocument* document = isOpened(file->path());
    if (!document){
        document = new QProjectDocument(file);
        file->setIsOpen(true);
        m_openedFiles[file->path()] = document;
    }
    setActive(document);
}

void QProject::setActive(const QString &path){
    QProjectDocument* document = isOpened(path);
    if ( !document ){
        document = new QProjectDocument(m_fileModel->openFile(path));
    }
    if ( document ){
        setActive(document);
    }
}

QProjectDocument *QProject::isOpened(const QString &path){
    if ( m_openedFiles.contains(path) )
        return m_openedFiles[path];
    return 0;
}

void QProject::closeFocusedFile(){

}

void QProject::openFile(const QUrl& path){
    openFile(path.toLocalFile());
}

void QProject::setInFocus(QProjectDocument *document){
    if ( m_focus != document ){
        m_focus = document;
        emit inFocusChanged(document);
    }
}

void QProject::setActive(QProjectDocument *document){
    if ( m_active != document ){
        if ( m_active != 0 ){
            m_active->file()->setIsActive(false);
        }
        m_active = document;
        document->file()->setIsActive(true);
        emit activeChanged(document);
    }
}

}// namespace

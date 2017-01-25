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

#include "qprojectdocumentmodel.h"
#include "qprojectdocument.h"
#include "qprojectfile.h"
#include "qprojectfilemodel.h"
#include "qproject.h"
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QDebug>

namespace lcv{

QProjectDocumentModel::QProjectDocumentModel(QProject *project)
    : QAbstractListModel(project)
    , m_fileWatcher(0)
{
    m_roles[QProjectDocumentModel::Name]   = "name";
    m_roles[QProjectDocumentModel::Path]   = "path";
    m_roles[QProjectDocumentModel::IsOpen] = "isOpen";
}

QProjectDocumentModel::~QProjectDocumentModel(){
    closeDocuments();
    if ( m_fileWatcher )
        delete m_fileWatcher;
}

int QProjectDocumentModel::rowCount(const QModelIndex &) const{
    return m_openedFiles.size();
}

QVariant QProjectDocumentModel::data(const QModelIndex &index, int role) const{
    if ( index.row() < 0 || index.row() >= m_openedFiles.size() )
        return QVariant();
    QHash<QString, QProjectDocument*>::const_iterator it = m_openedFiles.constBegin() + index.row();

    if ( role == QProjectDocumentModel::Name ){
        return it.value()->file()->name();
    } else if ( role == QProjectDocumentModel::Path ){
        return it.value()->file()->path();
    } else if ( role == QProjectDocumentModel::IsOpen ){
        return true;
    }

    return QVariant();
}

QHash<int, QByteArray> QProjectDocumentModel::roleNames() const{
    return m_roles;
}

void QProjectDocumentModel::openDocument(const QString &path, QProjectDocument *document){
    beginResetModel();
    m_openedFiles[path] = document;
    if ( document->file()->isMonitored() )
        fileWatcher()->addPath(path);
    endResetModel();
}

void QProjectDocumentModel::closeDocuments(){
    QProject* p = qobject_cast<QProject*>(parent());
    if ( p ){
        p->setInFocus(0);
    }

    if ( m_fileWatcher ){
        QObject::disconnect(m_fileWatcher, SIGNAL(fileChanged(QString)),
                            this, SLOT(monitoredFileChanged(QString)));
        m_fileWatcher->deleteLater();
        m_fileWatcher = 0;
    }

    for( QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin(); it != m_openedFiles.end(); ++it ){
        if ( it.value() != p->active() )
            delete it.value();
    }
    m_openedFiles.clear();

}

void QProjectDocumentModel::updateDocumeMonitoring(QProjectDocument *document, bool monitor){
    if ( document->file()->isMonitored() != monitor ){
        if ( monitor ){
            fileWatcher()->addPath(document->file()->path());
            document->file()->setIsMonitored(true);
        } else {
            fileWatcher()->removePath(document->file()->path());
            document->file()->setIsMonitored(false);
        }
    }
}

void QProjectDocumentModel::closeDocumentsInPath(const QString &path, bool closeIfActive){
    if ( path.isEmpty() ){
        closeDocument(path);
        return;
    }

    bool lookupNewFocus = false;
    QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin();
    QProject* p = qobject_cast<QProject*>(parent());
    beginResetModel();
    while ( it != m_openedFiles.end() ){
        if ( it.key().startsWith(path) ){
            QProjectDocument* doc = it.value();
            it = m_openedFiles.erase(it);
            if ( p ){
                if ( p->inFocus() == doc )
                    lookupNewFocus = true;

                if ( p->active() != doc )
                    delete doc;
                else if ( closeIfActive ){
                    p->setActive((QProjectDocument*)0);
                }
            } else {
                delete doc;
            }
        } else {
            ++it;
        }
    }

    if ( p ){
        QProjectFile* newActive = 0;
        if ( p->active() == 0 ){
            if ( p->fileModel()->root()->childCount() > 0 ){
                if ( p->fileModel()->root()->child(0)->isFile() )
                    newActive = qobject_cast<QProjectFile*>(p->fileModel()->root()->child(0));
                else {
                    newActive = p->lookupBestFocus(p->fileModel()->root()->child(0));
                }
            }
        }

        if ( p->active() == 0 )
            p->setActive(newActive);

        if ( lookupNewFocus ){
            if ( m_openedFiles.size() > 0 )
                p->setInFocus(*(m_openedFiles.begin()));
            else {
                p->setInFocus(0);
            }
        }
    }

    endResetModel();
}

void QProjectDocumentModel::closeDocument(const QString &path, bool closeIfActive){
    if ( m_openedFiles.contains(path) ){
        beginResetModel();
        QProjectDocument* document = m_openedFiles.take(path);
        QProject* p = qobject_cast<QProject*>(parent());
        if ( p ){
            bool lookupNewFocus = false;
            if ( p->inFocus() == document )
                lookupNewFocus = true;

            if ( p->active() != document )
                delete document;
            else if ( closeIfActive ){
                p->setActive((QProjectDocument*)0);
            }

            QProjectFile* newActive = 0;
            if ( (p->active() == 0 || (lookupNewFocus && m_openedFiles.isEmpty()))){
                if ( p->fileModel()->root()->childCount() > 0 ){
                    if ( p->fileModel()->root()->child(0)->isFile() )
                        newActive = qobject_cast<QProjectFile*>(p->fileModel()->root()->child(0));
                    else {
                        newActive = p->lookupBestFocus(p->fileModel()->root()->child(0));
                    }
                }
            }

            if ( p->active() == 0 ){
                p->setActive(newActive);
            }

            if ( lookupNewFocus ){
                if ( m_openedFiles.size() > 0 )
                    p->setInFocus(*(m_openedFiles.begin()));
                else
                    p->setInFocus(0);
            }

        } else  {
            delete document;
        }
        endResetModel();
    }
}

void QProjectDocumentModel::rescanDocuments(){
    for( QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin(); it != m_openedFiles.end(); ++it ){
        QDateTime modifiedDate = QFileInfo(it.key()).lastModified();
        if ( modifiedDate > it.value()->lastModified() && !it.value()->file()->isMonitored() )
            emit documentChangedOutside(it.value());
    }
}

void QProjectDocumentModel::monitoredFileChanged(const QString &path){
    QProjectDocument* doc = m_openedFiles[path];
    doc->readContent();
    emit monitoredDocumentChanged(doc);
}

bool QProjectDocumentModel::saveDocuments(){
    bool saved = true;
    for( QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin(); it != m_openedFiles.end(); ++it ){
        if ( it.value()->file()->isDirty() )
            if ( !it.value()->save() )
                saved = false;
    }
    return saved;
}

QStringList QProjectDocumentModel::listUnsavedDocuments(){
    QStringList base;
    for( QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin(); it != m_openedFiles.end(); ++it ){
        if ( it.value()->file()->isDirty() )
            base.append(it.value()->file()->path());
    }
    return base;
}

QStringList QProjectDocumentModel::listUnsavedDocumentsInPath(const QString &path){
    QStringList base;
    for( QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin(); it != m_openedFiles.end(); ++it ){
        if ( it.key().startsWith(path) ){
            if ( it.value()->file()->isDirty() )
                base.append(it.value()->file()->path());
        }
    }
    return base;
}

QFileSystemWatcher *QProjectDocumentModel::fileWatcher(){
    if ( !m_fileWatcher ){
        m_fileWatcher = new QFileSystemWatcher;
        QObject::connect(m_fileWatcher, SIGNAL(fileChanged(QString)),
                         this, SLOT(monitoredFileChanged(QString)));
    }
    return m_fileWatcher;
}


}// namespace


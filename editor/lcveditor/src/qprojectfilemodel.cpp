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

#include "qprojectfilemodel.h"
#include "qprojectdocumentmodel.h"
#include "qprojectentry.h"
#include "qprojectfile.h"
#include "qproject.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

#include <QDebug>

namespace lcv{

QProjectFileModel::QProjectFileModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_root(new QProjectEntry(""))
{
}

QProjectFileModel::~QProjectFileModel(){
    delete m_root;
}

QVariant QProjectFileModel::data(const QModelIndex &index, int role) const{
    if ( index.isValid() && role >= UrlStringRole ){
        QProjectEntry* item = static_cast<QProjectEntry*>(index.internalPointer());
        switch (role) {
        case UrlStringRole:
            return QVariant::fromValue(item);
        default:
            break;
        }
    }
    return QVariant();
}

QModelIndex QProjectFileModel::index(int row, int column, const QModelIndex &parent) const{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    QProjectEntry *parentItem = itemOrRoot(parent);
    QProjectEntry *childItem = parentItem->child(row);
    if (childItem){
        return createIndex(row, column, childItem);
    } else
        return QModelIndex();
}

QModelIndex QProjectFileModel::rootIndex(){
    return createIndex(0, 0, m_root);
}

QModelIndex QProjectFileModel::parent(const QModelIndex &index) const{
    if (!index.isValid())
        return QModelIndex();

    QProjectEntry *childItem = itemOrRoot(index);
    QProjectEntry *parentItem = childItem->parentEntry();

    if (parentItem == m_root)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int QProjectFileModel::rowCount(const QModelIndex &parent) const{
    QProjectEntry *parentItem = itemOrRoot(parent);
    if ( parentItem != m_root ){
        expandEntry(parentItem);
    }
    return parentItem->childCount();
}

int QProjectFileModel::columnCount(const QModelIndex &) const{
    return 1;
}

bool QProjectFileModel::hasChildren(const QModelIndex &parent) const{
    QProjectEntry* parentItem = itemOrRoot(parent);
    return parentItem ? !parentItem->isFile() : false;
}

Qt::ItemFlags QProjectFileModel::flags(const QModelIndex &index) const{
    if ( !index.isValid() )
        return 0;
    return QAbstractItemModel::flags(index);
}

QHash<int, QByteArray> QProjectFileModel::roleNames() const{
    QHash<int, QByteArray> result;;
    result.insert(UrlStringRole, "fileName");
    return result;
}

void QProjectFileModel::createProject(){
    beginResetModel();
    m_root->clearItems();
    m_root->addFileEntry("");
    endResetModel();
}

QProjectFile *QProjectFileModel::openFile(const QString &file){
    QString absolutePath = QFileInfo(file).absoluteFilePath();
    if ( m_root->childCount() == 0 )
        return openExternalFile(absolutePath);
    QProjectEntry* projectEntry   = m_root->child(0);
    QProjectEntry* foundPath = 0;
    if ( absolutePath.indexOf(projectEntry->path()) == 0 ){
        QString pathLeft = absolutePath.mid(projectEntry->path().size());
        pathLeft.prepend(pathLeft.startsWith("/") ? projectEntry->name() : (projectEntry->name() + "/"));
        foundPath = findPathInEntry(projectEntry, pathLeft);
    }

    if ( foundPath && foundPath->isFile() )
        return qobject_cast<QProjectFile*>(foundPath);
    return openExternalFile(absolutePath);
}

QProjectEntry *QProjectFileModel::findPathInEntry(QProjectEntry *entry, const QString &path){
    if ( path.indexOf(entry->name()) == 0 ){
        QString pathLeft = path.mid(entry->name().size());
        if ( pathLeft.startsWith("/") )
            pathLeft = pathLeft.mid(1);
        if ( pathLeft == "" ) {
            return entry;
        } else {
            if ( entry->lastCheckTime().isNull() )
                expandEntry(entry);

            foreach( QObject* obj, entry->children() ){
                QProjectEntry* currentEntry = qobject_cast<QProjectEntry*>(obj);
                if( currentEntry ){
                    QProjectEntry* foundPath = findPathInEntry(currentEntry, pathLeft);
                    if (foundPath)
                        return foundPath;
                }
            }
        }
    }
    return 0;
}

void QProjectFileModel::openProject(const QString &path){
    beginResetModel();
    m_root->clearItems();

    QFileInfo pathInfo(path);
    if ( !pathInfo.exists() ){
        endResetModel();
        emit error("Project path does not exist: " + path);
        return;
    }

    QProjectEntry* project = 0;
    if ( pathInfo.isDir() ){
        project = new QProjectEntry(path, m_root);
        expandEntry(project);
    } else {
        project = new QProjectFile(path, m_root);
    }
    endResetModel();
    emit projectNodeChanged(createIndex(0, 0, project));
}

void QProjectFileModel::closeProject(){
    beginResetModel();
    m_root->clearItems();
    endResetModel();
    emit projectNodeChanged(createIndex(0, 0, m_root));
}

void QProjectFileModel::entryRemoved(const QModelIndex &item){
    QProjectEntry* itemEntry = itemAt(item);
    if ( itemEntry )
        entryRemoved(item, itemEntry);
}

void QProjectFileModel::entryRemoved(QProjectEntry *entry){
    entryRemoved(createIndex(entry->childNumber(), 0, entry), entry);
}

void QProjectFileModel::entryRemoved(const QModelIndex &item, QProjectEntry *entry){
    QProjectEntry* parentEntry = entry->parentEntry();
    beginRemoveRows(parent(item), item.row(), item.row());
    entry->setParentEntry(0);
    endRemoveRows();

    QProject* project = qobject_cast<QProject*>(QObject::parent());
    if ( project && parentEntry )
        emit project->directoryChanged(parentEntry->path());
}

void QProjectFileModel::entryAdded(QProjectEntry *item, QProjectEntry *parent){
    QModelIndex parentIndex = createIndex(parent->childNumber(), 0, parent);
    int insertionIndex = parent->findEntryInsertionIndex(item);
    beginInsertRows(
        parentIndex,
        insertionIndex,
        insertionIndex
    );
    item->setParentEntry(parent);
    endInsertRows();

    QProject* project = qobject_cast<QProject*>(QObject::parent());
    if ( project )
        emit project->directoryChanged(parent->path());
}

void QProjectFileModel::moveEntry(QProjectEntry *item, QProjectEntry *newParent){
    if ( item && newParent ){
        QString newPath = newParent->path() + "/" + item->name();
        if ( item->isFile() ){
            if ( QFile::rename(item->path(), newPath ) ){
                entryRemoved(item);
                entryAdded(item, newParent);
                item->updatePaths();
            } else {
                emit error("Failed to move file: \"" + item->path() + "\" to \"" + newPath + "\"");
            }
        } else {
            if ( !item->parentEntry() ){
                emit error("Failed to move directory: \"" + item->path() + "\" to \"" + newPath + "\"");
            }
            if ( QDir(item->parentEntry()->path()).rename(item->path(), newPath ) ){
                entryRemoved(item);
                entryAdded(item, newParent);
                item->updatePaths();
            } else {
                emit error("Failed to move directory: \"" + item->path() + "\" to \"" + newPath + "\"");
            }
        }
    }
}

void QProjectFileModel::renameEntry(QProjectEntry *item, const QString &newName){
    QProjectEntry* parentEntry = item->parentEntry();
    if ( parentEntry == 0 )
        return;

    if ( item->name() != newName){
        QString newPath = parentEntry->path() + "/" + newName;

        if ( QFileInfo(newPath).exists() ){
            emit error("Failed to rename entry. There is already a path with that name: " + newPath);
            return;
        }

        if ( item->isFile() ){
            QFile f(item->path());
            if ( !f.rename(newPath) ){
                emit error("Failed to rename file to: " + newName);
                return;
            } else {
                item->setName(newName);
                QProject* project = qobject_cast<QProject*>(QObject::parent());
                if ( project )
                    emit project->directoryChanged(parentEntry->path());
            }
        } else {
            if ( parentEntry ){
                QDir d(parentEntry->path());
                if ( !d.rename(item->name(), newPath) ){
                    emit error("Failed to rename directory to:" + newName);
                    return;
                } else {
                    item->setName(newName);
                    item->updatePaths();
                    QProject* project = qobject_cast<QProject*>(QObject::parent());
                    if ( project )
                        emit project->directoryChanged(parentEntry->path());
                }
            }
        }
    }
}

QProjectFile *QProjectFileModel::addFile(QProjectEntry *parentEntry, const QString &name){
    QString filePath = QDir::cleanPath(parentEntry->path() + "/" + name);
    QFile file(filePath);
    if ( file.exists() ){
        emit error("Failed to create file: " + filePath + "\nFile exists.");
        return 0;
    }

    if ( !file.open(QIODevice::WriteOnly) ){
        emit error("Failed to create file: " + parentEntry->path() + "/" + name);
        return 0;
    }
    if ( file.fileName().endsWith(".qml") )
        file.write("import QtQuick 2.3\n\nItem{\n}");
    file.close();

    QProjectFile* fileEntry = new QProjectFile(parentEntry->path(), name, 0);
    entryAdded(fileEntry, parentEntry);
    return fileEntry;
}

QProjectEntry* QProjectFileModel::addDirectory(QProjectEntry *parentEntry, const QString &name){
    QString dirPath = QDir::cleanPath(parentEntry->path() + "/" + name);
    QDir d(parentEntry->path());
    if ( d.exists(name) ){
        emit error("Failed to create directory: " + dirPath + "\nDirectory exists.");
        return 0;
    }
    if ( !d.mkdir(name) ){
        emit error("Failed to create directory: " + dirPath + "");
        return 0;
    }

    QProjectEntry* entry = new QProjectEntry(parentEntry->path(), name);
    entryAdded(entry, parentEntry);
    return entry;
}

bool QProjectFileModel::removeEntry(QProjectEntry *entry){
    QProject* p = qobject_cast<QProject*>(QObject::parent());
    if ( entry->isFile() ){
        if ( QFile(entry->path()).remove() ){
            if ( p )
                p->documentModel()->closeDocument(entry->path(), true);
            entryRemoved(entry);
            return true;
        } else {
            emit error("Failed to remove file: " + entry->path());
            return false;
        }
    } else {
        if ( QDir(entry->path()).removeRecursively() ){
            if ( p )
                p->documentModel()->closeDocumentsInPath(entry->path(), true);
            entryRemoved(entry);
            return true;
        } else {
            emit error("Failed to remove directory: "  + entry->path());
            return false;
        }
    }
    return false;
}

void QProjectFileModel::expandEntry(QProjectEntry *entry) const{
    if ( !entry->lastCheckTime().isNull() )
        return;

    QDirIterator dit(entry->path());
    while( dit.hasNext() ){
        dit.next();
        QFileInfo info = dit.fileInfo();
        if ( info.fileName() == "." || info.fileName() == ".." )
            continue;

        if ( info.isDir() ){
            entry->addEntry(info.fileName());
        } else {
            entry->addFileEntry(info.fileName());
        }
    }

    entry->setLastCheckTime(QDateTime::currentDateTime());
}

void QProjectFileModel::rescanEntries(QProjectEntry *entry){
    if ( entry == 0 ){
        foreach( QProjectEntry* childEntry, m_root->entries() )
            rescanEntries(childEntry);
        return;
    }
    if ( entry->lastCheckTime().isNull() )
        return;

    QProject* project = qobject_cast<QProject*>(QObject::parent());

    QDirIterator dit(entry->path());
    QSet<QString> newEntries;

    bool hasDirectoryChanged = false;
    while( dit.hasNext() ){
        dit.next();
        QFileInfo info = dit.fileInfo();
        if ( info.fileName() == "." || info.fileName() == ".." )
            continue;

        newEntries.insert(info.fileName());
        if ( !entry->contains(info.fileName()) ){
            QProjectEntry* newEntry = info.isDir()
                ? new QProjectEntry(entry->path(), info.fileName(), (QProjectEntry*)0)
                : new QProjectFile(entry->path(), info.fileName(), 0);

            entryAdded(newEntry, entry);
            hasDirectoryChanged = true;
        }
    }

    foreach( QProjectEntry* childEntry, entry->entries() ){
        if ( !newEntries.contains(childEntry->name()) ){
            entryRemoved(childEntry);
            if ( childEntry->isFile() ){
                if ( project )
                    emit project->fileChanged(childEntry->path());
                QProjectFile* entryAsFile = qobject_cast<QProjectFile*>(childEntry);
                if ( !entryAsFile->isOpen() )
                    delete childEntry;
            } else {
                delete childEntry;
            }
            hasDirectoryChanged = true;
        }
    }

    entry->setLastCheckTime(QDateTime::currentDateTime());

    if ( hasDirectoryChanged )
        emit project->directoryChanged(entry->path());

    foreach( QProjectEntry* childEntry, entry->entries() ){
        rescanEntries(childEntry);
    }
}

QProjectEntry* QProjectFileModel::itemAt(const QModelIndex &index) const{
    if (index.isValid()) {
        QProjectEntry *item = static_cast<QProjectEntry*>(index.internalPointer());
        if (item)
            return item;
    }
    return 0;
}

QModelIndex QProjectFileModel::itemIndex(QProjectEntry *entry){
    return createIndex(entry->childNumber(), 0, entry);
}

QProjectFile *QProjectFileModel::openExternalFile(const QString &path){
    return new QProjectFile(path, 0);
}

QProjectEntry *QProjectFileModel::itemOrRoot(const QModelIndex &index) const{
    QProjectEntry* item = itemAt(index);
    return item ? item : m_root;
}

}// namespace

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

#include "projectfilemodel.h"
#include "live/projectdocumentmodel.h"
#include "live/projectentry.h"
#include "live/projectfile.h"
#include "live/project.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

#include <QDebug>

/**
 * \class lv::ProjectFileModel
 * \brief The model of the whole project file system used when opening a project in LiveKeys
 *
 * It's a tree-like structure
 * \ingroup lveditor
 */
namespace lv{

/** Default constructor */
ProjectFileModel::ProjectFileModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_root(new ProjectEntry(""))
{
}


/** Default destructor */
ProjectFileModel::~ProjectFileModel(){
    delete m_root;
}

/** Override of the standard function from QAbstractItemModel */
QVariant ProjectFileModel::data(const QModelIndex &index, int role) const{
    if ( index.isValid() && role >= UrlStringRole ){
        ProjectEntry* item = static_cast<ProjectEntry*>(index.internalPointer());
        switch (role) {
        case UrlStringRole:
            return QVariant::fromValue(item);
        default:
            break;
        }
    }
    return QVariant();
}

/** Override of the standard function from QAbstractItemModel */
QModelIndex ProjectFileModel::index(int row, int column, const QModelIndex &parent) const{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    ProjectEntry *parentItem = itemOrRoot(parent);
    ProjectEntry *childItem = parentItem->child(row);
    if (childItem){
        return createIndex(row, column, childItem);
    } else
        return QModelIndex();
}

QModelIndex ProjectFileModel::rootIndex(){
    return createIndex(0, 0, m_root);
}

QModelIndex ProjectFileModel::parent(const QModelIndex &index) const{
    if (!index.isValid())
        return QModelIndex();

    ProjectEntry *childItem = itemOrRoot(index);
    ProjectEntry *parentItem = childItem->parentEntry();

    if (parentItem == m_root)
        return QModelIndex();

    return createIndex(parentItem->childIndex(), 0, parentItem);
}

int ProjectFileModel::rowCount(const QModelIndex &parent) const{
    ProjectEntry *parentItem = itemOrRoot(parent);
    if ( parentItem != m_root ){
        expandEntry(parentItem);
    }
    return parentItem->childCount();
}

int ProjectFileModel::columnCount(const QModelIndex &) const{
    return 1;
}

bool ProjectFileModel::hasChildren(const QModelIndex &parent) const{
    ProjectEntry* parentItem = itemOrRoot(parent);
    return parentItem ? !parentItem->isFile() : false;
}

Qt::ItemFlags ProjectFileModel::flags(const QModelIndex &index) const{
    if ( !index.isValid() )
        return 0;
    return QAbstractItemModel::flags(index);
}

QHash<int, QByteArray> ProjectFileModel::roleNames() const{
    QHash<int, QByteArray> result;;
    result.insert(UrlStringRole, "fileName");
    return result;
}

void ProjectFileModel::createProject(){
    beginResetModel();
    m_root->clearItems();
    m_root->addFileEntry("");
    endResetModel();
}

ProjectFile *ProjectFileModel::openFile(const QString &file){
    QString absolutePath = QFileInfo(file).absoluteFilePath();
    if ( m_root->childCount() == 0 )
        return openExternalFile(absolutePath);
    ProjectEntry* projectEntry   = m_root->child(0);
    ProjectEntry* foundPath = 0;
    if ( absolutePath.indexOf(projectEntry->path()) == 0 ){
        QString pathLeft = absolutePath.mid(projectEntry->path().size());
        pathLeft.prepend(pathLeft.startsWith("/") ? projectEntry->name() : (projectEntry->name() + "/"));
        foundPath = findPathInEntry(projectEntry, pathLeft);
    }

    if ( foundPath && foundPath->isFile() )
        return qobject_cast<ProjectFile*>(foundPath);
    return openExternalFile(absolutePath);
}

ProjectEntry *ProjectFileModel::findPathInEntry(ProjectEntry *entry, const QString &path){
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
                ProjectEntry* currentEntry = qobject_cast<ProjectEntry*>(obj);
                if( currentEntry ){
                    ProjectEntry* foundPath = findPathInEntry(currentEntry, pathLeft);
                    if (foundPath)
                        return foundPath;
                }
            }
        }
    }
    return 0;
}

void ProjectFileModel::openProject(const QString &path){
    beginResetModel();
    m_root->clearItems();

    QFileInfo pathInfo(path);
    if ( !pathInfo.exists() ){
        endResetModel();
        emit error("Project path does not exist: " + path);
        return;
    }

    ProjectEntry* project = 0;
    if ( pathInfo.isDir() ){
        project = new ProjectEntry(path, m_root);
        expandEntry(project);
    } else {
        project = new ProjectFile(path, m_root);
    }
    endResetModel();
    emit projectNodeChanged(createIndex(0, 0, project));
}

void ProjectFileModel::closeProject(){
    beginResetModel();
    m_root->clearItems();
    endResetModel();
    emit projectNodeChanged(createIndex(0, 0, m_root));
}

void ProjectFileModel::entryRemoved(const QModelIndex &item){
    ProjectEntry* itemEntry = itemAt(item);
    if ( itemEntry )
        entryRemoved(item, itemEntry);
}

void ProjectFileModel::entryRemoved(ProjectEntry *entry){
    entryRemoved(createIndex(entry->childIndex(), 0, entry), entry);
}

void ProjectFileModel::entryRemoved(const QModelIndex &item, ProjectEntry *entry){
    ProjectEntry* parentEntry = entry->parentEntry();
    beginRemoveRows(parent(item), item.row(), item.row());
    entry->setParentEntry(0);
    endRemoveRows();

    Project* project = qobject_cast<Project*>(QObject::parent());
    if ( project && parentEntry )
        emit project->directoryChanged(parentEntry->path());
}

void ProjectFileModel::entryAdded(ProjectEntry *item, ProjectEntry *parent){
    QModelIndex parentIndex = createIndex(parent->childIndex(), 0, parent);
    int insertionIndex = parent->findEntryInsertionIndex(item);
    beginInsertRows(
        parentIndex,
        insertionIndex,
        insertionIndex
    );
    item->setParentEntry(parent);
    endInsertRows();

    Project* project = qobject_cast<Project*>(QObject::parent());
    if ( project )
        emit project->directoryChanged(parent->path());
}

void ProjectFileModel::moveEntry(ProjectEntry *item, ProjectEntry *newParent){
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

void ProjectFileModel::renameEntry(ProjectEntry *item, const QString &newName){
    ProjectEntry* parentEntry = item->parentEntry();
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
                Project* project = qobject_cast<Project*>(QObject::parent());
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
                    Project* project = qobject_cast<Project*>(QObject::parent());
                    if ( project )
                        emit project->directoryChanged(parentEntry->path());
                }
            }
        }
    }
}

ProjectFile *ProjectFileModel::addFile(ProjectEntry *parentEntry, const QString &name){
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

    ProjectFile* fileEntry = new ProjectFile(parentEntry->path(), name, 0);
    entryAdded(fileEntry, parentEntry);
    return fileEntry;
}

ProjectEntry* ProjectFileModel::addDirectory(ProjectEntry *parentEntry, const QString &name){
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

    ProjectEntry* entry = new ProjectEntry(parentEntry->path(), name);
    entryAdded(entry, parentEntry);
    return entry;
}

bool ProjectFileModel::removeEntry(ProjectEntry *entry){
    Project* p = qobject_cast<Project*>(QObject::parent());
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

void ProjectFileModel::expandEntry(ProjectEntry *entry) const{
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

void ProjectFileModel::rescanEntries(ProjectEntry *entry){
    if ( entry == 0 ){
        foreach( ProjectEntry* childEntry, m_root->entries() )
            rescanEntries(childEntry);
        return;
    }
    if ( entry->lastCheckTime().isNull() )
        return;

    Project* project = qobject_cast<Project*>(QObject::parent());

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
            ProjectEntry* newEntry = info.isDir()
                ? new ProjectEntry(entry->path(), info.fileName(), (ProjectEntry*)0)
                : new ProjectFile(entry->path(), info.fileName(), 0);

            entryAdded(newEntry, entry);
            hasDirectoryChanged = true;
        }
    }

    foreach( ProjectEntry* childEntry, entry->entries() ){
        if ( !newEntries.contains(childEntry->name()) ){
            entryRemoved(childEntry);
            if ( childEntry->isFile() ){
                if ( project )
                    emit project->fileChanged(childEntry->path());
                ProjectFile* entryAsFile = qobject_cast<ProjectFile*>(childEntry);
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

    foreach( ProjectEntry* childEntry, entry->entries() ){
        rescanEntries(childEntry);
    }
}

ProjectEntry* ProjectFileModel::itemAt(const QModelIndex &index) const{
    if (index.isValid()) {
        ProjectEntry *item = static_cast<ProjectEntry*>(index.internalPointer());
        if (item)
            return item;
    }
    return 0;
}

QModelIndex ProjectFileModel::itemIndex(ProjectEntry *entry){
    return createIndex(entry->childIndex(), 0, entry);
}

ProjectFile *ProjectFileModel::openExternalFile(const QString &path){
    return new ProjectFile(path, 0);
}

ProjectEntry *ProjectFileModel::itemOrRoot(const QModelIndex &index) const{
    ProjectEntry* item = itemAt(index);
    return item ? item : m_root;
}

}// namespace

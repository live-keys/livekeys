#include "qprojectfilemodel.h"
#include "qprojectentry.h"
#include "qprojectfile.h"

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
//        qDebug() << "Index request:" << row << column << childItem->name() << parentItem->children().size();
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
//    qDebug() << "Row count requested. for " << parent << parentItem->childCount();
    if ( parentItem->childCount() == 0)
        expandEntry(parentItem);
//    qDebug() << parentItem->childCount();
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
    QProjectEntry* project   = m_root->child(0);
    QProjectEntry* foundPath = 0;
    if ( absolutePath.indexOf(project->path()) == 0 ){
        QString pathLeft = absolutePath.mid(0, project->path().size());
        if ( pathLeft.startsWith("/") )
            pathLeft = pathLeft.mid(1);
        foundPath = findPathInEntry(project, absolutePath);
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
        if ( pathLeft == "" )
            return entry;
        else {
            if ( entry->childCount() == 0 )
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
//    emit projectNodeChanged(createIndex(0, 0, m_root));
}

void QProjectFileModel::fileClosed(const QString &path){
}

void QProjectFileModel::fileAdded(const QString &path){

}

void QProjectFileModel::fileRemoved(const QString &path){
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
    beginRemoveRows(parent(item), item.row(), item.row());
    entry->setParent(0);
    endRemoveRows();
}

void QProjectFileModel::entryAdded(QProjectEntry *item, QProjectEntry *parent){
    QModelIndex parentIndex = createIndex(parent->childNumber(), 0, parent);
    beginInsertRows(
        parentIndex,
        parent->childCount(),
        parent->childCount()
    );
    qDebug() << "entry added";
    item->setParent(parent);
    endInsertRows();
}

void QProjectFileModel::moveEntry(QProjectEntry *item, QProjectEntry *newParent){
    if ( item && newParent ){
        if ( item->isFile() ){
            //TODO: Removal actions
            entryRemoved(item);
            entryAdded(item, newParent);

//            qDebug() << "Move file:" << itemEntry->path() << " to parent:" << newParentEntry->path();
//            qDebug() << parent(item);
//            beginRemoveRows(parent(item), item.row(), item.row());
//            qDebug() << "moving...";
//            if ( !QDir().rename(itemEntry->path(), newParentEntry->path() ) ){
//                qDebug() << "failed to move";
//            }
//            endRemoveRows();
        } else {
////            qDebug() << "Move dir:" << itemEntry->path() << " to parent:" << newParentEntry->path();
////            qDebug() << parent(item);
            entryRemoved(item);
            entryAdded(item, newParent);
////            if ( !QDir().rename(itemEntry->path(), newParentEntry->path() ) ){

////            }
        }
    }
}

void QProjectFileModel::expandEntry(QProjectEntry *entry) const{
    //check if entry is already expanded or dirty
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
}

QProjectEntry* QProjectFileModel::itemAt(const QModelIndex &index) const{
    if (index.isValid()) {
        QProjectEntry *item = static_cast<QProjectEntry*>(index.internalPointer());
        if (item)
            return item;
    }
    return 0;
}

QProjectFile *QProjectFileModel::openExternalFile(const QString &path){
    return new QProjectFile(path, 0);
}

QProjectEntry *QProjectFileModel::itemOrRoot(const QModelIndex &index) const{
    QProjectEntry* item = itemAt(index);
    return item ? item : m_root;
}

}// namespace

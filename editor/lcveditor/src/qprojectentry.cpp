#include "qprojectentry.h"
#include "qprojectfile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>

#include <QDebug>

namespace lcv {


QProjectEntry::QProjectEntry(const QString &path, QProjectEntry *parent)
    : QObject(parent)
    , m_isFile(false)
    , m_name(QFileInfo(path).fileName())
    , m_path(path)
{
    if ( parent )
        parent->addChildEntry(this);
}

QProjectEntry::QProjectEntry(const QString &path, const QString &name, QProjectEntry *parent)
    : QObject(parent)
    , m_isFile(false)
    , m_name(name)
    , m_path(QDir::cleanPath((path != "" ? path + QDir::separator() : "") + name))
{
    if ( parent )
        parent->addChildEntry(this);
}

QProjectEntry::QProjectEntry(
        const QString &path,
        const QString &name,
        bool isFile,
        QProjectEntry *parent)
    : QObject(parent)
    , m_isFile(isFile)
    , m_name(name)
    , m_path(QDir::cleanPath((path != "" ? path + QDir::separator() : "") + name))
{
    if ( parent )
        parent->addChildEntry(this);
}

QProjectEntry::~QProjectEntry(){
}

QProjectEntry *QProjectEntry::child(int number){
    return entries().at(number);
}

QProjectEntry* QProjectEntry::addEntry(const QString &name){
    return new QProjectEntry(m_path, name, this);
}

QProjectEntry *QProjectEntry::addFileEntry(const QString &name){
    return new QProjectFile(m_path, name, this);
}

int QProjectEntry::childCount() const{
    return m_entries.size();
}

int QProjectEntry::childNumber() const{
    QProjectEntry* p = parentEntry();
    if (p)
        return p->entries().indexOf(const_cast<QProjectEntry*>(this));
    return 0;
}

void QProjectEntry::clearItems(){
    for( QList<QObject*>::const_iterator it = children().begin(); it != children().end(); ++it ){
        QObject* child = *it;
        QProjectEntry* entry = qobject_cast<QProjectEntry*>(child);

        bool canDelete = true;
        if ( entry && entry->isFile() ){
            QProjectFile* file = qobject_cast<QProjectFile*>(child);
            if ( file->isOpen() )
                canDelete = false;
        }
        entry->setParentEntry(0);
        if ( canDelete )
            child->deleteLater();
    }
    m_entries.clear();
}

void QProjectEntry::setParentEntry(QProjectEntry *pEntry){
    QProjectEntry* currentParentEntry = parentEntry();
    if ( currentParentEntry == pEntry )
        return;

    if ( currentParentEntry )
        currentParentEntry->removeChildEntry(this);
    if ( pEntry ){
        pEntry->addChildEntry(this);
    }
    setParent(pEntry);
}

void QProjectEntry::addChildEntry(QProjectEntry *entry){
    for ( int i = m_entries.size() - 1; i >= 0; --i ){
        QProjectEntry* listEntry = m_entries[i];
        if ( *entry > *listEntry ){
            m_entries.insert(i + 1, entry);
            return;
        }
    }
    m_entries.prepend(entry);
}

int QProjectEntry::findEntryInsertionIndex(QProjectEntry *entry){
    for ( int i = m_entries.size() - 1; i >= 0; --i ){
        QProjectEntry* listEntry = m_entries[i];
        if ( *entry > *listEntry ){
            return i + 1;
        }
    }
    return 0;
}

void QProjectEntry::removeChildEntry(QProjectEntry *entry){
    m_entries.removeOne(entry);
}

bool QProjectEntry::rename(const QString &newName){
    //TODO: Implement rename functionality
    return false;
}


}// namespace

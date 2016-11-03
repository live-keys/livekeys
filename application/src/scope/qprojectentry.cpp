#include "qprojectentry.h"
#include "qprojectfile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>

namespace lcv {


QProjectEntry::QProjectEntry(const QString &path, QProjectEntry *parent)
    : QObject(parent)
    , m_name(QFileInfo(path).fileName())
    , m_path(path)
{
}

QProjectEntry::QProjectEntry(const QString &path, const QString &name, QProjectEntry *parent)
    : QObject(parent)
    , m_name(name)
    , m_path(QDir::cleanPath(path + QDir::separator() + name))
{
}

QProjectEntry::~QProjectEntry(){
}

QProjectEntry *QProjectEntry::child(int number){
    return qobject_cast<QProjectEntry*>(children().at(number));
}

QProjectEntry* QProjectEntry::addEntry(const QString &name){
    return new QProjectEntry(m_path, name, this);
}

QProjectEntry *QProjectEntry::addFileEntry(const QString &name){
    return new QProjectFile(m_path, name, this);
}

int QProjectEntry::childCount() const{
    return children().count();
}

int QProjectEntry::childNumber() const
{
    QProjectEntry* p = parentEntry();
    if (p)
        return p->children().indexOf(const_cast<QProjectEntry*>(this));
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
        child->setParent(0);
        if ( canDelete )
            child->deleteLater();
    }
}

bool QProjectEntry::rename(const QString &newName){
    //TODO: Implement rename functionality
    return false;
}

}// namespace

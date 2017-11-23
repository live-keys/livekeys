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

#include "live/projectentry.h"
#include "live/projectfile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

namespace lv {

ProjectEntry::ProjectEntry(const QString &path, ProjectEntry *parent)
    : QObject(parent)
    , m_isFile(false)
    , m_name(QFileInfo(path).fileName())
    , m_path(path)
{
    if ( parent )
        parent->addChildEntry(this);
}

ProjectEntry::ProjectEntry(const QString &path, const QString &name, ProjectEntry *parent)
    : QObject(parent)
    , m_isFile(false)
    , m_name(name)
    , m_path(QDir::cleanPath((path != "" ? path + QDir::separator() : "") + name))
{
    if ( parent )
        parent->addChildEntry(this);
}

ProjectEntry::ProjectEntry(
        const QString &path,
        const QString &name,
        bool isFile,
        ProjectEntry *parent)
    : QObject(parent)
    , m_isFile(isFile)
    , m_name(name)
    , m_path(QDir::cleanPath((path != "" ? path + QDir::separator() : "") + name))
{
    if ( parent )
        parent->addChildEntry(this);
}

void ProjectEntry::updatePaths(){
    ProjectEntry* parent = parentEntry();
    if ( parent ){
        QString newPath = QDir::cleanPath(parent->path() + QDir::separator() + m_name);
        if ( newPath != m_path ){
            m_path = newPath;
            emit pathChanged();
            if ( !isFile() ){
                foreach( ProjectEntry* entry, m_entries ){
                    entry->updatePaths();
                }
            }
        }
    }
}

QUrl ProjectEntry::pathUrl() const{
    return QUrl::fromLocalFile(m_path);
}

ProjectEntry::~ProjectEntry(){
}

ProjectEntry *ProjectEntry::child(int number){
    return entries().at(number);
}

ProjectEntry* ProjectEntry::addEntry(const QString &name){
    return new ProjectEntry(m_path, name, this);
}

ProjectEntry *ProjectEntry::addFileEntry(const QString &name){
    return new ProjectFile(m_path, name, this);
}

int ProjectEntry::childCount() const{
    return m_entries.size();
}

int ProjectEntry::childNumber() const{
    ProjectEntry* p = parentEntry();
    if (p)
        return p->entries().indexOf(const_cast<ProjectEntry*>(this));
    return 0;
}

void ProjectEntry::clearItems(){
    for( QList<QObject*>::const_iterator it = children().begin(); it != children().end(); ++it ){
        QObject* child = *it;
        ProjectEntry* entry = qobject_cast<ProjectEntry*>(child);

        bool canDelete = true;
        if ( entry && entry->isFile() ){
            ProjectFile* file = qobject_cast<ProjectFile*>(child);
            if ( file->isOpen() )
                canDelete = false;
        }
        entry->setParentEntry(0);
        if ( canDelete )
            child->deleteLater();
    }
    m_entries.clear();
}

void ProjectEntry::setParentEntry(ProjectEntry *pEntry){
    ProjectEntry* currentParentEntry = parentEntry();
    if ( currentParentEntry == pEntry )
        return;

    if ( currentParentEntry )
        currentParentEntry->removeChildEntry(this);
    if ( pEntry ){
        pEntry->addChildEntry(this);
    }
    setParent(pEntry);
}

void ProjectEntry::addChildEntry(ProjectEntry *entry){
    for ( int i = m_entries.size() - 1; i >= 0; --i ){
        ProjectEntry* listEntry = m_entries[i];
        if ( *entry > *listEntry ){
            m_entries.insert(i + 1, entry);
            return;
        }
    }
    m_entries.prepend(entry);
}

int ProjectEntry::findEntryInsertionIndex(ProjectEntry *entry){
    for ( int i = m_entries.size() - 1; i >= 0; --i ){
        ProjectEntry* listEntry = m_entries[i];
        if ( *entry > *listEntry ){
            return i + 1;
        }
    }
    return 0;
}

void ProjectEntry::removeChildEntry(ProjectEntry *entry){
    m_entries.removeOne(entry);
}

void ProjectEntry::setName(const QString &name){
    if (m_name == name)
        return;

    m_name = name;
    m_path = QFileInfo().path() + "/" + name;

    emit nameChanged();
    emit pathChanged();

    if ( !isFile() ){
        foreach( ProjectEntry* entry, m_entries ){
            entry->updatePaths();
        }
    }
}


}// namespace

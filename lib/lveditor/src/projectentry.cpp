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

#include "live/projectentry.h"
#include "live/projectfile.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

/**
 * \class lv::ProjectEntry
 * \brief An entry within a LiveKeys project, either a folder or a file
 *
 * The whole hierarchy is in a tree-like structure
 * \ingroup lveditor
 */
namespace lv {

/** Default constructor */
ProjectEntry::ProjectEntry(const QString &path, ProjectEntry *parent)
    : QObject(parent)
    , m_isFile(false)
    , m_name(QFileInfo(path).fileName())
    , m_path(path)
{
    if ( parent )
        parent->addChildEntry(this);
}

/** Constructor with a name parameter */
ProjectEntry::ProjectEntry(const QString &path, const QString &name, ProjectEntry *parent)
    : QObject(parent)
    , m_isFile(false)
    , m_name(name)
    , m_path(QDir::cleanPath((path != "" ? path + QDir::separator() : "") + name))
{
    if ( parent )
        parent->addChildEntry(this);
}

/** Complex constructor with an indicator that the entry is a file */
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

/**
 * \brief Updates paths of child entries
 *
 * Useful when moving folders in LiveKeys and re-updates the sub-entry paths
 */
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

/** Returns the QUrl of the entry path */
QUrl ProjectEntry::pathUrl() const{
    return QUrl::fromLocalFile(m_path);
}

/** Blank destructor */
ProjectEntry::~ProjectEntry(){
}

/** Return a child at a given index*/
ProjectEntry *ProjectEntry::child(int index){
    return entries().at(index);
}

/** Adds entry with a given name to this entry */
ProjectEntry* ProjectEntry::addEntry(const QString &name){
    return new ProjectEntry(m_path, name, this);
}

/** Adds fuke entry with a given name to this entry */
ProjectEntry *ProjectEntry::addFileEntry(const QString &name){
    return new ProjectFile(m_path, name, this);
}

/** Number of entries */
int ProjectEntry::childCount() const{
    return m_entries.size();
}

/** Returns the position of this entry within its parent */
int ProjectEntry::childIndex() const{
    ProjectEntry* p = parentEntry();
    if (p)
        return p->entries().indexOf(const_cast<ProjectEntry*>(this));
    return 0;
}

/**
 * \brief Resets all the entries for which that's possible to do
 *
 * It's not possible to delete open files!
 */
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

/** Sets the parent entry */
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

/** Adds child entry */
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

/** Find a hypothetical insertion point for a given entry */
int ProjectEntry::findEntryInsertionIndex(ProjectEntry *entry){
    for ( int i = m_entries.size() - 1; i >= 0; --i ){
        ProjectEntry* listEntry = m_entries[i];
        if ( *entry > *listEntry ){
            return i + 1;
        }
    }
    return 0;
}


/** Remove a child entry */
void ProjectEntry::removeChildEntry(ProjectEntry *entry){
    m_entries.removeOne(entry);
}

/**
 * \brief Sets the name to the current entry, simultaneously changing the path as well
 *
 * Useful for e.g. renaming folders
 */
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

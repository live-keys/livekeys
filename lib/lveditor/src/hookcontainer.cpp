/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

#include "hookcontainer.h"
#include "live/visuallogqt.h"

#include <QtDebug>
namespace lv{

HookContainer::HookContainer(const QString &projectPath, Runnable* r, QObject *parent)
    : QObject(parent)
    , m_runnable(r)
    , m_projectPath(projectPath)
{
}

HookContainer::~HookContainer(){
}

// Stored:
void HookContainer::insertKey(const QString &file, const QString &id, QObject *obj){
    auto it = m_entries.find(file);
    if ( it == m_entries.end() ){
        it = m_entries.insert(file, QMap<QString, QList<QObject*> >());
    }

    auto idIt = it.value().find(id);
    if ( idIt == it.value().end() ){
        idIt = it.value().insert(id, QList<QObject*>());
    }

    idIt.value().append(obj);

    emit entryAdded(m_runnable, file, id, obj);
}

void HookContainer::removeEntry(const QString &sourceFile, const QString &id, QObject *obj){
    auto it = m_entries.find(sourceFile);
    if ( it == m_entries.end() ){
        return;
    }

    auto idIt = it.value().find(id);
    if ( idIt == it.value().end() ){
        return;
    }

    idIt.value().removeAll(obj);
    emit entryRemoved(sourceFile, id, obj);
}

QMap<QString, QList<QObject *> > HookContainer::entriesForFile(const QString &sourceFile){
    auto it = m_entries.find(sourceFile);
    if ( it == m_entries.end() )
        return QMap<QString, QList<QObject*> >();
    return it.value();
}

QList<QObject *> HookContainer::entriesFor(const QString &file, const QString &id){
    auto it = m_entries.find(file);
    if ( it == m_entries.end() ){
        return QList<QObject*>();
    }

    auto idIt = it.value().find(id);
    if ( idIt == it.value().end() )
        return QList<QObject*>();

    return idIt.value();
}

}// namespace

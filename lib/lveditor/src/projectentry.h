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

#ifndef LVPROJECTENTRY_H
#define LVPROJECTENTRY_H

#include <QObject>
#include <QDateTime>
#include <QUrl>

#include "live/lveditorglobal.h"

namespace lv{

class LV_EDITOR_EXPORT ProjectEntry : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name   READ name NOTIFY nameChanged)
    Q_PROPERTY(QString path   READ path NOTIFY pathChanged)
    Q_PROPERTY(bool    isFile READ isFile CONSTANT)

public:
    explicit ProjectEntry(const QString& path, ProjectEntry *parent = 0);
    ProjectEntry(const QString& path, const QString& name, ProjectEntry* parent = 0);
    ~ProjectEntry();

    ProjectEntry *child(int number);
    ProjectEntry *parentEntry() const;

    ProjectEntry* addEntry(const QString& name);
    ProjectEntry* addFileEntry(const QString& name);

    int childCount() const;
    int childNumber() const;

    const QString& name() const;
    void setName(const QString& name);

    const QString& path() const;

    bool isFile() const;

    void clearItems();

    bool contains(const QString& name) const;

    bool operator <(const ProjectEntry& other) const;
    bool operator >(const ProjectEntry& other) const;

    void setParentEntry(ProjectEntry* entry);
    void addChildEntry(ProjectEntry* entry);
    int  findEntryInsertionIndex(ProjectEntry* entry);
    void removeChildEntry(ProjectEntry* entry);

    const QDateTime& lastCheckTime() const;
    void setLastCheckTime(const QDateTime& lastCheckTime);

    const QList<ProjectEntry*>& entries() const;

    void updatePaths();

public slots:
    QUrl pathUrl() const;

signals:
    void nameChanged();
    void pathChanged();

protected:
    ProjectEntry(const QString& path, const QString& name, bool isFile, ProjectEntry* parent = 0);

    bool m_isFile;

private:

    QList<ProjectEntry*> m_entries;
    QString m_name;
    QString m_path;
    QDateTime m_lastCheckTime;
};

inline const QString &ProjectEntry::name() const{
    return m_name;
}

inline const QString &ProjectEntry::path() const{
    return m_path;
}

inline bool ProjectEntry::isFile() const{
    return m_isFile;
}

inline bool ProjectEntry::contains(const QString &name) const{
    foreach( ProjectEntry* entry, m_entries ){
        if ( entry->name() == name )
            return true;
    }
    return false;
}

inline bool ProjectEntry::operator <(const ProjectEntry &other) const{
    if ( isFile() && !other.isFile() )
        return false;
    if ( !isFile() && other.isFile() )
        return true;
    return m_name < other.m_name;
}

inline bool ProjectEntry::operator >(const ProjectEntry &other) const{
    if ( isFile() && !other.isFile() )
        return true;
    if ( !isFile() && other.isFile() )
        return false;
    return m_name > other.m_name;
}

inline const QDateTime &ProjectEntry::lastCheckTime() const{
    return m_lastCheckTime;
}

inline void ProjectEntry::setLastCheckTime(const QDateTime &lastCheckTime){
    m_lastCheckTime = lastCheckTime;
}

inline ProjectEntry *ProjectEntry::parentEntry() const{
    if ( parent())
        return qobject_cast<ProjectEntry*>(parent());
    return 0;
}

inline const QList<ProjectEntry *> &ProjectEntry::entries() const{
    return m_entries;
}

}// namespace

#endif // LVPROJECTENTRY_H

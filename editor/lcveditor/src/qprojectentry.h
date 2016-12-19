#ifndef QPROJECTENTRY_H
#define QPROJECTENTRY_H

#include <QObject>
#include <QDateTime>
#include "qlcveditorglobal.h"
#include <QDebug>

namespace lcv{

class Q_LCVEDITOR_EXPORT QProjectEntry : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name   READ name NOTIFY nameChanged)
    Q_PROPERTY(QString path   READ path NOTIFY pathChanged)
    Q_PROPERTY(bool    isFile READ isFile CONSTANT)

public:
    explicit QProjectEntry(const QString& path, QProjectEntry *parent = 0);
    QProjectEntry(const QString& path, const QString& name, QProjectEntry* parent = 0);
    ~QProjectEntry();

    QProjectEntry *child(int number);
    QProjectEntry *parentEntry() const;

    QProjectEntry* addEntry(const QString& name);
    QProjectEntry* addFileEntry(const QString& name);

    int childCount() const;
    int childNumber() const;

    const QString& name() const;
    void setName(const QString& name);

    const QString& path() const;

    bool isFile() const;

    void clearItems();

    bool contains(const QString& name) const;

    bool operator <(const QProjectEntry& other) const;
    bool operator >(const QProjectEntry& other) const;

    void setParentEntry(QProjectEntry* entry);
    void addChildEntry(QProjectEntry* entry);
    int  findEntryInsertionIndex(QProjectEntry* entry);
    void removeChildEntry(QProjectEntry* entry);

    const QDateTime& lastCheckTime() const;
    void setLastCheckTime(const QDateTime& lastCheckTime);

    const QList<QProjectEntry*>& entries() const;

    void updatePaths();

signals:
    void nameChanged();
    void pathChanged();

protected:
    QProjectEntry(const QString& path, const QString& name, bool isFile, QProjectEntry* parent = 0);

    bool m_isFile;

private:

    QList<QProjectEntry*> m_entries;
    QString m_name;
    QString m_path;
    QDateTime m_lastCheckTime;
};

inline const QString &QProjectEntry::name() const{
    return m_name;
}

inline const QString &QProjectEntry::path() const{
    return m_path;
}

inline bool QProjectEntry::isFile() const{
    return m_isFile;
}

inline bool QProjectEntry::contains(const QString &name) const{
    foreach( QProjectEntry* entry, m_entries ){
        if ( entry->name() == name )
            return true;
    }
    return false;
}

inline bool QProjectEntry::operator <(const QProjectEntry &other) const{
    if ( isFile() && !other.isFile() )
        return false;
    if ( !isFile() && other.isFile() )
        return true;
    return m_name < other.m_name;
}

inline bool QProjectEntry::operator >(const QProjectEntry &other) const{
    if ( isFile() && !other.isFile() )
        return true;
    if ( !isFile() && other.isFile() )
        return false;
    return m_name > other.m_name;
}

inline const QDateTime &QProjectEntry::lastCheckTime() const{
    return m_lastCheckTime;
}

inline void QProjectEntry::setLastCheckTime(const QDateTime &lastCheckTime){
    m_lastCheckTime = lastCheckTime;
}

inline QProjectEntry *QProjectEntry::parentEntry() const{
    if ( parent())
        return qobject_cast<QProjectEntry*>(parent());
    return 0;
}

inline const QList<QProjectEntry *> &QProjectEntry::entries() const{
    return m_entries;
}

}// namespace

#endif // QPROJECTENTRY_H

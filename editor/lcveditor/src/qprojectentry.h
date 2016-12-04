#ifndef QPROJECTENTRY_H
#define QPROJECTENTRY_H

#include <QObject>
#include "qlcveditorglobal.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QProjectEntry : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name   READ name NOTIFY nameChanged)
    Q_PROPERTY(QString path   READ path)
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

    virtual bool isFile() const;

    void clearItems();

public slots:
    bool rename(const QString& newName);

signals:
    void nameChanged();

private:
    QString m_name;
    QString m_path;
};

inline const QString &QProjectEntry::name() const{
    return m_name;
}

inline void QProjectEntry::setName(const QString& name){
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}

inline const QString &QProjectEntry::path() const{
    return m_path;
}

inline bool QProjectEntry::isFile() const{
    return false;
}

inline QProjectEntry *QProjectEntry::parentEntry() const{
    if ( parent())
        return qobject_cast<QProjectEntry*>(parent());
    return 0;
}

}// namespace

#endif // QPROJECTENTRY_H

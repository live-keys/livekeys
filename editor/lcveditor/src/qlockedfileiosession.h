#ifndef QLOCKEDFILEIOSESSION_H
#define QLOCKEDFILEIOSESSION_H

#include "qlcveditorglobal.h"

#include <QSharedPointer>
#include <QHash>
#include <QString>

class QReadWriteLock;
class QMutex;

namespace lcv{

class QFileLock;

class Q_LCVEDITOR_EXPORT QLockedFileIOSession{

    Q_DISABLE_COPY(QLockedFileIOSession)

public:
    typedef QSharedPointer<QLockedFileIOSession> Ptr;

public:
    ~QLockedFileIOSession();

    static QLockedFileIOSession::Ptr createInstance();

    QString readFromFile(const QString& path);
    bool writeToFile(const QString& path, const QString& data);

private:
    QLockedFileIOSession();

    QReadWriteLock* getLock(const QString& path);
    void releaseLock(const QString& path);

    QHash<QString, QFileLock*> m_locks;
    QMutex* m_locksMutex;
};

}// namespace

#endif // QLOCKEDFILEIOSESSION_H

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

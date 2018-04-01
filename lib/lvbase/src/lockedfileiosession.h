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

#ifndef LVLOCKEDFILEIOSESSION_H
#define LVLOCKEDFILEIOSESSION_H

#include "live/lvbaseglobal.h"

#include <QSharedPointer>
#include <QHash>
#include <QString>

class QReadWriteLock;
class QMutex;

namespace lv{

class FileLock;

class LV_BASE_EXPORT LockedFileIOSession{

    Q_DISABLE_COPY(LockedFileIOSession)

public:
    typedef QSharedPointer<LockedFileIOSession> Ptr;

public:
    ~LockedFileIOSession();

    static LockedFileIOSession::Ptr createInstance();

    QString readFromFile(const QString& path);
    bool writeToFile(const QString& path, const QString& data);

private:
    LockedFileIOSession();

    QReadWriteLock* getLock(const QString& path);
    void releaseLock(const QString& path);

    QHash<QString, FileLock*> m_locks;
    QMutex* m_locksMutex;
};

}// namespace

#endif // LVLOCKEDFILEIOSESSION_H

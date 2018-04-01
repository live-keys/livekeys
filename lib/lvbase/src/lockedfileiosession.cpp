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

#include "lockedfileiosession.h"
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QReadWriteLock>

namespace lv{

class FileLock{
public:
    FileLock() : refcount(0){}

    QReadWriteLock lock;
    int refcount;
};

LockedFileIOSession::LockedFileIOSession()
    : m_locksMutex(new QMutex)
{
}

QReadWriteLock *LockedFileIOSession::getLock(const QString &path){
    m_locksMutex->lock();
    FileLock* fl = m_locks.value(path, 0);
    if ( !fl ){
        fl = new FileLock;
        m_locks.insert(path, fl);
    }
    fl->refcount++;

    m_locksMutex->unlock();
    return &fl->lock;
}

void LockedFileIOSession::releaseLock(const QString &path){
    m_locksMutex->lock();
    FileLock* fl = m_locks.value(path, 0);
    if ( fl ){
        fl->lock.unlock();
        if ( --(fl->refcount) <= 0 )
            delete m_locks.take(path);
    }
    m_locksMutex->unlock();
}

LockedFileIOSession::~LockedFileIOSession(){
    delete m_locksMutex;
}

LockedFileIOSession::Ptr LockedFileIOSession::createInstance(){
    return LockedFileIOSession::Ptr(new LockedFileIOSession);
}

QString LockedFileIOSession::readFromFile(const QString &path){
    getLock(path)->lockForRead();

    QFile fileInput(path);
    if ( !fileInput.open(QIODevice::ReadOnly | QIODevice::Text) ){
        qCritical("Cannot open file: %s", qPrintable(path));
        releaseLock(path);
        return "";
    }

    QTextStream in(&fileInput);
    QString content = in.readAll();
    fileInput.close();

    releaseLock(path);
    return content;
}

bool LockedFileIOSession::writeToFile(const QString &path, const QString &data){
    getLock(path)->lockForWrite();

    QFile fileInput(path);
    if ( !fileInput.open(QIODevice::WriteOnly ) ){
        qCritical("Can't open file for writing: %s", qPrintable(path));
        releaseLock(path);
        return false;
    }

    QTextStream stream(&fileInput);
    stream << data;
    stream.flush();
    fileInput.close();

    releaseLock(path);
    return true;
}

}// namespace

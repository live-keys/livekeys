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
#include <fstream>
#include <istream>
#include <unordered_map>

#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QReadWriteLock>
#include <QHash>
#include <QString>

/**
  \class lv::LockedFileIOSession
  \ingroup lvbase
  \brief Handles IO from files in a thread-safe manner.
 */

namespace lv{

// Class FileLock
// --------------------------------------------------

/// \private
class FileLock{
public:
    FileLock() : refcount(0){}

    QReadWriteLock lock;
    int refcount;
};

// Class LockedFileIOSessionPrivate
// --------------------------------------------------

/// \private
class LockedFileIOSessionPrivate{

public:
    QReadWriteLock* getLock(const std::string& path);
    void releaseLock(const std::string& path);

    std::unordered_map<std::string, FileLock*> m_locks;
    QMutex* m_locksMutex;
};

QReadWriteLock *LockedFileIOSessionPrivate::getLock(const std::string &path){
    m_locksMutex->lock();

    FileLock* fl = nullptr;

    auto it = m_locks.find(path);
    if ( it == m_locks.end() ){
        fl = new FileLock;
        m_locks[path] = fl;
    } else {
        fl = it->second;
    }

    fl->refcount++;

    m_locksMutex->unlock();
    return &fl->lock;
}

void LockedFileIOSessionPrivate::releaseLock(const std::string &path){
    m_locksMutex->lock();

    auto it = m_locks.find(path);
    if ( it != m_locks.end() ){
        FileLock* fl = it->second;
        fl->lock.unlock();
        if ( --(fl->refcount) <= 0 ){
            m_locks.erase(it);
            delete fl;
        }
    }
    m_locksMutex->unlock();
}

// Class LockedFileIOSession
// --------------------------------------------------

LockedFileIOSession::LockedFileIOSession()
    : m_d(new LockedFileIOSessionPrivate)
{
    m_d->m_locksMutex = new QMutex;
}


LockedFileIOSession::~LockedFileIOSession(){
    delete m_d->m_locksMutex;
    delete m_d;
}

LockedFileIOSession::Ptr LockedFileIOSession::createInstance(){
    return LockedFileIOSession::Ptr(new LockedFileIOSession);
}

std::string LockedFileIOSession::readFromFile(const std::string &path){
    m_d->getLock(path)->lockForRead();

    std::ifstream instream(path, std::ifstream::in | std::ifstream::binary);
    if ( !instream.is_open() ){
        qCritical("Cannot open file: %s", path.c_str());
        m_d->releaseLock(path);
        return "";
    }

    instream.seekg(0, std::ios::end);
    size_t size = instream.tellg();
    std::string buffer(size, ' ');
    instream.seekg(0);
    instream.read(&buffer[0], size);

    m_d->releaseLock(path);
    return buffer;
}

bool LockedFileIOSession::writeToFile(const std::string &path, const std::string &data){
    m_d->getLock(path)->lockForWrite();

    QFile fileInput(path.c_str());
    if ( !fileInput.open(QIODevice::WriteOnly ) ){
        qCritical("Can't open file for writing: %s", path.c_str());
        m_d->releaseLock(path);
        return false;
    }

    fileInput.write(data.c_str(), data.size());
    fileInput.close();

    m_d->releaseLock(path);
    return true;
}

}// namespace

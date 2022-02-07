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

  This class contains a flexible internal representation, in accordance with the D-pointer design pattern.
  Each file (given by its path) gets a unique file lock (the acquisition and release of locks themselves is
  guaranteed to be in critical sections as well), and depending on the context, we get either read or write access.
  There can be multiple readers, but only one writer, at a time.
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

/**
 * \brief Default constructor of LockFileIOSession
 *
 * It initializes the internal mutex that helps us keep lock operations atomic.
 */
LockedFileIOSession::LockedFileIOSession()
    : m_d(new LockedFileIOSessionPrivate)
{
    m_d->m_locksMutex = new QMutex;
}

/**
 * \brief LockedFileIOSession destructor
 *
 * Deletes the internal mutex and the D-pointer itself.
 */
LockedFileIOSession::~LockedFileIOSession(){
    delete m_d->m_locksMutex;
    delete m_d;
}

/**
 * \brief Creates an instance of the LockedFileIOSession
 */
LockedFileIOSession::Ptr LockedFileIOSession::createInstance(){
    return LockedFileIOSession::Ptr(new LockedFileIOSession);
}

/**
 * \brief Locks the file (given by its path) for reading and loads up a buffer with its content.
 *
 * If the file can be opened, the buffer is returned with the entirety of the file's content.
 * If not, an empty string is returned.
 */
std::string LockedFileIOSession::readFromFile(const std::string &path){
    m_d->getLock(path)->lockForRead();

    std::ifstream instream(path, std::ifstream::in | std::ifstream::binary);
    if ( !instream.is_open() ){
        m_d->releaseLock(path);
        qCritical("Cannot open file: %s", path.c_str());
        return "";
    }

    instream.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(instream.tellg());
    std::string buffer(size, ' ');
    instream.seekg(0);
    instream.read(&buffer[0], size);

    m_d->releaseLock(path);
    return buffer;
}

/**
 * \brief Locks the file (given by its path) for writing and writes the given data into it.
 *
 * Write lock means that only the singular writer can access it. If the file exists and it's possible
 * to write in it, the given data is writen in, and a true flag is returned. If not, we return false.
 * The return value is an indicator of a successful write.
 */
bool LockedFileIOSession::writeToFile(const std::string &path, const std::string &data){
    return writeToFile(path, data.c_str(), data.length());
}

bool LockedFileIOSession::writeToFile(const std::string &path, const char *data, size_t length){
    m_d->getLock(path)->lockForWrite();

    QFile fileInput(path.c_str());
    if ( !fileInput.open(QIODevice::WriteOnly ) ){
        m_d->releaseLock(path);
        qCritical("Can't open file for writing: %s", path.c_str());
        return false;
    }

    fileInput.write(data, static_cast<int>(length));
    fileInput.close();

    m_d->releaseLock(path);
    return true;
}

}// namespace

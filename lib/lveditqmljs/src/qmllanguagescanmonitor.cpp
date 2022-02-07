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

#include "qmllanguagescanmonitor.h"
#include "live/visuallogqt.h"

namespace lv{

QmlLanguageScanMonitor::QmlLanguageScanMonitor(QmlLanguageScanner *scanner, QObject *parent)
    : QObject(parent)
    , m_scanner(scanner)
{
    m_scanner->onLibraryUpdate([this](QmlLibraryInfo::Ptr lib, int queueSize){
        m_collectedLibraries.append(QmlLibraryInfo::clone(lib));

        if ( m_collectedLibraries.size() > 5 || queueSize == 0 ){

            m_librariesMutex.lock();
            for ( auto collectedLib : m_collectedLibraries ){
                m_libraries.append(collectedLib);
            }
            m_librariesMutex.unlock();

            m_collectedLibraries.clear();

            emit libraryUpdates();
        }
    });

    m_scanner->onQueueFinished([this](){

        if ( m_collectedLibraries.size() ){

            m_librariesMutex.lock();

            for ( auto collectedLib : m_collectedLibraries )
                m_libraries.append(collectedLib);

            m_librariesMutex.unlock();
            m_collectedLibraries.clear();

            emit libraryUpdates();
        }

        if ( m_scanner->plannedQueueSize() == 0 )
            emit scannerQueueCleared();
    });
}

QmlLanguageScanMonitor::~QmlLanguageScanMonitor(){
    delete m_scanner;
}

void QmlLanguageScanMonitor::queueLibrary(const QmlLibraryInfo::Ptr &lib){
    m_scanner->queueLibrary(lib);
}

QList<QmlLibraryInfo::Ptr> QmlLanguageScanMonitor::detachLibraries(){
    m_librariesMutex.lock();
    QList<QmlLibraryInfo::Ptr> listCopy = m_libraries;
    m_libraries.clear();
    m_librariesMutex.unlock();
    return listCopy;
}

void QmlLanguageScanMonitor::requestStop(){
    m_scanner->requestStop();
}

bool QmlLanguageScanMonitor::isProcessing() const{
    return m_scanner->isProcessing();
}

void QmlLanguageScanMonitor::processQueue(){
    m_scanner->processQueue();
}

} // namespace

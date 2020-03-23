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
            m_libraries = m_collectedLibraries;
            m_librariesMutex.unlock();

            m_collectedLibraries.clear();

            emit libraryUpdates();
        }
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

#include "qplugininfoextractor.h"
#include "qprojectqmlscanner_p.h"
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

namespace lcv{

QPluginInfoExtractor::QPluginInfoExtractor(QProjectQmlScanner *scanner, const QString& path, QObject *parent)
    : QObject(parent)
    , m_scanner(scanner)
    , m_path(path)
    , m_timeout(new QTimer)
    , m_isDone(false)
    , m_timedOut(false)
{
    m_timeout->setSingleShot(true);
    connect(m_timeout, SIGNAL(timeout()), this, SLOT(timeOut()));
}

QPluginInfoExtractor::~QPluginInfoExtractor(){
    delete m_timeout;
}

void QPluginInfoExtractor::waitForResult(int msTimeout){
    m_timeout->start(msTimeout);
    while (!m_timedOut && !m_isDone ){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void QPluginInfoExtractor::newProjectScope(){
    if ( !m_isDone ){
        m_timeout->start();
        m_result.clear();
        m_isDone = m_scanner->tryToExtractPluginInfo(m_path, &m_result);
        if ( m_isDone )
            m_timeout->stop();
    }
}

void QPluginInfoExtractor::timeOut(){
    m_timedOut = true;
}

}// namespace

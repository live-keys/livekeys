#include "qprojectqmlscanner_p.h"
#include <QThread>
#include <QTimer>
#include <QDebug>

namespace lcv{

QProjectQmlScanner::QProjectQmlScanner(QObject *parent)
    : QObject(parent)
    , m_project(0)
    , m_lastDocumentScope(0)
    , m_thread(new QThread)
    , m_timer(new QTimer)
{
    m_timer->setInterval(5000);
    m_timer->setSingleShot(false);

    this->moveToThread(m_thread);
    connect(this, SIGNAL(queueProjectScan()), this, SLOT(scanProjectScope()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(scanProjectScope()));

    connect(
        this, SIGNAL(queueDocumentScan(const QString&,const QString&,QProjectQmlScope*)),
        this, SLOT(scanDocumentScope(const QString&,const QString&,QProjectQmlScope*))
    );

    m_thread->start();
}

QProjectQmlScanner::~QProjectQmlScanner(){
    m_thread->exit();
    if ( m_thread->wait(5000) ){
        qCritical("QmlScanner Thread failed to close, forcing quit. This may lead to inconsistent application state.");
        m_thread->terminate();
        m_thread->wait();
    }
    delete m_thread;
}

void QProjectQmlScanner::setProjectScope(QProjectQmlScope::Ptr scope){
    m_project = scope;
    m_timer->start();
    emit queueProjectScan();
}

void QProjectQmlScanner::queueDocumentScopeScan(
        const QString &path,
        const QString &content,
        QProjectQmlScope* projectScope)
{
    emit queueDocumentScan(path, content, projectScope);
}

void QProjectQmlScanner::scanDocumentScope(
        const QString &path,
        const QString &content,
        QProjectQmlScope *projectScope)
{
    if ( m_project.data() != projectScope )
        return;
    m_lastDocumentScope = QDocumentQmlScope::createScope(path, content, m_project);
    emit documentScopeReady();
}

void QProjectQmlScanner::scanProjectScope(){
    m_project->updateScope();

    //TODO: Only when actually updating the scope
    //Move code here
    emit projectScopeReady();

}

}// namespace

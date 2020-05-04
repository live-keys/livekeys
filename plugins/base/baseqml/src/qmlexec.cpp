#include "qmlexec.h"
#include "live/visuallogqt.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/qmlstream.h"
#include "live/qmlwritablestream.h"

#include <QProcess>
#include <QDebug>

namespace lv{

QmlExec::QmlExec(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess)
    , m_mode(QmlExec::ReadWrite)
    , m_out(new QmlStream(this))
    , m_in(nullptr)
{
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(__processFinished(int, QProcess::ExitStatus)));
}

QmlExec::~QmlExec(){
}

void QmlExec::componentComplete(){
    m_componentComplete = true;
    emit ready();
}

void QmlExec::__processRead(){
    QByteArray data = m_process->readAll();
    m_out->push(qmlEngine(this)->toScriptValue(data));
}

void QmlExec::__processFinished(int exitCode, QProcess::ExitStatus){
    emit finished(exitCode);
}

void QmlExec::setMode(QmlExec::ChannelMode mode){
    if (m_mode == mode)
        return;

    //TODO

    m_mode = mode;
    emit modeChanged();
}

void QmlExec::onInStream(QObject *that, const QJSValue &val){
    QmlExec* ethat = static_cast<QmlExec*>(that);

    ethat->m_process->write(val.toVariant().toByteArray());
}

void QmlExec::setInput(QmlStream *in){
    if (m_in == in)
        return;

    if ( m_in )
        m_in->forward(nullptr, nullptr);

    m_in = in;
    emit inChanged();

    if ( m_in )
        m_in->forward(this, &QmlExec::onInStream);
}

void QmlExec::run(){
    if ( m_process->state() == QProcess::Running ){
        Exception e = CREATE_EXCEPTION(Exception, "Process is already running.", Exception::toCode("~Process"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    if ( m_out ){
        connect(m_process, &QProcess::readyRead, this, &QmlExec::__processRead);
    } else {
        disconnect(m_process, &QProcess::readyRead, this, &QmlExec::__processRead);
    }

    m_process->setProgram(m_path);
    m_process->setArguments(m_args);
    m_process->start();
}

void QmlExec::closeInput(){
    m_process->closeWriteChannel();
}

}// namespace

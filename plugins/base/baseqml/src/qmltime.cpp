#include "qmltime.h"
#include "live/viewengine.h"
#include "live/qmlerror.h"

#include <QTimer>
#include <QQmlEngine>
#include <QThread>

namespace lv{

QmlTime::QmlTime(QObject *parent)
    : QObject(parent)
{
}

QmlTime::~QmlTime(){
}

void QmlTime::sleep(int seconds){
    return QThread::sleep(seconds);
}

void QmlTime::msleep(int mseconds){
    return QThread::msleep(mseconds);
}

void QmlTime::usleep(int useconds){
    return QThread::usleep(useconds);
}

void QmlTime::delay(int mseconds, QJSValue callback){
    ViewEngine* ve = ViewEngine::grabFromQmlEngine(qobject_cast<QQmlEngine*>(parent()));
    if ( !ve )
        QmlError::warnNoEngineCaptured(this);
    QTimer::singleShot(mseconds, [callback, ve]() mutable{
        QJSValue res = callback.call();
        if ( res.isError() ){
            ve->throwError(res);
        }
    });
}

} // namespace

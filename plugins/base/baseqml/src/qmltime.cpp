#include "qmltime.h"

#include <QTimer>
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
    QTimer::singleShot(mseconds, [callback]() mutable{
        callback.call();
    });
}

} // namespace

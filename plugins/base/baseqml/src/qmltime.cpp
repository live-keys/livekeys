#include "qmltime.h"

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

} // namespace

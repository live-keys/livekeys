#include "qmlthreadinfo.h"

#include <QThread>

namespace lv{

QmlThreadInfo::QmlThreadInfo(QObject *parent)
    : QObject(parent)
{
}

QmlThreadInfo::~QmlThreadInfo(){
}

QString QmlThreadInfo::currentThreadIdString() const{
    QThread* ptr = QThread::currentThread();
    QString ptrStr = QString("0x%1").arg((quintptr)ptr, QT_POINTER_SIZE * 2, 16, QChar('0'));
    return ptrStr;
}

QString QmlThreadInfo::objectThreadIdString(QObject *object) const{
    QThread* ptr = object->thread();
    QString ptrStr = QString("0x%1").arg((quintptr)ptr, QT_POINTER_SIZE * 2, 16, QChar('0'));
    return ptrStr;
}

}// namespace

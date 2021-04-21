#include "qmlworkerinterface.h"

namespace lv{

QmlWorkerInterface::QmlWorkerInterface(QObject *parent) : QObject(parent){
}

void QmlWorkerInterface::start(QmlWorkerPool::Task *, int){
}

}//

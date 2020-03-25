#include "qmlwatcher.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QQmlEngine>

#include <QDebug>
#include "live/visuallogqt.h"

namespace lv{

QmlWatcher::QmlWatcher(QObject *parent)
    : QObject(parent)
{
}

QmlWatcher::~QmlWatcher(){
}

void QmlWatcher::componentComplete(){
}

}// namespace

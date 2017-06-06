#include "qenginemonitor.h"
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlEngine>
#include <QVariant>

namespace lcv{

QEngineMonitor::QEngineMonitor(QObject *parent)
    : QObject(parent)
{
}

QEngineMonitor *QEngineMonitor::grabFromContext(QQuickItem *item, const QString &contextProperty){
    return static_cast<QEngineMonitor*>(qmlContext(item)->contextProperty(contextProperty).value<QObject*>());
}

}// namespace

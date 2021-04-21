#include "timelineproperties.h"
#include "live/shared.h"
#include "live/visuallog.h"
#include <QtDebug>

namespace lv{

TimelineProperties::TimelineProperties(QObject *parent)
    : QQmlPropertyMap(parent)
{
}

TimelineProperties::~TimelineProperties(){
}

void TimelineProperties::insertAndCheckValue(const QString &key, const QVariant &input){
    QVariant newVal = updateValue(key, input);
    insert(key, newVal);
}

QVariant TimelineProperties::updateValue(const QString &key, const QVariant &input){
    QVariant val = value(key);
    if ( val.isValid() ){
        QObject* ob = val.value<QObject*>();
        if ( ob ){
            Shared* sob = qobject_cast<Shared*>(ob);
            if ( sob ){
                Shared::unref(sob);
            } else {
                QQmlEngine::setObjectOwnership(ob, QQmlEngine::JavaScriptOwnership);
            }
        }
    }

    if ( input.isValid() ){
        QObject* ob = input.value<QObject*>();
        if ( ob ){
            Shared* sob = qobject_cast<Shared*>(ob);
            if ( sob ){
                Shared::ref(sob);
            } else {
                QQmlEngine::setObjectOwnership(ob, QQmlEngine::CppOwnership);
            }
        }
    }

    return input;
}

}// namespace

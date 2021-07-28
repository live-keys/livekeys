#include "qmlcollector.h"
#include "live/qmlpropertywatcher.h"
#include "live/shared.h"

#include <QQmlProperty>

namespace lv{

QmlCollector::QmlCollector(QObject *parent)
    : QObject(parent)
    , m_properties(new std::list<QmlPropertyWatcher*>)
    , m_propertiesToCollect(new std::list<QString>)
    , m_result(QJSValue::NullValue)
{
}

QmlCollector::~QmlCollector(){
    for ( auto it = m_properties->begin(); it != m_properties->end(); ++it ){
        delete *it;
    }
    delete m_properties;
    delete m_propertiesToCollect;
}

void QmlCollector::propertyChange(const QmlPropertyWatcher &watcher){
    QmlCollector* collector = reinterpret_cast<QmlCollector*>(watcher.object());
    collector->collectProperty(watcher.name(), watcher.read());
}

void QmlCollector::release(){
    m_result = m_collectingResult;
    m_collectingResult = m_engine->engine()->newObject();

    emit resultChanged();
}

void QmlCollector::reset(){
    m_collectingResult = m_engine->engine()->newObject();
    m_propertiesToCollect->clear();

    for ( auto it = m_properties->begin(); it != m_properties->end(); ++it ){
        QmlPropertyWatcher* pw = *it;
        m_propertiesToCollect->push_back(pw->name());
    }
}

void QmlCollector::componentComplete(){
    m_engine = ViewEngine::grab(this);

    const QMetaObject* meta = metaObject();

    for ( int i = 0; i < meta->propertyCount(); i++ ){
        QMetaProperty property = meta->property(i);
        QByteArray name = property.name();
        if ( name != "objectName" &&
             name != "result" )
        {
            QQmlProperty qp(this, name);
            auto pw = new QmlPropertyWatcher(qp);
            pw->onChange(&QmlCollector::propertyChange);
            m_properties->push_back(pw);
        }
    }

    m_collectingResult = m_engine->engine()->newObject();

    emit complete();
}

void QmlCollector::collectProperty(const QString &name, const QVariant &value){
    for ( auto it = m_propertiesToCollect->begin(); it != m_propertiesToCollect->end(); ++it ){
        if ( name == *it ){
            m_collectingResult.setProperty(name, Shared::transfer(value, m_engine->engine()));
            m_propertiesToCollect->erase(it);
            if ( m_propertiesToCollect->empty() ){
                release();
            }
            return;
        }
    }
}

}// namespace

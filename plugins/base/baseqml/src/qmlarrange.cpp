#include "qmlarrange.h"

#include <QJSValueIterator>

namespace lv{

QmlArrange::QmlArrange(QObject *parent)
    : QObject(parent)
    , m_engine(nullptr)
{
}

QmlArrange::~QmlArrange(){
}

void QmlArrange::componentComplete(){
    m_engine = ViewEngine::grab(this);
}

void QmlArrange::run(){
    if ( !m_engine )
        return;

    if ( m_value.isObject() && m_map.isObject() ){
        QJSValueIterator it(m_map);

        QJSValue result = m_engine->engine()->newObject();

        while ( it.hasNext() ){
            it.next();

            if ( m_value.hasOwnProperty(it.name()) ){
                result.setProperty(it.value().toString(), m_value.property(it.name()));
            }
        }

        m_result = result;
        emit resultChanged();

    } else {
        m_result = m_value;
        emit resultChanged();
    }
}

}// namespace

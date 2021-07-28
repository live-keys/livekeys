#include "qmlsplit.h"
#include "qmlcollector.h"

namespace lv{

QmlSplit::QmlSplit(QObject *parent)
    : QObject(parent)
    , m_collector(nullptr)
{

}

void lv::QmlSplit::setValue(QJSValue value){
    m_value = value;

    QmlCollector* collector = qobject_cast<QmlCollector*>(m_collector);
    if ( collector ){
        collector->reset();
    }

    emit valueChanged();
}

}// namespace

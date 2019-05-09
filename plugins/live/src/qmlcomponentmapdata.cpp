#include "qmlcomponentmapdata.h"
#include "qmlcomponentmap.h"

namespace lv{

QmlComponentMapData::QmlComponentMapData(QmlComponentMap *parent)
    : QObject(parent)
    , m_index(0)
{

}

void QmlComponentMapData::setCurrent(int index, const QVariant &value){
    m_index   = index;
    m_current = value;
    emit currentChanged();
}

QmlComponentMap *QmlComponentMapData::mapArray(){
    return qobject_cast<QmlComponentMap*>(parent());
}

int QmlComponentMapData::index() const{
    return m_index;
}

void QmlComponentMapData::result(const QVariant &value){
    QmlComponentMap* p = mapArray();
    p->assignResult(this, value);
}

}// namespace

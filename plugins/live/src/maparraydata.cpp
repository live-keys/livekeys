#include "maparraydata.h"
#include "maparray.h"

namespace lv{

MapArrayData::MapArrayData(MapArray *parent)
    : QObject(parent)
    , m_index(0)
{

}

void MapArrayData::setCurrent(int index, const QVariant &value){
    m_index   = index;
    m_current = value;
    emit currentChanged();
}

MapArray *MapArrayData::mapArray(){
    return qobject_cast<MapArray*>(parent());
}

int MapArrayData::index() const{
    return m_index;
}

void MapArrayData::result(const QVariant &value){
    MapArray* p = mapArray();
    p->assignResult(this, value);
}

}// namespace

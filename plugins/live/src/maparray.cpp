#include "maparray.h"
#include "maparraydata.h"
#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

namespace{

template<class T>
void resizeList(QList<T> & list, int newSize) {
    int diff = newSize - list.size();
    T t;
    if (diff > 0) {
        list.reserve(diff);
        while (diff--) list.append(t);
    } else if (diff < 0) list.erase(list.end() + diff, list.end());
}

}

MapArray::MapArray(QObject *parent)
    : QObject(parent)
    , m_f(nullptr)
    , m_current(new MapArrayData)
{
}

MapArray::~MapArray(){
}

void MapArray::process(){
    if ( !m_input.size() || !m_f){
        return;
    }

    resizeList(m_output, m_input.size());

    m_current->setCurrent(0, m_input[0]);

    QQmlContext* ctx = new QQmlContext(qmlEngine(this), this);
    ctx->setContextProperty("map", m_current);

    QObject* obj = qobject_cast<QObject*>(m_f->create(m_f->creationContext()));
    if ( obj == 0 ){
        qCritical("MapArray: Failed to create item: %s", qPrintable(m_f->errorString()));
        return;
    }
}

void MapArray::assignResult(MapArrayData *mad, const QVariant &v){
    m_output[mad->index()] = v;

    if ( mad->index() + 1 < m_input.size() ){
        m_current->setCurrent(mad->index() + 1, m_input[mad->index() + 1]);
    } else {
        emit outputChanged();
    }
}


} // namespace

#include "qmlobjectlist.h"

namespace lv{

QmlObjectList::QmlObjectList(
        void *data,
        const std::type_info* typeInfo,
        std::function<int (QmlObjectList *)> itemCount,
        std::function<QObject *(QmlObjectList *, int)> itemAt,
        std::function<void (QmlObjectList *, QObject *)> appendItem,
        std::function<void (QmlObjectList *)> clearItems,
        QObject *parent)
    : QObject(parent)
    , m_data(data)
    , m_type(typeInfo)
    , m_itemCount(itemCount)
    , m_itemAt(itemAt)
    , m_appendItem(appendItem)
    , m_clearItems(clearItems)
{
}

QmlObjectList::QmlObjectList(
        void *data,
        const std::type_info *typeInfo,
        std::function<int (QmlObjectList *)> itemCount,
        std::function<QObject *(QmlObjectList *, int)> itemAt,
        QObject *parent)
    : QObject(parent)
    , m_data(data)
    , m_type(typeInfo)
    , m_itemCount(itemCount)
    , m_itemAt(itemAt)
{
}

QmlObjectList::~QmlObjectList(){
    if ( m_clearItems )
        m_clearItems(this);
}

QQmlListProperty<QObject> QmlObjectList::items(){
    if ( !isReadOnly() ){
        return QQmlListProperty<QObject>(this, this,
                 &QmlObjectList::appendItem,
                 &QmlObjectList::itemCount,
                 &QmlObjectList::itemAt,
                 &QmlObjectList::clearItems);
    } else {
        return QQmlListProperty<QObject>(this, this,
                 &QmlObjectList::itemCount,
                 &QmlObjectList::itemAt);
    }
}

void QmlObjectList::appendItem(QQmlListProperty<QObject> *list, QObject *o){
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    ol->m_appendItem(ol, o);
}

int QmlObjectList::itemCount(QQmlListProperty<QObject> *list){
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    return ol->m_itemCount(ol);
}

QObject *QmlObjectList::itemAt(QQmlListProperty<QObject> *list, int i){
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    return ol->m_itemAt(ol, i);
}

void QmlObjectList::clearItems(QQmlListProperty<QObject> *list){
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    ol->m_clearItems(ol);
}


}// namespace

#include "qmlobjectlist.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

namespace lv{

QmlObjectList::QmlObjectList(
        void *data,
        const std::type_info* typeInfo,
        std::function<int (QmlObjectList *)> itemCount,
        std::function<QObject *(QmlObjectList *, int)> itemAt,
        std::function<void (QmlObjectList *, QObject *)> appendItem,
        std::function<void (QmlObjectList *)> clearItems,
        QObject *parent)
    : Shared(parent)
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
    : Shared(parent)
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
    if ( !isConst() ){
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

void QmlObjectList::setClone(std::function<QmlObjectList *(const QmlObjectList *)> clone){
    m_clone = clone;
}

QmlObjectList *QmlObjectList::cloneConst() const{
    if ( !m_clone ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is not clonable.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    }
    QmlObjectList* res = m_clone(this);
    res->m_appendItem = nullptr;
    res->m_clearItems = nullptr;
    return res;
}

QmlObjectList *QmlObjectList::clone() const{
    if ( !m_clone ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is not clonable.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    }
    return m_clone(this);
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

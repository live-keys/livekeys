#include "qmlobjectlist.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "qmlobjectlistmodel.h"
#include <QJSValueIterator>

namespace lv{

QmlObjectList::QmlObjectList(QObject *parent): lv::Shared(parent)
{
    m_data = new QList<QObject*>;
    m_type = &typeid(QList<QObject*>);
    m_itemCount = &defaultItemCount;
    m_itemAt = &defaultItemAt;
    m_appendItem = &defaultAppendItem;
    m_removeItemAt = &defaultRemoveItemAt;
    m_clearItems = &defaultClearItems;
}

QmlObjectList::QmlObjectList(
        void *data,
        const std::type_info* typeInfo,
        std::function<int (QmlObjectList *)> itemCount,
        std::function<QObject *(QmlObjectList *, int)> itemAt,
        std::function<void (QmlObjectList *, QObject *)> appendItem,
        std::function<void(QmlObjectList*, int)> removeItemAt,
        std::function<void (QmlObjectList *)> clearItems,
        QObject *parent)
    : lv::Shared(parent)
    , m_data(data)
    , m_type(typeInfo)
    , m_itemCount(itemCount)
    , m_itemAt(itemAt)
    , m_appendItem(appendItem)
    , m_removeItemAt(removeItemAt)
    , m_clearItems(clearItems)
{
}

QmlObjectList::QmlObjectList(
        void *data,
        const std::type_info *typeInfo,
        std::function<int (QmlObjectList *)> itemCount,
        std::function<QObject *(QmlObjectList *, int)> itemAt,
        QObject *parent)
    : lv::Shared(parent)
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

QObject *QmlObjectList::itemAt(int index)
{
    if (index >= m_itemCount(this))
        return nullptr;
    return m_itemAt(this, index);
}

int QmlObjectList::itemCount()
{
    return m_itemCount(this);
}

void QmlObjectList::clearItems()
{
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_clearItems(this);
}

void QmlObjectList::appendItem(QObject* item)
{
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_appendItem(this,item);
}

void QmlObjectList::removeItemAt(int index)
{
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_removeItemAt(this, index);
}

QmlObjectListModel* QmlObjectList::model()
{
    return new QmlObjectListModel(this);
}

void QmlObjectList::appendItem(QQmlListProperty<QObject> *list, QObject *o){
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    ol->m_appendItem(ol, o);
}

int QmlObjectList::itemCount(QQmlListProperty<QObject> *list){
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    return ol->m_itemCount(ol);
}

void QmlObjectList::removeItemAt(QQmlListProperty<QObject> *list, int index)
{
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    ol->m_removeItemAt(ol, index);
}

QObject *QmlObjectList::itemAt(QQmlListProperty<QObject> *list, int i){
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    return ol->m_itemAt(ol, i);
}

void QmlObjectList::clearItems(QQmlListProperty<QObject> *list){
    QmlObjectList* ol = reinterpret_cast<QmlObjectList*>(list->data);
    ol->m_clearItems(ol);
}

void QmlObjectList::defaultAppendItem(QmlObjectList *list, QObject *item)
{
    auto data = list->dataAs<QList<QObject*>>();
    data->push_back(item);
}

int QmlObjectList::defaultItemCount(QmlObjectList *list)
{
    auto data = list->dataAs<QList<QObject*>>();
    return data->count();
}

QObject *QmlObjectList::defaultItemAt(QmlObjectList *list, int index)
{
    auto data = list->dataAs<QList<QObject*>>();
    if (index >= data->size() || index < 0) return nullptr;
    return data->at(index);
}

void QmlObjectList::defaultRemoveItemAt(QmlObjectList *list, int index)
{
    auto data = list->dataAs<QList<QObject*>>();
    if (index >= data->size() || index < 0) return;
    data->removeAt(index);
}

void QmlObjectList::defaultClearItems(QmlObjectList *list)
{
    auto data = list->dataAs<QList<QObject*>>();
    data->clear();
}


}// namespace

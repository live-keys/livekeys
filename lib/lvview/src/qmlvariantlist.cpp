#include "qmlvariantlist.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "qmlvariantlistmodel.h"

namespace lv{

QmlVariantList::QmlVariantList(QObject *parent): Shared(parent)
{
    m_data = new QVariantList;
    m_type = &typeid(QVariantList);
    m_itemList = &defaultItemList;
    m_itemAt = &defaultItemAt;
    m_itemCount = &defaultItemCount;
    m_appendItem = &defaultAppendItem;
    m_removeItemAt = &defaultRemoveItemAt;
    m_quickAssign = &defaultQuickAssign;
}

QmlVariantList::QmlVariantList(
        void *data,
        const std::type_info* typeInfo,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int (QmlVariantList *)> itemCount,
        std::function<QVariant(QmlVariantList *, int)> itemAt,
        std::function<void (QmlVariantList *, QVariant)> appendItem,
        std::function<void(QmlVariantList*, int)> removeItemAt,
        std::function<void (QmlVariantList *)> clearItems,
        QObject *parent)
    : Shared(parent)
    , m_data(data)
    , m_type(typeInfo)
    , m_itemList(itemList)
    , m_itemCount(itemCount)
    , m_itemAt(itemAt)
    , m_appendItem(appendItem)
    , m_removeItemAt(removeItemAt)
    , m_clearItems(clearItems)
{
}

QmlVariantList::QmlVariantList(
        void *data,
        const std::type_info *typeInfo,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int (QmlVariantList *)> itemCount,
        std::function<QVariant(QmlVariantList *, int)> itemAt,
        QObject *parent)
    : Shared(parent)
    , m_data(data)
    , m_type(typeInfo)
    , m_itemList(itemList)
    , m_itemCount(itemCount)
    , m_itemAt(itemAt)
{
}

QmlVariantList::~QmlVariantList(){
    if ( m_clearItems )
        m_clearItems(this);
}

void QmlVariantList::setItems(const QVariantList &items){
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }

    if ( m_quickAssign ){
        m_quickAssign(this, items);
    } else {
        clearItems();
        for ( auto it = items.begin(); it != items.end(); ++it )
            appendItem(*it);
    }
}

void QmlVariantList::setClone(std::function<QmlVariantList*(const QmlVariantList *)> clone){
    m_clone = clone;
}

void QmlVariantList::setQuickAssign(std::function<void (QmlVariantList *, QVariantList)> qa){
    m_quickAssign = qa;
}

QVariantList QmlVariantList::items(){
    return m_itemList(this);
}

QVariant QmlVariantList::itemAt(int index){
    return m_itemAt(this, index);
}

int QmlVariantList::itemCount(){
    return m_itemCount(this);
}

QmlVariantList* QmlVariantList::cloneConst() const{
    if ( !m_clone ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is not clonable.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    }
    QmlVariantList* res = m_clone(this);
    res->m_appendItem = nullptr;
    res->m_clearItems = nullptr;
    return res;
}

QmlVariantList *QmlVariantList::clone() const{
    if ( !m_clone ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is not clonable.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    }
    return m_clone(this);
}

void QmlVariantList::clearItems(){
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_clearItems(this);
}

void QmlVariantList::appendItem(QVariant item){
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_appendItem(this, item);
}

void QmlVariantList::removeItemAt(int index)
{
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_removeItemAt(this, index);
}

QmlVariantListModel *QmlVariantList::model()
{
    return new QmlVariantListModel(this);
}

QVariantList QmlVariantList::defaultItemList(QmlVariantList *list)
{
    QVariantList* data = list->dataAs<QVariantList>();
    return (*data);
}

int QmlVariantList::defaultItemCount(QmlVariantList *list)
{
    QVariantList* data = list->dataAs<QVariantList>();
    return data->size();
}

QVariant QmlVariantList::defaultItemAt(QmlVariantList *list, int index)
{
    QVariantList* data = list->dataAs<QVariantList>();
    if (index >= data->size() || index < 0) return QVariant();
    return data->at(index);
}

void QmlVariantList::defaultAppendItem(QmlVariantList *list , QVariant item)
{
    QVariantList* data = list->dataAs<QVariantList>();
    data->append(item);
}

void QmlVariantList::defaultRemoveItemAt(QmlVariantList *list, int index)
{
    QVariantList* data = list->dataAs<QVariantList>();
    if (index >= data->size() || index < 0) return;

}

void QmlVariantList::defaultClearItems(QmlVariantList *list)
{
    QVariantList* data = list->dataAs<QVariantList>();
    data->clear();
}

void QmlVariantList::defaultQuickAssign(QmlVariantList *qmllist, QVariantList vlist)
{
    QVariantList* data = qmllist->dataAs<QVariantList>();
    for (auto it = vlist.begin(); it != vlist.end(); ++it){
        QVariant v = *it;
        data->append(v);
    }
}

}// namespace

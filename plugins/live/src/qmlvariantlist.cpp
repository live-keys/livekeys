#include "qmlvariantlist.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/viewengine.h"

namespace lv{

QmlVariantList::QmlVariantList(
        void *data,
        const std::type_info* typeInfo,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int (QmlVariantList *)> itemCount,
        std::function<QVariant(QmlVariantList *, int)> itemAt,
        std::function<void (QmlVariantList *, QVariant)> appendItem,
        std::function<void (QmlVariantList *)> clearItems,
        QObject *parent)
    : QObject(parent)
    , m_data(data)
    , m_type(typeInfo)
    , m_itemList(itemList)
    , m_itemCount(itemCount)
    , m_itemAt(itemAt)
    , m_appendItem(appendItem)
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
    : QObject(parent)
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
    if ( isReadOnly() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "QmlVariantList is read only.", 0);
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

void QmlVariantList::clearItems(){
    if ( isReadOnly() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "QmlVariantList is read only.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_clearItems(this);
}

void QmlVariantList::appendItem(QVariant item){
    if ( isReadOnly() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "QmlVariantList is read only.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_appendItem(this, item);
}

}// namespace

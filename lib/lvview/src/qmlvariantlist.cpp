#include "qmlvariantlist.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "qmlvariantlistmodel.h"

namespace lv{

/**
 * \class lv::QmlVariantList
 *
 * \brief Contains a custom list of values with a user buffer
 *
 * This class is a wrapper for a variety of value containers to which we pass
 * the usual methods to manipulate the data inside. It's to be used universally throughout
 * LiveKeys as a container since it offers greater flexibility than usual Qml arrays.
 * They can also be modifiable or read-only, depending on whether we provide modifier methods.
 * \ingroup lvview
 */

/**
 * \fn lv::QmlVariantList::data
 * \brief Returns the pointer to the data structure
 */

/**
 * \fn lv::QmlVariantList::canCast
 * \brief Shows if the underlying type is matching the one we're passing
*/

/**
 * \fn lv::QmlVariantList::dataAs
 * \brief Returns the data pointer cast to the type we provide
 *
 * Should be used simultaneously with a `canCast` check to avoid errors
 */

/**
 * \fn lv::QmlVariantList::isConst
 * \brief Checks if the list is non-modifiable
 */

/**
 * \fn lv::QmlVariantList::create(void *data, std::function< QVariantList(QmlVariantList *)> itemList, std::function< int(QmlVariantList *)> itemCount, std::function< QVariant(QmlVariantList *, int)> itemAt, std::function< void(QmlVariantList *, QVariant)> appendItem, std::function< void(QmlVariantList *, int)> removeItemAt, std::function< void(QmlVariantList *)> clearItems, QObject *parent=nullptr)
 * \brief Create a modifiable list
 */

/**
 * \fn lv::QmlVariantList::create(void* data, std::function<QVariantList(QmlVariantList*)> itemList, std::function<int(QmlVariantList*)> itemCount, std::function<QVariant(QmlVariantList*, int)> itemAt, QObject *parent = nullptr);
 * \brief Creates a read-only list
 */

/**
 * \fn lv::QmlVariantList::defaultItemCount
 * \brief Default implementation of function that returns the number of items
 */

/**
 * \fn lv::QmlVariantList::defaultItemList
 * \brief Default implementation of function that lists contained items
 */

/**
 * \fn lv::QmlVariantList::defaultItemAt
 * \brief Default implementation of function that returns the item at the given index
 */

/**
 * \fn lv::QmlVariantList::defaultAppendItem
 * \brief Default implementation of function that appends the item to the list
 */

/**
 * \fn lv::QmlVariantList::defaultRemoveItemAt
 * \brief Default implementation of function that removes the item at the given index
 */

/**
 * \fn lv::QmlVariantList::defaultClearItems
 * \brief Default implementation of function that clears the list
 */

/**
 * \fn lv::QmlVariantList::defaultQuickAssign
 * \brief Default implementation of function that does a quick assign
 */

/**
 * \brief Default contructor
 *
 * The default QmlVariantList is actually a wrapped QVariantList.
 */

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

/**
 * \brief Constructor where we provide every wrapper method for the given data container
 */
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

/**
 * \brief Constructor for a read-only container
 *
 * For this type of container, we don't have to provide the modifier methods
 */
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

/**
 * \brief Default destructor
 *
 * If a `clear` method is provided, we clear the container items as well
 */
QmlVariantList::~QmlVariantList(){
    if ( m_clearItems )
        m_clearItems(this);
}


/**
 * \brief Sets items for a modifiable list
 */
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

/**
 * \brief Sets a `clone` function for the list
 */
void QmlVariantList::setClone(std::function<QmlVariantList*(const QmlVariantList *)> clone){
    m_clone = clone;
}

/**
 * \brief Sets a quick assign function for the list
 */
void QmlVariantList::setQuickAssign(std::function<void (QmlVariantList *, QVariantList)> qa){
    m_quickAssign = qa;
}

/**
 * \brief Returns the items this list contains
 */
QVariantList QmlVariantList::items(){
    return m_itemList(this);
}

/**
 * \brief Returns the item at the given index
 */
QVariant QmlVariantList::itemAt(int index){
    return m_itemAt(this, index);
}

/**
 * \brief Returns the item count
 */
int QmlVariantList::itemCount(){
    return m_itemCount(this);
}

/**
 * \brief Creates a const clone of our modifiable list
 *
 * If there's no `clone` function, this will throw an exception
 */
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

/**
 * \brief Clone a modifiable list
 *
 * If there's no `clone` function, this will throw an exception
 */
QmlVariantList *QmlVariantList::clone() const{
    if ( !m_clone ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is not clonable.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    }
    return m_clone(this);
}

/**
 * \brief Clear a modifiable list
 *
 * If a list is read-only, this will throw an exception
 */
void QmlVariantList::clearItems(){
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_clearItems(this);
}

/**
 * \brief Append item to modifiable list
 *
 * If a list is read-only, this will throw an exception
 */
void QmlVariantList::appendItem(QVariant item){
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_appendItem(this, item);
}

/**
 * \brief Remove an item from a modifiable list at the given index
 *
 * If a list is read-only, this will throw an exception
 */
void QmlVariantList::removeItemAt(int index)
{
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_removeItemAt(this, index);
}

/**
 * \brief Return a data model for this list
 */
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

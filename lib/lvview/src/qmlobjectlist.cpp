#include "qmlobjectlist.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "qmlobjectlistmodel.h"
#include <QJSValueIterator>

namespace lv{

/**
 * \class lv::QmlObjectList
 *
 * \brief Contains a list of objects with a user buffer
 *
 * This class is a wrapper for a variety of object containers to which we pass
 * the usual methods to manipulate the data inside. It's to be used universally throughout
 * LiveKeys as a container since it offers greater flexibility than usual Qml arrays.
 * They can also be modifiable or read-only, depending on whether we provide modifier methods.
 * \ingroup lvview
 */

/**
 * \fn lv::QmlObjectList::data
 * \brief Returns the pointer to the data structure
 */

/**
 * \fn lv::QmlObjectList::canCast
 * \brief Shows if the underlying type is matching the one we're passing
*/

/**
 * \fn lv::QmlObjectList::dataAs
 * \brief Returns the data pointer cast to the type we provide
 *
 * Should be used simultaneously with a `canCast` check to avoid errors
 */

/**
 * \fn lv::QmlObjectList::populateObjectList
 * \brief Sets the parameters for an existing object list
 *
 * We pass the data and the associated data modification functions
 */

/**
 * \fn lv::QmlObjectList::isConst
 * \brief Checks if the list is non-modifiable
 */

/**
 * \fn lv::QmlObjectList::create(void *data, std::function< int(QmlObjectList *)> itemCount, std::function< QObject *(QmlObjectList *, int)> itemAt, std::function< void(QmlObjectList *, QObject *)> appendItem, std::function< void(QmlObjectList *, int)> removeItemAt, std::function< void(QmlObjectList *)> clearItems, QObject *parent=nullptr)
 * \brief Create a modifiable list
 */

/**
 * \fn lv::QmlObjectList::create(void *data, std::function< int(QmlObjectList *)> itemCount, std::function< QObject *(QmlObjectList *, int)> itemAt, QObject *parent=nullptr)
 * \brief Creates a read-only list
 */

/**
 * \fn lv::QmlObjectList::defaultItemCount
 * \brief Default implementation of function that returns the number of items
 */

/**
 * \fn lv::QmlObjectList::defaultItemAt
 * \brief Default implementation of function that returns the item at the given index
 */

/**
 * \fn lv::QmlObjectList::defaultAppendItem
 * \brief Default implementation of function that appends the item to the list
 */

/**
 * \fn lv::QmlObjectList::defaultRemoveItemAt
 * \brief Default implementation of function that removes the item at the given index
 */

/**
 * \fn lv::QmlObjectList::defaultClearItems
 * \brief Default implementation of function that clears the list
 */

/**
 * \brief Default contructor
 *
 * The default QmlObjectList is actually a wrapped list of QObjects.
 */
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

/**
 * \brief Constructor for a modifiable list
 */
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

/**
 * \brief Constructor for a read-only list
 *
 * We don't pass the functions which would allow us to modify the list in any way
 */
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

/**
 * \brief Default destructor
 *
 * If a `clear` method is provided, we clear the container items as well
 */
QmlObjectList::~QmlObjectList(){
    if ( m_clearItems )
        m_clearItems(this);
}

/**
 * \brief Returns the items this list contains
 */
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


/**
 * \brief Sets a `clone` function for the list
 */
void QmlObjectList::setClone(std::function<QmlObjectList *(const QmlObjectList *)> clone){
    m_clone = clone;
}

void QmlObjectList::setAssignAt(std::function<void (QmlObjectList *, int, QObject *)> assignAt){
    m_assignAt = assignAt;
}

/**
 * \brief Creates a const clone of our modifiable list
 *
 * If there's no `clone` function, this will throw an exception
 */
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


/**
 * \brief Clone a modifiable list
 *
 * If there's no `clone` function, this will throw an exception
 */
QmlObjectList *QmlObjectList::clone() const{
    if ( !m_clone ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is not clonable.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    }
    return m_clone(this);
}

/**
 * \brief Converts the list to a js array.
 */
QJSValue QmlObjectList::toArray(){
    QQmlEngine* engine = lv::ViewContext::instance().engine()->engine();
    QJSValue result = engine->newArray(itemCount());

    for ( int i = 0; i < itemCount(); ++i ){
        result.setProperty(i, engine->newQObject(itemAt(i)));
    }

    return result;
}

/**
 * \brief Returns the item at the given index
 */
QObject *QmlObjectList::itemAt(int index)
{
    if (index >= m_itemCount(this))
        return nullptr;
    return m_itemAt(this, index);
}

/**
 * \brief Returns the item count
 */
int QmlObjectList::itemCount()
{
    return m_itemCount(this);
}

/**
 * \brief Clear a modifiable list
 *
 * If a list is read-only, this will throw an exception
 */
void QmlObjectList::clearItems()
{
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
void QmlObjectList::appendItem(QObject* item)
{
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_appendItem(this,item);
}

/**
 * \brief Remove an item from a modifiable list at the given index
 *
 * If a list is read-only, this will throw an exception
 */
void QmlObjectList::removeItemAt(int index)
{
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
    m_removeItemAt(this, index);
}

void QmlObjectList::assignAt(int index, QObject *item){
    if ( isConst() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "List is const.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }

    m_assignAt(this, index, item);
}

/**
 * \brief Return a data model for this list
 */
QmlObjectListModel* QmlObjectList::model(){
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

void QmlObjectList::defaultAssignAt(QmlObjectList *list , int index, QObject *o)
{
    auto data = list->dataAs<QList<QObject*>>();
    if (index >= data->size() || index < 0) return;
    (*data)[index] = o;
}

void QmlObjectList::defaultClearItems(QmlObjectList *list)
{
    auto data = list->dataAs<QList<QObject*>>();
    data->clear();
}


}// namespace

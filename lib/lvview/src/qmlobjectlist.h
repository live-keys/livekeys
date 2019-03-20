#ifndef QMLOBJECTLIST_H
#define QMLOBJECTLIST_H

#include <QObject>
#include <QQmlListProperty>
#include <functional>
#include <QDebug>

#include "live/lvviewglobal.h"
#include "live/shared.h"

namespace lv{

class QmlObjectListModel;

class LV_VIEW_EXPORT QmlObjectList : public Shared {


    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> items READ items)

public:
    explicit QmlObjectList(QObject* parent = nullptr);

    QmlObjectList(
        void* data,
        const std::type_info* typeInfo,
        std::function<int(QmlObjectList*)> itemCount,
        std::function<QObject*(QmlObjectList*, int)> itemAt,
        std::function<void(QmlObjectList*, QObject*)> appendItem,
        std::function<void(QmlObjectList*, int)> removeItemAt,
        std::function<void(QmlObjectList*)> clearItems,
        QObject *parent = nullptr
    );
    QmlObjectList(
        void* data,
        const std::type_info* typeInfo,
        std::function<int(QmlObjectList*)> itemCount,
        std::function<QObject*(QmlObjectList*, int)> itemAt,
        QObject *parent = nullptr
    );
    ~QmlObjectList();

    QQmlListProperty<QObject> items();
    bool isConst() const;
    template<typename T> static QmlObjectList* create(
        void* data,
        std::function<int(QmlObjectList*)> itemCount,
        std::function<QObject*(QmlObjectList*, int)> itemAt,
        std::function<void(QmlObjectList*, QObject*)> appendItem,
        std::function<void(QmlObjectList*, int)> removeItemAt,
        std::function<void(QmlObjectList*)> clearItems,
        QObject *parent = nullptr
    );

    template<typename T> static QmlObjectList* create(
        void* data,
        std::function<int(QmlObjectList*)> itemCount,
        std::function<QObject*(QmlObjectList*, int)> itemAt,
        QObject *parent = nullptr
    );

    void* data();
    template<typename T> bool canCast() const;
    template<typename T> T* dataAs();

    template<typename T>
    void populateObjectList(
        void* data,
        std::function<int(QmlObjectList*)> itemCount,
        std::function<QObject*(QmlObjectList*, int)> itemAt,
        std::function<void(QmlObjectList*, QObject*)> appendItem,
        std::function<void(QmlObjectList*, int)> removeItemAt,
        std::function<void(QmlObjectList*)> clearItems
    );


    static void defaultAppendItem(QmlObjectList*, QObject*);
    static int defaultItemCount(QmlObjectList*);
    static QObject* defaultItemAt(QmlObjectList*, int);
    static void defaultRemoveItemAt(QmlObjectList*, int);
    static void defaultClearItems(QmlObjectList*);
    void setClone(std::function<QmlObjectList*(const QmlObjectList *)> clone);

public slots:
    QObject* itemAt(int index);
    int itemCount();
    void clearItems();
    void appendItem(QObject* item);
    void removeItemAt(int index);
    lv::QmlObjectListModel* model();

    lv::QmlObjectList *cloneConst() const;
    lv::QmlObjectList* clone() const;

private:
    static int itemCount(QQmlListProperty<QObject>*);
    static void removeItemAt(QQmlListProperty<QObject>*, int);
    static QObject* itemAt(QQmlListProperty<QObject>*, int);

    static void appendItem(QQmlListProperty<QObject>*, QObject*);
    static void clearItems(QQmlListProperty<QObject>*);

private:
    void*                 m_data;
    const std::type_info* m_type;

    std::function<int(QmlObjectList*)>                  m_itemCount;
    std::function<QObject*(QmlObjectList*, int)>        m_itemAt;
    std::function<QmlObjectList*(const QmlObjectList*)> m_clone;

    std::function<void(QmlObjectList*, QObject*)> m_appendItem;
    std::function<void(QmlObjectList*, int)>      m_removeItemAt;
    std::function<void(QmlObjectList*)>           m_clearItems;

    friend class QmlObjectListModel;
};

template<typename T> QmlObjectList *QmlObjectList::create(
        void *data,
        std::function<int (QmlObjectList *)> itemCount,
        std::function<QObject *(QmlObjectList *, int)> itemAt,
        std::function<void (QmlObjectList *, QObject *)> appendItem,
        std::function<void(QmlObjectList*, int)> removeItemAt,
        std::function<void (QmlObjectList *)> clearItems,
        QObject *parent)
{
    return new QmlObjectList(data, &typeid(T), itemCount, itemAt, appendItem, removeItemAt, clearItems, parent);
}

template<typename T>
void QmlObjectList::populateObjectList(void *data, std::function<int (QmlObjectList *)> itemCount, std::function<QObject *(QmlObjectList *, int)> itemAt, std::function<void (QmlObjectList *, QObject *)> appendItem, std::function<void (QmlObjectList *, int)> removeItemAt, std::function<void (QmlObjectList *)> clearItems)
{
    m_data = data;
    m_type = &typeid(T);
    m_itemCount = itemCount;
    m_appendItem = appendItem;
    m_itemAt = itemAt;
    m_removeItemAt = removeItemAt;
    m_clearItems = clearItems;
}

template<typename T> QmlObjectList *QmlObjectList::create(
        void *data,
        std::function<int (QmlObjectList *)> itemCount,
        std::function<QObject *(QmlObjectList *, int)> itemAt,
        QObject *parent)
{
    return new QmlObjectList(data, &typeid(T), itemCount, itemAt, parent);
}

template<typename T> T* QmlObjectList::dataAs(){
    bool cc = canCast<T>();
    if ( cc )
        return reinterpret_cast<T*>(m_data);
    return 0;
}

template<typename T> bool QmlObjectList::canCast() const{
    return *m_type == typeid(T);
}

inline void *QmlObjectList::data(){
    return m_data;
}

inline bool QmlObjectList::isConst() const{
    return !m_appendItem || !m_removeItemAt;
}

}// namespace

#endif // QMLLIST_H

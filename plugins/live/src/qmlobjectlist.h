#ifndef QMLOBJECTLIST_H
#define QMLOBJECTLIST_H

#include <QObject>
#include <QQmlListProperty>
#include <functional>
#include "qliveglobal.h"

namespace lv{

class Q_LIVE_EXPORT QmlObjectList : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> items READ items)

public:
    QmlObjectList(
        void* data,
        const std::type_info* typeInfo,
        std::function<int(QmlObjectList*)> itemCount,
        std::function<QObject*(QmlObjectList*, int)> itemAt,
        std::function<void(QmlObjectList*, QObject*)> appendItem,
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
    bool isReadOnly() const;

    template<typename T> static QmlObjectList* create(
        void* data,
        std::function<int(QmlObjectList*)> itemCount,
        std::function<QObject*(QmlObjectList*, int)> itemAt,
        std::function<void(QmlObjectList*, QObject*)> appendItem,
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

private:
    static void appendItem(QQmlListProperty<QObject>*, QObject*);
    static int itemCount(QQmlListProperty<QObject>*);
    static QObject* itemAt(QQmlListProperty<QObject>*, int);
    static void clearItems(QQmlListProperty<QObject>*);

private:
    void*                 m_data;
    const std::type_info* m_type;

    std::function<int(QmlObjectList*)>            m_itemCount;
    std::function<QObject*(QmlObjectList*, int)>  m_itemAt;

    std::function<void(QmlObjectList*, QObject*)> m_appendItem;
    std::function<void(QmlObjectList*)>           m_clearItems;

};


template<typename T> QmlObjectList *QmlObjectList::create(
        void *data,
        std::function<int (QmlObjectList *)> itemCount,
        std::function<QObject *(QmlObjectList *, int)> itemAt,
        std::function<void (QmlObjectList *, QObject *)> appendItem,
        std::function<void (QmlObjectList *)> clearItems,
        QObject *parent)
{
    return new QmlObjectList(data, &typeid(T), itemCount, itemAt, appendItem, clearItems, parent);
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
    if ( canCast<T>() )
        return reinterpret_cast<T*>(m_data);
    return 0;
}

template<typename T> bool QmlObjectList::canCast() const{
    return *m_type == typeid(T);
}

inline void *QmlObjectList::data(){
    return m_data;
}

inline bool QmlObjectList::isReadOnly() const{
    return !m_appendItem;
}

}// namespace

#endif // QMLLIST_H

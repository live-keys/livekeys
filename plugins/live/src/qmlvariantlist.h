#ifndef QMLVARIANTLIST_H
#define QMLVARIANTLIST_H

#include <QObject>
#include <QVariant>
#include <functional>
#include "qliveglobal.h"
#include "live/shared.h"

namespace lv{

class Q_LIVE_EXPORT QmlVariantList : public lv::Shared{

    Q_OBJECT
    Q_PROPERTY(QVariantList items READ items WRITE setItems)

public:
    QmlVariantList(void* data,
        const std::type_info* typeInfo,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int(QmlVariantList*)> itemCount,
        std::function<QVariant(QmlVariantList *, int)> itemAt,
        std::function<void(QmlVariantList*, QVariant)> appendItem,
        std::function<void(QmlVariantList*)> clearItems,
        QObject *parent = nullptr
    );
    QmlVariantList(
        void* data,
        const std::type_info* typeInfo,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int(QmlVariantList*)> itemCount,
        std::function<QVariant(QmlVariantList*, int)> itemAt,
        QObject *parent = nullptr
    );
    ~QmlVariantList();

    template<typename T> static QmlVariantList* create(
        void* data,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int(QmlVariantList*)> itemCount,
        std::function<QVariant(QmlVariantList*, int)> itemAt,
        std::function<void(QmlVariantList*, QVariant)> appendItem,
        std::function<void(QmlVariantList*)> clearItems,
        QObject *parent = nullptr
    );

    template<typename T> static QmlVariantList* create(
        void* data,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int(QmlVariantList*)> itemCount,
        std::function<QVariant(QmlVariantList*, int)> itemAt,
        QObject *parent = nullptr
    );

    void* data();
    template<typename T> bool canCast() const;
    template<typename T> T* dataAs();

    void setItems(const QVariantList& items);

    void setClone(std::function<QmlVariantList*(const QmlVariantList *)> clone);
    void setQuickAssign(std::function<void(QmlVariantList*, QVariantList)> qa);

public slots:
    QVariantList items();
    QVariant itemAt(int index);
    int itemCount();
    bool isConst() const;

    QmlVariantList *cloneConst() const;
    QmlVariantList* clone() const;

    void clearItems();
    void appendItem(QVariant item);

private:
    void*                 m_data;
    const std::type_info* m_type;

    std::function<QVariantList(QmlVariantList*)>          m_itemList;
    std::function<int(QmlVariantList*)>                   m_itemCount;
    std::function<QVariant(QmlVariantList*, int)>         m_itemAt;
    std::function<QmlVariantList*(const QmlVariantList*)> m_clone;

    std::function<void(QmlVariantList*, QVariant)>        m_appendItem;
    std::function<void(QmlVariantList*)>                  m_clearItems;
    std::function<void(QmlVariantList*, QVariantList)>    m_quickAssign;
};


template<typename T> QmlVariantList *QmlVariantList::create(
        void *data,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int (QmlVariantList *)> itemCount,
        std::function<QVariant(QmlVariantList *, int)> itemAt,
        std::function<void (QmlVariantList *, QVariant)> appendItem,
        std::function<void (QmlVariantList *)> clearItems,
        QObject *parent)
{
    return new QmlVariantList(data, &typeid(T), itemList, itemCount, itemAt, appendItem, clearItems, parent);
}

template<typename T> QmlVariantList *QmlVariantList::create(
        void *data,
        std::function<QVariantList(QmlVariantList*)> itemList,
        std::function<int (QmlVariantList *)> itemCount,
        std::function<QVariant(QmlVariantList *, int)> itemAt,
        QObject *parent)
{
    return new QmlVariantList(data, &typeid(T), itemList, itemCount, itemAt, parent);
}

template<typename T> T* QmlVariantList::dataAs(){
    if ( canCast<T>() )
        return reinterpret_cast<T*>(m_data);
    return 0;
}

template<typename T> bool QmlVariantList::canCast() const{
    return *m_type == typeid(T);
}

inline void *QmlVariantList::data(){
    return m_data;
}

inline bool QmlVariantList::isConst() const{
    return !m_appendItem;
}

}// namespace

#endif // QMLVARIANTLIST_H

#ifndef QSTATICITEMCONTAINER_H
#define QSTATICITEMCONTAINER_H

#include "qliveglobal.h"
#include <QLinkedList>
#include <QObject>
#include <QDebug>

#define QSTATIC_ITEM_CONTAINER_DEBUG_FLAG
#ifdef QSTATIC_ITEM_CONTAINER_DEBUG_FLAG
#define QSTATIC_ITEM_CONTAINER_DEBUG(_param) qDebug() << (_param)
#else
#define QSTATIC_ITEM_CONTAINER_DEBUG(_param)
#endif

class QQuickItem;
class QQuickWindow;
class QStaticTypeContainerBase;

class Q_LIVE_EXPORT QStaticContainer : public QObject{

    Q_OBJECT

public:
    QStaticContainer(QObject* parent = 0);
    ~QStaticContainer();

    void statecontainer(QStaticTypeContainerBase* container);
    void setWindow(QQuickWindow* window);

    template<class T> T* get(const QString& key);
    template<class T> void set(const QString& key, T* value);

    static QStaticContainer* grabFromContext(QQuickItem* item, const QString& contextProperty = "staticContainer");

public slots:
    void beforeCompile();
    void afterCompile();
    void clearStates();

private:
    QLinkedList<QStaticTypeContainerBase*> m_stateContainerList;

private:
    // disable copy
    QStaticContainer(QStaticContainer const&);
    QStaticContainer& operator=(QStaticContainer const&);
};


class Q_LIVE_EXPORT QStaticTypeContainerBase{

public:
    QStaticTypeContainerBase(){}
    virtual ~QStaticTypeContainerBase(){}

    virtual void beforeCompile() = 0;
    virtual void afterCompile() = 0;
    virtual void clearStates() = 0;
};


template<typename T>
class QStaticTypeContainer : public QStaticTypeContainerBase{

public:
    static QStaticTypeContainer<T> &instance(QStaticContainer* parent);

    void registerState(const QString& key, T* state);
    T*   state(const QString& key);

    void beforeCompile();
    void afterCompile();
    void clearStates();

private:
    QStaticTypeContainer(QStaticContainer* item);
    ~QStaticTypeContainer();

    // disable copy
    QStaticTypeContainer(QStaticTypeContainer const&);
    QStaticTypeContainer& operator=(QStaticTypeContainer const&);

    static QStaticTypeContainer* m_instance;

private:
    QMap<QString, T*>   m_states;
    QMap<QString, bool> m_statesActive;

};

template<class T> T *QStaticContainer::get(const QString &key){
    return QStaticTypeContainer<T>::instance(this).state(key);
}


template<class T> void QStaticContainer::set(const QString &key, T *value){
    QStaticTypeContainer<T>::instance(this).registerState(key, value);
}

template<typename T> QStaticTypeContainer<T>* QStaticTypeContainer<T>::m_instance = 0;

/**
 * @brief Singleton instance getter.
 */
template<typename T> QStaticTypeContainer<T> &QStaticTypeContainer<T>::instance(QStaticContainer* item){
    if ( m_instance == 0 )
        m_instance = new QStaticTypeContainer<T>(item);
    return *m_instance;
}

/**
 * @brief Register a state.
 * @param key : The key to register the state by
 * @param state : The actual state
 */
template<typename T> void QStaticTypeContainer<T>::registerState(const QString &key, T *state){
    m_states[key]       = state;
    m_statesActive[key] = true;
    QSTATIC_ITEM_CONTAINER_DEBUG(QString("Key activated : ") + key);
}

/**
 * @brief Retrieve a registered state.
 * @return The found state on success. False otherwise.
 */
template<typename T> T *QStaticTypeContainer<T>::state(const QString &key){
    typename QMap<QString, T*>::iterator it = m_states.find(key);
    if ( it != m_states.end() ){
        m_statesActive[it.key()] = true;
        QSTATIC_ITEM_CONTAINER_DEBUG(QString("Key activated : ") + key);
        return it.value();
    }
    return 0;
}

/**
 * @brief Before compilation routine.
 */
template<typename T> void QStaticTypeContainer<T>::beforeCompile(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----Before Compile-----");
    QMap<QString, bool>::iterator it = m_statesActive.begin();
    while ( it != m_statesActive.end() ){
        if ( it.value() == true ){
            QSTATIC_ITEM_CONTAINER_DEBUG(QString("Key deactivated : ") + it.key());
            it.value() = false;
        }
        ++it;
    }
}

/**
 * @brief After compilation routine.
 */
template<typename T> void QStaticTypeContainer<T>::afterCompile(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----After Compile-----");
    QMap<QString, bool>::iterator it = m_statesActive.begin();
    while ( it != m_statesActive.end() ){
        if ( it.value() == false ){
            QMap<QString, bool>::iterator prev = it;
            ++it;
            delete m_states[prev.key()];
            m_states.remove(prev.key());
            QSTATIC_ITEM_CONTAINER_DEBUG(QString("Key deleted : ") + prev.key());
            m_statesActive.erase(prev);
        } else {
            ++it;
        }
    }
}

template<typename T> void QStaticTypeContainer<T>::clearStates(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----Clear States-----");
    for ( typename QMap<QString, T*>::iterator it = m_states.begin(); it != m_states.end(); ++it )
        delete it.value();
    m_states.clear();
    m_statesActive.clear();
}

/**
 * @brief QStaticItemTypeContainer constructor
 * @param item
 */
template<typename T> QStaticTypeContainer<T>::QStaticTypeContainer(QStaticContainer *item)
    : QStaticTypeContainerBase(){

    item->statecontainer(this);
}

/**
 * @brief QStaticItemTypeContainer destructor
 */
template<typename T> QStaticTypeContainer<T>::~QStaticTypeContainer(){
    clearStates();
}

#endif // QSTATICITEMCONTAINER_H

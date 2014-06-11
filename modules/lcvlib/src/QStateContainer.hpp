#ifndef QSTATECONTAINER_HPP
#define QSTATECONTAINER_HPP

#include "QLCVGlobal.hpp"
#include <QObject>
#include <QMap>
#include <QLinkedList>

#define QSTATE_CONTAINER_DEBUG_FLAG
#ifdef QSTATE_CONTAINER_DEBUG_FLAG
#define QSTATE_CONTAINER_DEBUG(_param) qDebug() << (_param)
#else
#define QSTATE_CONTAINER_DEBUG(_param)
#endif

class QQuickItem;
class QQuickWindow;
class QStateContainerBase;

class Q_LCV_EXPORT QStateContainerManager : public QObject{

    Q_OBJECT

public:
    static QStateContainerManager& instance(QQuickItem* compiler, QObject* parent = 0);
    void registerStateContainer(QStateContainerBase* container);

private:
    static void cleanStateManager();

    QStateContainerManager(QQuickItem *item, QObject* parent = 0);
    ~QStateContainerManager();

    // disable copy
    QStateContainerManager(QStateContainerManager const&);
    QStateContainerManager& operator=(QStateContainerManager const&);

    static QStateContainerManager* m_instance;

public slots:
    void beforeCompile();
    void afterCompile();
    void attachWindow(QQuickWindow* window);

private:
    QLinkedList<QStateContainerBase*> m_stateContainerList;

};


class Q_LCV_EXPORT QStateContainerBase{

public:
    QStateContainerBase();
    virtual ~QStateContainerBase(){}

    virtual void beforeCompile() = 0;
    virtual void afterCompile() = 0;
};


template<typename T>
class QStateContainer : public QStateContainerBase{

public:
    static QStateContainer<T> &instance(QQuickItem *item);

    void registerState(const QString& key, T* state);
    T*   state(const QString& key);

    void beforeCompile();
    void afterCompile();

private:
    QStateContainer(QQuickItem* item);
    ~QStateContainer();

    // disable copy
    QStateContainer(QStateContainer const&);
    QStateContainer& operator=(QStateContainer const&);

    static QStateContainer* m_instance;

private:
    QMap<QString, T*>   m_states;
    QMap<QString, bool> m_statesActive;

};

template<typename T> QStateContainer<T>* QStateContainer<T>::m_instance = 0;

template<typename T> QStateContainer<T> &QStateContainer<T>::instance(QQuickItem* item){
    if ( m_instance == 0 )
        m_instance = new QStateContainer<T>(item);
    return *m_instance;
}


template<typename T> void QStateContainer<T>::registerState(const QString &key, T *state){
    m_states[key]       = state;
    m_statesActive[key] = true;
    QSTATE_CONTAINER_DEBUG(QString("Key activated : ") + key);
}

template<typename T> T *QStateContainer<T>::state(const QString &key){
    QMap<QString, T*>::iterator it = m_states.find(key);
    if ( it != m_states.end() ){
        m_statesActive[it.key()] = true;
        QSTATE_CONTAINER_DEBUG(QString("Key activated : ") + key);
        return it.value();
    }
    return 0;
}

template<typename T> void QStateContainer<T>::beforeCompile(){
    QSTATE_CONTAINER_DEBUG("-----Before Compile-----");
    QMap<QString, bool>::iterator it = m_statesActive.begin();
    while ( it != m_statesActive.end() ){
        if ( it.value() == true ){
            QSTATE_CONTAINER_DEBUG(QString("Key deactivated : ") + it.key());
            it.value() = false;
        }
        ++it;
    }
}

template<typename T> void QStateContainer<T>::afterCompile(){
    QSTATE_CONTAINER_DEBUG("-----After Compile-----");
    QMap<QString, bool>::iterator it = m_statesActive.begin();
    while ( it != m_statesActive.end() ){
        qDebug() << it.key() << m_states[it.key()];
        if ( it.value() == false ){
            QMap<QString, bool>::iterator prev = it;
            ++it;
            qDebug() << "here";
            delete m_states[prev.key()];
            m_states.remove(prev.key());
            QSTATE_CONTAINER_DEBUG(QString("Key deleted : ") + prev.key());
            m_statesActive.erase(prev);
            qDebug() << "here2";
        } else {
            ++it;
        }
    }
    qDebug() << "##after compile end.";
}

template<typename T> QStateContainer<T>::QStateContainer(QQuickItem* item)
    : QStateContainerBase(){

    QStateContainerManager::instance(item).registerStateContainer(this);
}

template<typename T> QStateContainer<T>::~QStateContainer(){
    for ( QMap<QString, T*>::iterator it = m_states.begin(); it != m_states.end(); ++it )
        delete it.value();
    m_states.clear();
    m_statesActive.clear();
}

#endif // QSTATECONTAINER_HPP

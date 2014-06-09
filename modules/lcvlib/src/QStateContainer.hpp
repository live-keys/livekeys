#ifndef QSTATECONTAINER_HPP
#define QSTATECONTAINER_HPP

#include "QLCVGlobal.hpp"
#include <QObject>
#include <QMap>
#include <QLinkedList>

class QQuickView;
class QStateContainerBase;

class Q_LCV_EXPORT QStateContainerManager : public QObject{

    Q_OBJECT

public:
    static QStateContainerManager& instance(QQuickView* compiler, QObject* parent = 0);

private:
    static void cleanStateManager();

    QStateContainerManager(QQuickView *view, QObject* parent = 0);
    ~QStateContainerManager();

    void registerStateContainer(QStateContainerBase* container);

    // disable copy
    QStateContainerManager(QStateContainerManager const&);
    QStateContainerManager& operator=(QStateContainerManager const&);

    static QStateContainerManager* m_instance;

public slots:
    void beforeCompile();

private:
    QLinkedList<QStateContainerBase*> m_stateContainerList;

};


class Q_LCV_EXPORT QStateContainerBase{

public:
    QStateContainerBase();
    virtual ~QStateContainerBase(){}

    virtual void beforeCompile() = 0;
};


template<typename T>
class Q_LCV_EXPORT QStateContainer : public QStateContainerBase{

public:
    static QStateContainer<T> &instance();

    void registerState(const QString& key, T* state);
    T*   state(const QString& key);

    void beforeCompile();

private:
    QStateContainer();
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

template<typename T> QStateContainer<T> &QStateContainer<T>::instance(){
    if ( m_instance == 0 )
        m_instance = new QStateContainer<T>();
    return *m_instance;
}


template<typename T> void QStateContainer<T>::registerState(const QString &key, T *state){
    m_states[key] = state;
    m_statesActive[key] = true;
}

template<typename T> T *QStateContainer<T>::state(const QString &key){
    QMap<QString, T*>::iterator it = m_states.find(key);
    if ( it != m_states.end() ){
        m_statesActive = true;
        it.value()->activate();
        return it.value();
    }
    return 0;
}

template<typename T> void QStateContainer<T>::beforeCompile(){
    for ( QMap<QString, bool>::iterator it = m_statesActive.begin(); it != m_statesActive.end(); ++it ){
        if ( it.value() == true ){
            it.value() = false;
        } else {
            QMap<QString, bool>::iterator prev = it;
            ++it;
            delete m_states[prev.key()];
            m_states.remove(prev.key());
            m_statesActive.erase(prev);
        }
    }
}

template<typename T> QStateContainer<T>::QStateContainer()
    : QStateContainerBase(){

    QStateContainerManager::instance().RegisterStateContainer(this);
}

template<typename T> QStateContainer<T>::~QStateContainer(){
    for ( QMap<QString, T*>::iterator it = m_states.begin(); it != m_states.end(); ++it )
        delete it.value();
    m_states.clear();
    m_statesActive.clear();
}

#endif // QSTATECONTAINER_HPP

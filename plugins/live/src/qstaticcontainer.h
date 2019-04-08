/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef QSTATICCONTAINER_H
#define QSTATICCONTAINER_H

#include "live/lvliveglobal.h"
#include <QLinkedList>
#include <QObject>

class QQuickItem;
class QQuickWindow;
class QStaticTypeContainerBase;

#ifdef VLOG_DEBUG_BUILD
#define debug_static_container(_message) QStaticContainer::debugMessage(_message)
#else
#define debug_static_container(_message)
#endif

class LV_LIVE_EXPORT QStaticContainer : public QObject{

    Q_OBJECT

public:
    QStaticContainer(QObject* parent = 0);
    ~QStaticContainer();

    void statecontainer(QStaticTypeContainerBase* container);

    template<class T> T* get(const QString& key);
    template<class T> void set(const QString& key, T* value);

    static QStaticContainer* grabFromContext(QQuickItem* item, const QString& contextProperty = "staticContainer");
    static void debugMessage(const QString& message);

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


/// \private
class LV_LIVE_EXPORT QStaticTypeContainerBase{

public:
    QStaticTypeContainerBase(){}
    virtual ~QStaticTypeContainerBase(){}

    virtual void beforeCompile() = 0;
    virtual void afterCompile() = 0;
    virtual void clearStates() = 0;
};

/// \private
template<typename T>
class QStaticTypeContainer : public QStaticTypeContainerBase{

private:
    class Entry{
    public:
        Entry(T* v) : value(v), activated(true){}

        T*   value;
        bool activated;
    };

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
    QMap<QString, Entry> m_entries;
    QLinkedList<T*>      m_toDelete;
};

template<class T> T *QStaticContainer::get(const QString &key){
    return QStaticTypeContainer<T>::instance(this).state(key);
}


template<class T> void QStaticContainer::set(const QString &key, T *value){
    QStaticTypeContainer<T>::instance(this).registerState(key, value);
}

template<typename T> QStaticTypeContainer<T>* QStaticTypeContainer<T>::m_instance = 0;

template<typename T> QStaticTypeContainer<T> &QStaticTypeContainer<T>::instance(QStaticContainer* item){
    if ( m_instance == 0 )
        m_instance = new QStaticTypeContainer<T>(item);
    return *m_instance;
}

template<typename T> void QStaticTypeContainer<T>::registerState(const QString &key, T *state){
    m_entries.insert(key, Entry(state));
    debug_static_container(QString("Key set and activated : ") + key);
}

template<typename T> T *QStaticTypeContainer<T>::state(const QString &key){
    typename QMap<QString, Entry>::iterator it = m_entries.find(key);
    if ( it != m_entries.end() ){
        it.value().activated = true;
        debug_static_container(QString("Key activated : ") + key);
        return it.value().value;
    }
    return 0;
}

template<typename T> void QStaticTypeContainer<T>::beforeCompile(){
    while ( !m_toDelete.isEmpty() )
        delete m_toDelete.takeLast();

    typename QMap<QString, Entry>::iterator it = m_entries.begin();
    while ( it != m_entries.end() ){
        if ( it.value().activated == true ){
            debug_static_container(QString("Key deactivated : ") + it.key());
            it.value().activated = false;
        }
        ++it;
    }
}

template<typename T> void QStaticTypeContainer<T>::afterCompile(){
    typename QMap<QString, Entry>::iterator it = m_entries.begin();
    while ( it != m_entries.end() ){
        if ( it.value().activated == false ){
            debug_static_container(QString("Key removed: ") + it.key());
            m_toDelete.append(it.value().value);
            it = m_entries.erase(it);
        } else {
            ++it;
        }
    }
}

template<typename T> void QStaticTypeContainer<T>::clearStates(){
    for ( typename QMap<QString, Entry>::iterator it = m_entries.begin(); it != m_entries.end(); ++it ){
        m_toDelete.append(it.value().value);
    }
    m_entries.clear();
}

template<typename T> QStaticTypeContainer<T>::QStaticTypeContainer(QStaticContainer *item)
    : QStaticTypeContainerBase(){

    item->statecontainer(this);
}

template<typename T> QStaticTypeContainer<T>::~QStaticTypeContainer(){
    clearStates();
}

#endif // QSTATICCONTAINER_H

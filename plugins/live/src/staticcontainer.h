/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

#ifndef LVSTATICCONTAINER_H
#define LVSTATICCONTAINER_H

#include "live/lvliveglobal.h"
#include <QLinkedList>
#include <QObject>

#ifdef VLOG_DEBUG_BUILD
#define debug_static_container(_message) lv::StaticContainer::debugMessage(_message)
#else
#define debug_static_container(_message)
#endif

class QQuickItem;
class QQmlContext;
class QQuickWindow;

namespace lv{

class StaticTypeContainerBase;

class LV_LIVE_EXPORT StaticContainer : public QObject{

    Q_OBJECT

public:
    StaticContainer(QObject* parent = 0);
    ~StaticContainer();

    void statecontainer(StaticTypeContainerBase* container);

    template<class T> T* get(const QString& key);
    template<class T> void set(const QString& key, T* value);

    static StaticContainer* grabFromContext(QQuickItem* item, const QString& contextProperty = "staticContainer");
    static void debugMessage(const QString& message);

public slots:
    void beforeCompile(const QUrl&);
    void afterCompile(QObject*, const QUrl&, QObject*, QQmlContext*);
    void clearStates();

private:
    QLinkedList<StaticTypeContainerBase*> m_stateContainerList;

private:
    // disable copy
    StaticContainer(StaticContainer const&);
    StaticContainer& operator=(StaticContainer const&);
};


/// \private
class LV_LIVE_EXPORT StaticTypeContainerBase{

public:
    StaticTypeContainerBase(){}
    virtual ~StaticTypeContainerBase(){}

    virtual void beforeCompile() = 0;
    virtual void afterCompile() = 0;
    virtual void clearStates() = 0;
};

/// \private
template<typename T>
class StaticTypeContainer : public StaticTypeContainerBase{

private:
    class Entry{
    public:
        Entry(T* v) : value(v), activated(true){}

        T*   value;
        bool activated;
    };

public:
    static StaticTypeContainer<T> &instance(StaticContainer* parent);

    void registerState(const QString& key, T* state);
    T*   state(const QString& key);

    void beforeCompile();
    void afterCompile();
    void clearStates();

private:
    StaticTypeContainer(StaticContainer* item);
    ~StaticTypeContainer();

    // disable copy
    StaticTypeContainer(StaticTypeContainer const&);
    StaticTypeContainer& operator=(StaticTypeContainer const&);

    static StaticTypeContainer* m_instance;

private:
    QMap<QString, Entry> m_entries;
    QLinkedList<T*>      m_toDelete;
};

template<class T> T *StaticContainer::get(const QString &key){
    return StaticTypeContainer<T>::instance(this).state(key);
}


template<class T> void StaticContainer::set(const QString &key, T *value){
    StaticTypeContainer<T>::instance(this).registerState(key, value);
}

template<typename T> StaticTypeContainer<T>* StaticTypeContainer<T>::m_instance = 0;

template<typename T> StaticTypeContainer<T> &StaticTypeContainer<T>::instance(StaticContainer* item){
    if ( m_instance == 0 )
        m_instance = new StaticTypeContainer<T>(item);
    return *m_instance;
}

template<typename T> void StaticTypeContainer<T>::registerState(const QString &key, T *state){
    m_entries.insert(key, Entry(state));
    debug_static_container(QString("Key set and activated : ") + key);
}

template<typename T> T *StaticTypeContainer<T>::state(const QString &key){
    typename QMap<QString, Entry>::iterator it = m_entries.find(key);
    if ( it != m_entries.end() ){
        it.value().activated = true;
        debug_static_container(QString("Key activated : ") + key);
        return it.value().value;
    }
    return 0;
}

template<typename T> void StaticTypeContainer<T>::beforeCompile(){
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

template<typename T> void StaticTypeContainer<T>::afterCompile(){
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

template<typename T> void StaticTypeContainer<T>::clearStates(){
    for ( typename QMap<QString, Entry>::iterator it = m_entries.begin(); it != m_entries.end(); ++it ){
        m_toDelete.append(it.value().value);
    }
    m_entries.clear();
}

template<typename T> StaticTypeContainer<T>::StaticTypeContainer(StaticContainer *item)
    : StaticTypeContainerBase(){

    item->statecontainer(this);
}

template<typename T> StaticTypeContainer<T>::~StaticTypeContainer(){
    clearStates();
}

}// namespace`

#endif // LVSTATICCONTAINER_H

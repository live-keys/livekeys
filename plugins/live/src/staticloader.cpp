/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "staticloader.h"
#include "live/staticcontainer.h"
#include <QQmlEngine>

namespace lv{

class StaticLoaderItem{
public:
    StaticLoaderItem() : item(0){}
    ~StaticLoaderItem(){
        if ( item )
            delete item;
    }

    QObject* item;
};

StaticLoader::StaticLoader(QQuickItem *parent)
    : QQuickItem(parent)
    , m_source(0)
    , m_data(0)
{
}

StaticLoader::~StaticLoader(){
    if ( m_data ){
        m_data->item->setParent(0);
        QQuickItem* item = qobject_cast<QQuickItem*>(m_data->item);
        if ( item )
            item->setParentItem(0);
    }
}


QObject *StaticLoader::item() const{
    if ( m_data )
        return m_data->item;
    return 0;
}

void StaticLoader::setSource(QQmlComponent *arg){
    if ( m_source == arg )
        return;

    m_source  = arg;
    emit sourceChanged();

    if ( m_source && m_data && !m_data->item ){
        createObject();
    }
}

void StaticLoader::staticLoad(const QString &id){
    StaticContainer* container = StaticContainer::grabFromContext(this);
    m_data = container->get<StaticLoaderItem>(id);
    if ( !m_data ){
        m_data = new StaticLoaderItem;
        container->set<StaticLoaderItem>(id, m_data);

        if ( m_source ){
            createObject();
        }

    } else if ( m_data->item ){
        m_data->item->setParent(this);
        QQuickItem* item = qobject_cast<QQuickItem*>(m_data->item);
        if ( item ){
            item->setParentItem(this);
        }
    }

    emit itemChanged();

}

void StaticLoader::createObject(){
    QObject* obj = qobject_cast<QObject*>(m_source->create(m_source->creationContext()));
    if ( obj == 0 ){
        qCritical("StaticLoader: Failed to create item: %s", qPrintable(m_source->errorString()));
        return;
    }
    m_data->item = obj;
    m_data->item->setParent(this);

    QQuickItem* item = qobject_cast<QQuickItem*>(obj);
    if ( item ){
        item->setParentItem(this);
    }
    qmlEngine(this)->setObjectOwnership(m_data->item, QQmlEngine::CppOwnership);

    emit itemCreated();
}

}// namespace

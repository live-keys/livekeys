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

#include "qstaticloader.h"
#include "qstaticcontainer.h"
#include <QQmlEngine>


class QStaticLoaderItem{
public:
    QStaticLoaderItem() : item(0){}
    ~QStaticLoaderItem(){
        if ( item )
            delete item;
    }

    QObject* item;
};

QStaticLoader::QStaticLoader(QQuickItem *parent)
    : QQuickItem(parent)
    , m_source(0)
    , m_data(0)
{
}

QStaticLoader::~QStaticLoader(){
    if ( m_data ){
        m_data->item->setParent(0);
        QQuickItem* item = qobject_cast<QQuickItem*>(m_data->item);
        if ( item )
            item->setParentItem(0);
    }
}


QObject *QStaticLoader::item() const{
    if ( m_data )
        return m_data->item;
    return 0;
}

void QStaticLoader::setSource(QQmlComponent *arg){
    if ( m_source == arg )
        return;

    m_source  = arg;
    emit sourceChanged();

    if ( m_source && m_data && !m_data->item ){
        createObject();
    }
}

void QStaticLoader::staticLoad(const QString &id){
    QStaticContainer* container = QStaticContainer::grabFromContext(this);
    m_data = container->get<QStaticLoaderItem>(id);
    if ( !m_data ){
        m_data = new QStaticLoaderItem;
        container->set<QStaticLoaderItem>(id, m_data);

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

void QStaticLoader::createObject(){
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


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

#include "qmlcomponentmapdata.h"
#include "qmlcomponentmap.h"

namespace lv{

QmlComponentMapData::QmlComponentMapData(QmlComponentMap *parent)
    : QObject(parent)
    , m_index(0)
{

}

void QmlComponentMapData::setCurrent(int index, const QVariant &value){
    m_index   = index;
    m_current = value;
    emit currentChanged();
}

QmlComponentMap *QmlComponentMapData::mapArray(){
    return qobject_cast<QmlComponentMap*>(parent());
}

int QmlComponentMapData::index() const{
    return m_index;
}

void QmlComponentMapData::result(const QVariant &value){
    QmlComponentMap* p = mapArray();
    p->assignResult(this, value);
}

}// namespace

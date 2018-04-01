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

#include "visuallogbasemodel.h"

namespace lv{

VisualLogEntry::VisualLogEntry(const QString &ptag, const QString &pprefix, const QString &p)
    : prefix(pprefix)
    , tag(ptag)
    , data(p)
    , objectData(0)
    , component(0)
    , context(0)
{
}

VisualLogEntry::VisualLogEntry(const QString &ptag, const QString &pprefix, QVariant *od, QQmlComponent *c)
    : prefix(pprefix)
    , tag(ptag)
    , objectData(od)
    , component(c)
    , context(0)
{
}

VisualLogBaseModel::VisualLogBaseModel(QObject *parent)
    : QAbstractListModel(parent){

}

VisualLogBaseModel::~VisualLogBaseModel(){
}

QHash<int, QByteArray> VisualLogBaseModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[VisualLogBaseModel::Msg]    = "msg";
    roles[VisualLogBaseModel::Prefix] = "prefix";
    return roles;
}

}// namespace

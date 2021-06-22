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

#include "visuallogbasemodel.h"

namespace lv{

/**
 * \class lv::VisualLogEntry
 * \brief Struct-like class for storing relevant data about a log entry
 *
 * \ingroup lvview
 */

/** Constructor for string-type log entries */
VisualLogEntry::VisualLogEntry(const QString &ptag, quint8 lvl, const QString &pprefix, const QString& loc, const QString &p)
    : prefix(pprefix)
    , tag(ptag)
    , level(lvl)
    , location(loc)
    , data(p)
    , objectData(0)
    , component(0)
    , context(0)
{
}

/** Constructor for objects and/or image log entries */
VisualLogEntry::VisualLogEntry(const QString &ptag, quint8 lvl, const QString &pprefix, const QString &loc, QVariant *od, QQmlComponent *c)
    : prefix(pprefix)
    , tag(ptag)
    , level(lvl)
    , location(loc)
    , objectData(od)
    , component(c)
    , context(0)
{
}

/**
 * \class lv::VisualLogBaseModel
 * \brief Abstract model of a set of visual log entries, to be extended for concrete usage by the VisualLogModel and VisualLogFilter
 * \ingroup lvview
 */

/** Default constructor */
VisualLogBaseModel::VisualLogBaseModel(QObject *parent)
    : QAbstractListModel(parent){

}

/** Default destructor */
VisualLogBaseModel::~VisualLogBaseModel(){
}

QHash<int, QByteArray> VisualLogBaseModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[VisualLogBaseModel::Msg]      = "msg";
    roles[VisualLogBaseModel::Prefix]   = "prefix";
    roles[VisualLogBaseModel::Location] = "location";
    return roles;
}

}// namespace

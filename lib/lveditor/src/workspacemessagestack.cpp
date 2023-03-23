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

#include "workspacemessagestack.h"
#include "live/qmlerror.h"
#include "live/visuallogqt.h"
#include <QDebug>

namespace lv {

WorkspaceMessageStack::WorkspaceMessageStack(ViewEngine *engine, QObject* parent)
    : QAbstractListModel(parent)
    , m_engine(engine)
    , m_logEnabled(true)
{
    m_roles[Message]     = "message";
    m_roles[Code]        = "code";
    m_roles[MessageType] = "type";
}

QVariant WorkspaceMessageStack::data(const QModelIndex &index, int role) const{
    int row = index.row();
    if ( row >= m_messages.size() )
        return QVariant();

    if ( role == Message ){
        return m_messages[row].message;
    } else if ( role == Code ){
        return m_messages[row].code;
    } else if ( role == MessageType ){
        return m_messages[row].type;
    }
    return QVariant();
}

int WorkspaceMessageStack::rowCount(const QModelIndex &) const{
    return static_cast<int>(m_messages.size());
}

QHash<int, QByteArray> WorkspaceMessageStack::roleNames() const{
    return m_roles;
}

void WorkspaceMessageStack::pushInfo(const QString &message, int code){
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.push_back(WorkspaceMessage(WorkspaceMessageStack::Info, message, code));
    endInsertRows();

    if ( m_logEnabled ){
        vlog("workspace").i() << message;
    }

    emit messageAdded(WorkspaceMessageStack::Info, message, code);
    emit countChanged();

}

void WorkspaceMessageStack::pushWarning(const QString &message, int code){
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.push_back(WorkspaceMessage(WorkspaceMessageStack::Warning, message, code));
    endInsertRows();

    if ( m_logEnabled ){
        vlog("workspace").w() << message;
    }

    emit messageAdded(WorkspaceMessageStack::Warning, message, code);
    emit countChanged();
}

void WorkspaceMessageStack::pushError(const QString &message, int code){
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.push_back(WorkspaceMessage(WorkspaceMessageStack::Error, message, code));
    endInsertRows();

    if ( m_logEnabled ){
        vlog("workspace").e() << message;
    }

    emit messageAdded(WorkspaceMessageStack::Error, message, code);
    emit countChanged();

}

void WorkspaceMessageStack::pushErrorObject(const QJSValue &e){
    QmlError qe(m_engine, e);
    pushError(qe.toString(), static_cast<int>(qe.code()));
}

void WorkspaceMessageStack::removeAt(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);
    m_messages.erase(m_messages.begin() + idx);
    endRemoveRows();
    emit countChanged();

}

void WorkspaceMessageStack::clear()
{
    if (m_messages.size() == 0) return;
    beginRemoveRows(QModelIndex(), 0, m_messages.size() - 1);
    m_messages.clear();
    endRemoveRows();
    emit countChanged();

}

int WorkspaceMessageStack::count(){
    return m_messages.size();
}


}

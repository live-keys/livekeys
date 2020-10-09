#include "workspacemessagestack.h"
#include <QDebug>

namespace lv {

WorkspaceMessageStack::WorkspaceMessageStack(QObject* parent)
    : QAbstractListModel(parent)
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

    emit messageAdded(WorkspaceMessageStack::Info, message, code);
}

void WorkspaceMessageStack::pushWarning(const QString &message, int code){
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.push_back(WorkspaceMessage(WorkspaceMessageStack::Warning, message, code));
    endInsertRows();

    emit messageAdded(WorkspaceMessageStack::Warning, message, code);
}

void WorkspaceMessageStack::pushError(const QString &message, int code){
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.push_back(WorkspaceMessage(WorkspaceMessageStack::Error, message, code));
    endInsertRows();

    emit messageAdded(WorkspaceMessageStack::Error, message, code);
}

void WorkspaceMessageStack::removeAt(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);
    m_messages.erase(m_messages.begin() + idx);
    endRemoveRows();

}

void WorkspaceMessageStack::clear()
{
    if (m_messages.size() == 0) return;
    beginRemoveRows(QModelIndex(), 0, m_messages.size() - 1);
    m_messages.clear();
    endRemoveRows();
}


}

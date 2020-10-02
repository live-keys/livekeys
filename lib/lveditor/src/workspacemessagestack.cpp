#include "workspacemessagestack.h"
#include <QDebug>

namespace lv {

WorkspaceMessageStack::WorkspaceMessageStack(QObject* parent) : QAbstractListModel(parent)
{
    m_roles[Message] = "message";
    m_roles[Code]    = "code";
}

QVariant WorkspaceMessageStack::data(const QModelIndex &index, int role) const
{
    unsigned row = static_cast<unsigned>(index.row());

    if ( row < m_messages.size() ){
        if ( role == Message ){
            return m_messages[row].message;
        } else if ( role == Code ){
            return m_messages[row].code;
        }
    }
    return QVariant();
}

int WorkspaceMessageStack::rowCount(const QModelIndex &) const
{
    return static_cast<int>(m_messages.size());
}

QHash<int, QByteArray> WorkspaceMessageStack::roleNames() const
{
    return m_roles;
}

void WorkspaceMessageStack::pushMessage(QString m, int c)
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.push_back(WorkspaceMessage(m, c));
    endInsertRows();
}

void WorkspaceMessageStack::removeAtIndex(int idx)
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

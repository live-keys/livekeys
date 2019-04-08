#include "qmlobjectlistmodel.h"
#include "qmlobjectlist.h"

namespace lv{

QmlObjectListModel::QmlObjectListModel(QmlObjectList* model):m_qmlObjectList(model){}

QVariant QmlObjectListModel::data(const QModelIndex &index, int role) const
{
    if ( index.row() >= m_qmlObjectList->itemCount() )
        return QVariant();
    if ( role == Qt::UserRole )
        return QVariant::fromValue(m_qmlObjectList->itemAt(index.row()));
    return QVariant();
}

int QmlObjectListModel::rowCount(const QModelIndex &) const
{
    return m_qmlObjectList->itemCount();
}

QHash<int, QByteArray> QmlObjectListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "item";
    return roles;
}

void QmlObjectListModel::appendData(QObject* data)
{
    beginInsertRows(QModelIndex(), m_qmlObjectList->itemCount(), m_qmlObjectList->itemCount());
    m_qmlObjectList->appendItem(data);
    endInsertRows();
}

void QmlObjectListModel::removeAt(int index)
{
    if ( index == -1 || index >= m_qmlObjectList->itemCount() )
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_qmlObjectList->removeItemAt(index);
    endRemoveRows();
}

QObject* QmlObjectListModel::at(int index)
{
    if ( index < m_qmlObjectList->itemCount() )
        return m_qmlObjectList->itemAt(index);
    return nullptr;
}

int QmlObjectListModel::size() const
{
    return m_qmlObjectList->itemCount();
}

}


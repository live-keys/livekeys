#include "qmlvariantlistmodel.h"
namespace lv{

QmlVariantListModel::QmlVariantListModel(QmlVariantList* model):m_qmlVariantList(model){}

QVariant QmlVariantListModel::data(const QModelIndex &index, int role) const
{
    if ( index.row() >= m_qmlVariantList->itemCount() )
        return QVariant();
    if ( role == Qt::UserRole )
        return m_qmlVariantList->itemAt(index.row());
    return QVariant();
}

int QmlVariantListModel::rowCount(const QModelIndex &parent) const
{
    return m_qmlVariantList->itemCount();
}

QHash<int, QByteArray> QmlVariantListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "item";
    return roles;
}

void QmlVariantListModel::appendData(const QVariant &data)
{
    beginInsertRows(QModelIndex(), m_qmlVariantList->itemCount(), m_qmlVariantList->itemCount());
    m_qmlVariantList->appendItem(data);
    endInsertRows();
}

void QmlVariantListModel::removeAt(int index)
{
    if ( index == -1 || index >= m_qmlVariantList->itemCount() )
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_qmlVariantList->removeItemAt(index);
    endRemoveRows();
}

QVariant QmlVariantListModel::at(int index)
{
    if ( index < m_qmlVariantList->itemCount() )
        return m_qmlVariantList->itemAt(index);
    return QVariant();
}

int QmlVariantListModel::size() const
{
    return m_qmlVariantList->itemCount();
}

}

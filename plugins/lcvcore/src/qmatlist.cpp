#include "qmatlist.h"


QMatList::QMatList(QObject *parent)
     : QAbstractListModel(parent)
{
}

QMatList::~QMatList(){
}

QVariant QMatList::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_list.size() )
        return QVariant();
    if ( role == Qt::UserRole )
        return QVariant::fromValue(m_list[index.row()]);
    return QVariant();
}

QHash<int, QByteArray> QMatList::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "mat";
    return roles;
}

void QMatList::appendMat(QMat *mat){
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(mat);
    endInsertRows();
}

void QMatList::removeMat(QMat *mat){
    if ( !mat )
        return;
    removeAt(m_list.indexOf(mat));
}

void QMatList::removeAt(int index){
    if ( index == -1 || index >= m_list.size() )
        return;
    beginRemoveRows(QModelIndex(), index, index);
    m_list.removeAt(index);
    endRemoveRows();
}

QMat *QMatList::at(int index){
    if ( index < m_list.size() )
        return m_list.at(index);
    return 0;
}

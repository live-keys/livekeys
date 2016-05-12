/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#include "qmatlist.h"


QMatList::QMatList(QObject *parent)
     : QAbstractListModel(parent)
{
}

QMatList::~QMatList(){
    for ( int i = 0; i < m_list.size(); ++i )
        delete m_list[i];
    m_list.clear();
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
    cv::Mat* nmat = new cv::Mat(*mat->cvMat());
    m_list.append(new QMat(nmat));
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

int QMatList::size() const{
    return m_list.size();
}

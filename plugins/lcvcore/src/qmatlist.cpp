/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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
#include <QQmlEngine>
#include <QJSValueIterator>

//TODO: Document

QMatList::QMatList(QObject *parent)
     : QAbstractListModel(parent)
{
}

QMatList::~QMatList(){
    clearList();
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

void QMatList::resize(int newSize){
    if ( newSize < m_list.size() ){
        for ( int i = newSize; i < m_list.size(); ++i ){
            delete m_list[i];
        }
        m_list.erase(m_list.begin() + newSize, m_list.end());
    } else {
        m_list.reserve(newSize);
        for ( int i = m_list.size(); i < newSize; ++i ){
            m_list.append(new QMat);
        }
    }
}

std::vector<cv::Mat> QMatList::asVector(){
    std::vector<cv::Mat> base;
    for ( auto it = m_list.begin(); it != m_list.end(); ++it ){
        QMat* m = *it;
        base.push_back(m->data());
    }

    return base;
}

void QMatList::fromVector(const std::vector<cv::Mat> &v){
    resize((int)v.size());
    for ( int i = 0; i < (int)v.size(); ++i ){
        *m_list[i]->cvMat() = v[i];
    }
}

void QMatList::appendMat(QMat *mat){
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    cv::Mat* nmat = new cv::Mat(*mat->cvMat());
    m_list.append(new QMat(nmat));
    endInsertRows();

    emit entriesAdded();
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
    delete m_list[index];
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

void QMatList::fromArray(const QJSValue &matArray){
    beginResetModel();

    bool entriesWereAdded = false;
    if ( m_list.size() > 0 ){
        entriesWereAdded = true;
        clearList();
    }

    if ( matArray.isArray() ){
        QJSValueIterator it(matArray);
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                QMat* m = qobject_cast<QMat*>(it.value().toQObject());
                if ( m ){
                    cv::Mat* nmat = new cv::Mat(*m->cvMat());
                    m_list.append(new QMat(nmat));
                } else {
                    clearList();
                    break;
                }
            }
        }
    }

    if ( m_list.size() > 0 )
        entriesWereAdded = true;

    endResetModel();

    if ( entriesWereAdded )
        emit entriesAdded();
}

void QMatList::clearList(){
    for( auto it = m_list.begin(); it != m_list.end(); ++it )
        delete *it;
    m_list.clear();
}

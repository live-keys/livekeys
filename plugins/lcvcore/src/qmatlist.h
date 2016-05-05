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

#ifndef QMATLIST_H
#define QMATLIST_H

#include <QAbstractListModel>
#include "qmat.h"

class QMatList : public QAbstractListModel{

    Q_OBJECT

public:
    explicit QMatList(QObject* parent = 0);
    ~QMatList();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

public slots:
    void appendMat(QMat* mat);
    void removeMat(QMat* mat);
    void removeAt(int index);
    QMat* at(int index);
    int size() const;

private:
    QList<QMat*> m_list;

};


inline int QMatList::rowCount(const QModelIndex &) const{
    return m_list.size();
}

#endif // QMATLIST_H

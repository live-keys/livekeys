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

#ifndef QMATACCESS_H
#define QMATACCESS_H

#include <QQuickItem>
#include "qmat.h"
#include "qlcvcoreglobal.h"

#include <QAbstractListModel>

class Q_LCVCORE_EXPORT QMatAccess : public QQuickItem{

    Q_OBJECT

public:
    explicit QMatAccess(QQuickItem* parent = 0);
    QMatAccess(QMat* mat, QQuickItem *parent = 0);
    ~QMatAccess();

public slots:
    int rows() const;
    int cols() const;
    int channels() const;

    int depth() const;

    QVariantList values();
    void setValues(QVariantList values);

private:
    void setupCache();

    QMat* m_mat;

    uchar* m_dataPtr;
    int    m_typeStep;
    int    m_channels;

    QVariantList  m_values;

};

inline int QMatAccess::cols() const{
    return m_mat->cvMat()->cols;
}

inline int QMatAccess::rows() const{
    return m_mat->cvMat()->rows;
}

inline int QMatAccess::channels() const{
    return m_channels;
}

inline int QMatAccess::depth() const{
    return m_mat->cvMat()->depth();
}

#endif // QMATACCESS_H

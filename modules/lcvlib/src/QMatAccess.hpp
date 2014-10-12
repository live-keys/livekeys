/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/
#ifndef QMATACCESS_HPP
#define QMATACCESS_HPP

#include <QQuickItem>
#include "QMat.hpp"
#include "QLCVGlobal.hpp"

#include <QAbstractListModel>

class Q_LCV_EXPORT QMatAccess : public QQuickItem{

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
    void setUpCache();


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

#endif // QMATACCESS_HPP

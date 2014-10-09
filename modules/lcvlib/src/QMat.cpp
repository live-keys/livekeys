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

#include "QMat.hpp"
#include "QMatAccess.hpp"

/**
 * @brief QMat::QMat
 * @param parent
 */
QMat::QMat(QQuickItem *parent):
    QQuickItem(parent),
    m_cvmat(new cv::Mat){
}

/**
 * @brief QMat::QMat
 * @param mat
 * @param parent
 */
QMat::QMat(cv::Mat *mat, QQuickItem *parent):
    QQuickItem(parent),
    m_cvmat(mat){
}

/**
 * @brief Returns a mat pixel values access object
 * @return
 */
QMatAccess *QMat::data(){
    return new QMatAccess(this);
}

/**
 * @brief QMat::dataSize
 * @return
 */
QSize QMat::dataSize() const{
    return QSize(m_cvmat->cols, m_cvmat->rows);
}

/**
 * @brief QMat::~QMat
 */
QMat::~QMat(){
    delete m_cvmat;
}

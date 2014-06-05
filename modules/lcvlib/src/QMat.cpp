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

QMat::QMat(QQuickItem *parent):
    QQuickItem(parent),
    m_data(new cv::Mat){
}

QMat::QMat(cv::Mat *mat, QQuickItem *parent):
    QQuickItem(parent),
    m_data(mat){
}

QMat::~QMat(){
    delete m_data;
}

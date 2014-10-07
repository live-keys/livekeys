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

#include "QMatFilter.hpp"

/**
 * @brief QMatFilter::QMatFilter
 * @param parent
 */
QMatFilter::QMatFilter(QQuickItem *parent) :
    QMatDisplay(parent),
    m_in(new QMat(this)),
    m_inInternal(m_in)
{
}

/**
 * @brief QMatFilter::~QMatFilter
 */
QMatFilter::~QMatFilter(){
    delete m_inInternal;
}

/**
 * @brief QMatFilter::transform
 */
void QMatFilter::transform(cv::Mat&, cv::Mat&){
}

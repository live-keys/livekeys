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

#ifndef QMAT_HPP
#define QMAT_HPP

#include <QQuickItem>
#include "opencv2/core/core.hpp"
#include "QLCVGlobal.hpp"

class Q_LCV_EXPORT QMat : public QQuickItem{

    Q_OBJECT
    Q_ENUMS(Type)

public:
    enum Type{
        CV8U  = CV_8U,
        CV8S  = CV_8S,
        CV16U = CV_16U,
        CV16S = CV_16S,
        CV32S = CV_32S,
        CV32F = CV_32F,
        CV64F = CV_64F
    };

public:
    explicit QMat(QQuickItem *parent = 0);
    QMat(cv::Mat *mat, QQuickItem *parent = 0);
    ~QMat();

    cv::Mat* data();

public:
    cv::Mat* m_data;
    
};

inline cv::Mat *QMat::data(){
    return m_data;
}

#endif // QMAT_HPP

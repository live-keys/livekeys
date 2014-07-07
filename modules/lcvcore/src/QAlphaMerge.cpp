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
#include "QAlphaMerge.hpp"

QAlphaMerge::QAlphaMerge(QQuickItem *parent) :
    QMatFilter(parent),
    m_mask(0)
{
}

void QAlphaMerge::transform(cv::Mat &in, cv::Mat &out){
    if ( mask() )
        mergeMask(in, *mask()->cvMat(), out);
}

void QAlphaMerge::mergeMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output){
    if ( input.cols != mask.cols || input.rows != mask.rows ){
        qDebug() << "opencv error : different mask size";
        return;
    }
    output.create(input.size(), CV_8UC4);
    uchar* pi, *pm, *po;
    if ( input.channels() == 1 ){
        for ( int i = 0; i < input.rows; ++i ){
            pi = input.ptr<uchar>(i);
            pm = mask.ptr<uchar>(i);
            po = output.ptr<uchar>(i);
            for ( int j = 0; j < input.cols; ++j ){
                po[j * 4 + 0] = pi[j];
                po[j * 4 + 1] = pi[j];
                po[j * 4 + 2] = pi[j];
                po[j * 4 + 3] = pm[j];
            }
        }
    } else {
        for ( int i = 0; i < input.rows; ++i ){
            pi = input.ptr<uchar>(i);
            pm = mask.ptr<uchar>(i);
            po = output.ptr<uchar>(i);
            for ( int j = 0; j < input.cols; ++j ){
                po[j * 4 + 0] = pi[j * 3 + 0];
                po[j * 4 + 1] = pi[j * 3 + 1];
                po[j * 4 + 2] = pi[j * 3 + 2];
                po[j * 4 + 3] = pm[j];
            }
        }
    }
}

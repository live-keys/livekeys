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

#include "qimwrite.h"
#include "opencv2/highgui.hpp"
#include <QJSValueIterator>

//TODO: Document

QImWrite::QImWrite(QObject *parent)
    : QObject(parent)
{
}

QImWrite::~QImWrite(){
}

bool QImWrite::saveImage(const QString &file, QMat *image){
    if (!image)
        return false;

    try{
        return cv::imwrite(file.toStdString(), *image->cvMat(), m_convertedParams);
    } catch ( cv::Exception& e ){
        qWarning("Failed to save image due to exception: %s", e.what());
        return false;
    }
}

void QImWrite::setParams(const QJSValue &params){
    m_convertedParams.clear();
    if ( params.isObject() ){
        QJSValueIterator paramsIt(params);
        while( paramsIt.hasNext() ){
            paramsIt.next();
            if ( paramsIt.name() == "jpegQuality" ){
                m_convertedParams.push_back(CV_IMWRITE_JPEG_QUALITY);
                m_convertedParams.push_back(paramsIt.value().toInt());
            } else if ( paramsIt.name() == "pngCompression" ){
                m_convertedParams.push_back(CV_IMWRITE_PNG_COMPRESSION);
                m_convertedParams.push_back(paramsIt.value().toInt());
            } else if ( paramsIt.name() == "pxmBinary" ){
                m_convertedParams.push_back(CV_IMWRITE_PXM_BINARY);
                m_convertedParams.push_back(paramsIt.value().toInt());
            }
        }
    }

    m_params = params;
    emit paramsChanged();
}

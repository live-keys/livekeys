/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#ifndef QMATFILTER_H
#define QMATFILTER_H

#include "qlcvcoreglobal.h"
#include "qmatdisplay.h"

class Q_LCVCORE_EXPORT QMatFilter : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMat* input READ inputMat WRITE setInputMat NOTIFY inputChanged)

public:
    explicit QMatFilter(QQuickItem *parent = nullptr);
    virtual ~QMatFilter();
    
    QMat* inputMat();
    void setInputMat(QMat* mat);

    void transform();
    virtual void transform(const cv::Mat& in, cv::Mat& out);

signals:
    void inputChanged();

protected:
    void componentComplete();

private:
    QMat* m_in;
    
};

inline QMat *QMatFilter::inputMat(){
    return m_in;
}

inline void QMatFilter::setInputMat(QMat *mat){
    if ( mat == nullptr )
        return;

    cv::Mat* matData = mat->internalPtr();
    if ( implicitWidth() != matData->cols || implicitHeight() != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_in = mat;
    emit inputChanged();
    transform();
}

#endif // QMATFILTER_H

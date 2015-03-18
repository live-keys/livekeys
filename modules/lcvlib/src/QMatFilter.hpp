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

#ifndef QMATTRANSFORMATION_HPP
#define QMATTRANSFORMATION_HPP

#include "QMatDisplay.hpp"

class Q_LCV_EXPORT QMatFilter : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMat* input READ inputMat WRITE setInputMat NOTIFY inputChanged)

public:
    explicit QMatFilter(QQuickItem *parent = 0);
    virtual ~QMatFilter();
    
    QMat* inputMat();
    void setInputMat(QMat* mat);

    void transform();
    virtual void transform(cv::Mat& in, cv::Mat& out);

signals:
    void inputChanged();

private:
    QMat* m_in;
    
};

inline QMat *QMatFilter::inputMat(){
    return m_in;
}

inline void QMatFilter::setInputMat(QMat *mat){
    cv::Mat* matData = mat->cvMat();
    if ( implicitWidth() != matData->cols || implicitHeight() != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_in = mat;
    emit inputChanged();
    transform();
}

inline void QMatFilter::transform(){
    try{
        transform(*inputMat()->cvMat(), *output()->cvMat());
    } catch (cv::Exception& e ){
        qCritical(e.msg.c_str());
    }
    emit outputChanged();
    update();
}

#endif // QMATTRANSFORMATION_HPP

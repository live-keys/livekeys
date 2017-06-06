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

#ifndef QSTRUCTURINGELEMENT_H
#define QSTRUCTURINGELEMENT_H

#include <QQuickItem>
#include "qmat.h"
#include "opencv2/imgproc.hpp"

class QStructuringElement : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(ElementShape shape  READ shape  WRITE setShape  NOTIFY shapeChanged)
    Q_PROPERTY(QSize        ksize  READ ksize  WRITE setKsize  NOTIFY ksizeChanged)
    Q_PROPERTY(QPoint       anchor READ anchor WRITE setAnchor NOTIFY anchorChanged)
    Q_PROPERTY(QMat*        output READ output WRITE setOutput NOTIFY outputChanged)

    Q_ENUMS(ElementShape)

public:
    enum ElementShape{
        MORPH_RECT    = cv::MORPH_RECT,
        MORPH_ELLIPSE = cv::MORPH_ELLIPSE,
        MORPH_CROSS   = cv::MORPH_CROSS
    };

    QStructuringElement(QQuickItem* parent = 0);
    ~QStructuringElement();

    ElementShape shape() const;
    void setShape(ElementShape shape);

    const QSize& ksize() const;
    void setKsize(const QSize& ksize);

    const QPoint& anchor() const;
    void setAnchor(const QPoint& anchor);

    QMat* output() const;
    void setOutput(QMat* output);

signals:
    void ksizeChanged();
    void shapeChanged();
    void anchorChanged();
    void outputChanged();

private:
    void setOutputDirty();

private:
    ElementShape m_shape;
    QPoint       m_anchor;
    QSize        m_ksize;
    QMat*        m_output;
    bool         m_outputDirty;
};

inline void QStructuringElement::setOutputDirty(){
    m_outputDirty = true;
    emit outputChanged();
}

inline void QStructuringElement::setOutput(QMat* output){
	if (m_output != output){
        m_output = output;
		emit outputChanged();
	}
}

inline const QPoint& QStructuringElement::anchor() const{
	return m_anchor;
}

inline void QStructuringElement::setAnchor(const QPoint& anchor){
	if (m_anchor != anchor){
		m_anchor = anchor;
        emit anchorChanged();
        setOutputDirty();
	}
}

inline const QSize& QStructuringElement::ksize() const{
    return m_ksize;
}

inline void QStructuringElement::setKsize(const QSize& ksize){
    if (m_ksize != ksize){
        m_ksize = ksize;
        emit ksizeChanged();
        setOutputDirty();
	}
}

inline QStructuringElement::ElementShape QStructuringElement::shape() const{
	return m_shape;
}

inline void QStructuringElement::setShape(ElementShape shape){
	if (m_shape != shape){
		m_shape = shape;
		emit shapeChanged();
        setOutputDirty();
	}
}


#endif // QSTRUCTURINGELEMENT_H

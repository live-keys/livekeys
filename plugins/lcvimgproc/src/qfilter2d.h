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

#ifndef QFILTER2D_H
#define QFILTER2D_H

#include "qmatfilter.h"

class QFilter2D : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(int    ddepth     READ ddepth     WRITE setDdepth     NOTIFY ddepthChanged)
    Q_PROPERTY(QMat*  kernel     READ kernel     WRITE setKernel     NOTIFY kernelChanged)
    Q_PROPERTY(QPoint anchor     READ anchor     WRITE setAnchor     NOTIFY anchorChanged)
    Q_PROPERTY(double delta      READ delta      WRITE setDelta      NOTIFY deltaChanged)
    Q_PROPERTY(int    borderType READ borderType WRITE setBorderType NOTIFY borderTypeChanged)

public:
    explicit QFilter2D(QQuickItem *parent = 0);
    ~QFilter2D();

    void transform(const cv::Mat& in, cv::Mat& out);

signals:
    void ddepthChanged();
    void kernelChanged();
    void anchorChanged();
    void deltaChanged();
    void borderTypeChanged();

public:
	int ddepth() const;
	void setDdepth(int ddepth);

	QMat* kernel() const;
	void setKernel(QMat* kernel);

    const QPoint& anchor() const;
    void setAnchor(const QPoint& anchor);

	double delta() const;
	void setDelta(double delta);

	int borderType() const;
	void setBorderType(int borderType);

private:
    QMat*     m_kernel;
    int       m_ddepth;
    QPoint    m_anchor;
    cv::Point m_anchorCv;
    double    m_delta;
    int       m_borderType;

};

inline int QFilter2D::borderType() const{
	return m_borderType;
}

inline void QFilter2D::setBorderType(int borderType){
	if (m_borderType != borderType){
		m_borderType = borderType;
		emit borderTypeChanged();
        QMatFilter::transform();
	}
}

inline double QFilter2D::delta() const{
	return m_delta;
}

inline void QFilter2D::setDelta(double delta){
	if (m_delta != delta){
		m_delta = delta;
		emit deltaChanged();
        QMatFilter::transform();
	}
}

inline const QPoint& QFilter2D::anchor() const{
	return m_anchor;
}

inline void QFilter2D::setAnchor(const QPoint& anchor){
	if (m_anchor != anchor){
		m_anchor = anchor;
        m_anchorCv = cv::Point(anchor.x(), anchor.y());
		emit anchorChanged();
        QMatFilter::transform();
	}
}

inline QMat* QFilter2D::kernel() const{
	return m_kernel;
}

inline void QFilter2D::setKernel(QMat*kernel){
    m_kernel = kernel;
    emit kernelChanged();
    QMatFilter::transform();
}

inline int QFilter2D::ddepth() const{
	return m_ddepth;
}

inline void QFilter2D::setDdepth(int ddepth){
	if (m_ddepth != ddepth){
		m_ddepth = ddepth;
		emit ddepthChanged();
        QMatFilter::transform();
	}
}


#endif // QFILTER2D_H

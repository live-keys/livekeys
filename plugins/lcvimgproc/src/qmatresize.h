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

#ifndef QMATRESIZE_H
#define QMATRESIZE_H

#include "qmatfilter.h"
#include "opencv2/imgproc/types_c.h"

class QMatResize : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QSize matSize       READ matSize       WRITE setMatSize       NOTIFY matSizeChanged)
    Q_PROPERTY(qreal fx            READ fx            WRITE setFx            NOTIFY fxChanged)
    Q_PROPERTY(qreal fy            READ fy            WRITE setFy            NOTIFY fyChanged)
    Q_PROPERTY(int   interpolation READ interpolation WRITE setInterpolation NOTIFY interpolationChanged)
    Q_ENUMS(Interpolation)

public:
    enum Interpolation{
        INTER_NEAREST    = CV_INTER_NN, //!< nearest neighbor interpolation
        INTER_LINEAR     = CV_INTER_LINEAR, //!< bilinear interpolation
        INTER_CUBIC      = CV_INTER_CUBIC, //!< bicubic interpolation
        INTER_AREA       = CV_INTER_AREA, //!< area-based (or super) interpolation
        INTER_LANCZOS4   = CV_INTER_LANCZOS4, //!< Lanczos interpolation over 8x8 neighborhood
        INTER_MAX        = 7,
        WARP_INVERSE_MAP = CV_WARP_INVERSE_MAP
    };

public:
    explicit QMatResize(QQuickItem *parent = 0);
    ~QMatResize();

    QSize matSize() const;
    void setMatSize(const QSize& matSize);

    qreal fx() const;
    void setFx(qreal fx);

    qreal fy() const;
    void setFy(qreal fy);

    int interpolation() const;
    void setInterpolation(int interpolation);

    void transform(const cv::Mat& in, cv::Mat& out);

signals:
    void matSizeChanged();
    void fxChanged();
    void fyChanged();
    void interpolationChanged();

private:
    cv::Size m_matSize;
    qreal    m_fx;
    qreal    m_fy;
    int      m_interpolation;

};

inline int QMatResize::interpolation() const{
	return m_interpolation;
}

inline void QMatResize::setInterpolation(int interpolation){
	if (m_interpolation != interpolation){
		m_interpolation = interpolation;
        emit interpolationChanged();
        QMatFilter::transform();
    }
}

inline qreal QMatResize::fy() const{
	return m_fy;
}

inline void QMatResize::setFy(qreal fy){
	if (m_fy != fy){
		m_fy = fy;
        emit fyChanged();
        QMatFilter::transform();
	}
}

inline qreal QMatResize::fx() const{
	return m_fx;
}

inline void QMatResize::setFx(qreal fx){
	if (m_fx != fx){
		m_fx = fx;
        emit fxChanged();
        QMatFilter::transform();
	}
}

inline QSize QMatResize::matSize() const{
    return QSize(m_matSize.width, m_matSize.height);
}

inline void QMatResize::setMatSize(const QSize& matSize){
    if ( m_matSize.width != matSize.width() || m_matSize.height != matSize.height() ){
        m_matSize = cv::Size(matSize.width(), matSize.height());
        emit matSizeChanged();
        QMatFilter::transform();
    }
}


#endif // QMATRESIZE_H

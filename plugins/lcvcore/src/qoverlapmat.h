/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef QOVERLAPMAT_H
#define QOVERLAPMAT_H

#include <QQuickItem>
#include "qmatfilter.h"

/// \private
class QOverlapMat : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat* input2 READ input2 WRITE setInput2 NOTIFY input2Changed)
    Q_PROPERTY(QMat* mask   READ mask   WRITE setMask   NOTIFY maskChanged)

public:
    explicit QOverlapMat(QQuickItem* parent = 0);
    virtual ~QOverlapMat(){}

    QMat* mask();
    void setMask(QMat* m);

    QMat* input2();
    void setInput2(QMat* m);

    void transform(const cv::Mat& in, cv::Mat& out);

signals:
    void maskChanged();
    void input2Changed();

private:
    QMat* m_input2;
    QMat* m_mask;
};

inline QMat* QOverlapMat::mask(){
    return m_mask;
}

inline void QOverlapMat::setMask(QMat *m){
    if ( m == 0 )
        return;

    m_mask = m;
    emit maskChanged();

    QMatFilter::transform();
}

inline QMat *QOverlapMat::input2(){
    return m_input2;
}

inline void QOverlapMat::setInput2(QMat *m){
    if ( m == 0 )
        return;

    m_input2 = m;
    emit input2Changed();

    QMatFilter::transform();
}

#endif // QOVERLAPMAT_H

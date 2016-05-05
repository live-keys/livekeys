/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#ifndef QABSDIFF_H
#define QABSDIFF_H

#include "qmatfilter.h"

class QAbsDiff : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat* input2 READ input2 WRITE setInput2 NOTIFY input2Changed)

public:
    explicit QAbsDiff(QQuickItem *parent = 0);
    virtual ~QAbsDiff();

    QMat* input2();
    void setInput2(QMat* m);

    void transform(cv::Mat& in, cv::Mat& out);

signals:
    void input2Changed();

private:
    QMat* m_in2;

};

inline QMat *QAbsDiff::input2(){
    return m_in2;
}

inline void QAbsDiff::setInput2(QMat *mat){
    if ( mat == 0 )
        return;

    m_in2 = mat;
    emit input2Changed();
    if ( m_in2->cvMat()->size() == inputMat()->cvMat()->size() )
        QMatFilter::transform();
}

#endif // QABSDIFF_H

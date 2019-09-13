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
#ifndef QALPHAMERGE_H
#define QALPHAMERGE_H

#include "qmatfilter.h"
/// \private
class QAlphaMerge : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat* mask READ mask WRITE setMask NOTIFY maskChanged)

public:
    explicit QAlphaMerge(QQuickItem *parent = 0);

    QMat* mask();
    void setMask(QMat* mask);

    virtual void transform(const cv::Mat &in, cv::Mat &out);
    static void mergeMask(const cv::Mat &input, cv::Mat& mask, cv::Mat& output);

signals:
    void maskChanged();

private:
    QMat* m_mask;

};

/*!
 * \fn QAlphaMerge::mask
 * \return
 */
inline QMat *QAlphaMerge::mask(){
    return m_mask;
}

inline void QAlphaMerge::setMask(QMat *mask){
    m_mask = mask;
    emit inputChanged();
    QMatFilter::transform();
}

#endif // QALPHAMERGE_H

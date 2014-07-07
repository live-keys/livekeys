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
#ifndef QALPHAMERGE_HPP
#define QALPHAMERGE_HPP

#include "QMatFilter.hpp"

/**
 * @brief The QAlphaMerge class
 *
 * Merges according to an alpha channel
 *
 * @code
 * QAlphaMerge::maskChanged();
 * QAlphaMerge a;
 * a.transform(cv::Mat(), cv::Mat());
 * @endcode
 */
class QAlphaMerge : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat* mask READ mask WRITE setMask NOTIFY maskChanged)

public:
    explicit QAlphaMerge(QQuickItem *parent = 0);

    QMat* mask();
    void setMask(QMat* mask);

    virtual void transform(cv::Mat &in, cv::Mat &out);
    static void mergeMask( cv::Mat& input, cv::Mat& mask, cv::Mat& output);

signals:
    void maskChanged();

private:
    QMat* m_mask;

};

inline QMat *QAlphaMerge::mask(){
    return m_mask;
}

inline void QAlphaMerge::setMask(QMat *mask){
    m_mask = mask;
    emit inputChanged();
    QMatFilter::transform();
}

#endif // QALPHAMERGE_HPP

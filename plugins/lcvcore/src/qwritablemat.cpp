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

#include "qwritablemat.h"
#include "qmat.h"
#include "qmatop.h"
#include <QQmlEngine>


/**
 * \class QWritableMat
 * \ingroup plugin-lcvcore
 * \brief Similar to QMat, but used for quick writeable functions like drawing.
 *
 * This class will mostly used to preserve QMat's immutability accross functions.
 *
 * To convert to a QMat, use:
 *
 * \code
 * QMat* m = writableMat.toMat();
 * \endcode
 */

/**
 *\brief QWritableMat constructor
 */
QWritableMat::QWritableMat(QObject *parent)
    : QObject(parent)
    , m_internal(new cv::Mat)
{
}

/**
 * \brief QWritableMat constructor with internal mat
 */
QWritableMat::QWritableMat(cv::Mat *mat, QObject *parent)
    : QObject(parent)
    , m_internal(mat)
{
}

/**
 * \brief QWritableMat destructor
 */
QWritableMat::~QWritableMat(){
    delete m_internal;
}

/**
 * \brief Returns the internal mat
 */
const cv::Mat &QWritableMat::internal() const{
    return *m_internal;
}

/**
 * \brief Returns the internal mat
 */
cv::Mat &QWritableMat::internal(){
    return *m_internal;
}

/**
 * \brief Returns an equivalent ArrayBuffer to access the matrix values
 */
QByteArray QWritableMat::buffer(){
    return QByteArray::fromRawData(
        reinterpret_cast<const char*>(m_internal->data),
        static_cast<int>(m_internal->total() * m_internal->elemSize())
                );
}

/**
 * \brief Returns the number of channels
 */
int QWritableMat::channels() const{
    return m_internal->channels();
}

/**
 * \brief Returns the matrix depth (CV8U, CV16S, ...)
 */
int QWritableMat::depth() const{
    return m_internal->depth();
}

/**
 * \brief Returns the size of the matrix.
 */
QSize QWritableMat::dimensions() const{
    return QSize(m_internal->cols, m_internal->rows);
}

/**
 * \brief Returns copied QMat of this object
 */
QMat *QWritableMat::toMat() const{
    cv::Mat* m = new cv::Mat;
    m_internal->copyTo(*m);
    QMat* mwrap = new QMat(m);
    return mwrap;
}

/**
 * \brief Fills the matrix with a specified color
 */
void QWritableMat::fill(const QColor &color, QMat *mask){
    if ( mask ){
        m_internal->setTo(QMatOp::toScalar(color), mask->internal());
    } else {
        m_internal->setTo(QMatOp::toScalar(color));
    }
}

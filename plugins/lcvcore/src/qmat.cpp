/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "qmat.h"
#include <QQmlEngine>


/**
 *\class QMat
 *\ingroup plugin-lcvcore
 *\brief Open cv matrix wrapper.

 *The class represents the wrapper for the opencv matrix element to be passed around in the QML structure. To access
 *its cv mat vaue, use the cvMat() function.

 *To access it's pixels within qml, use the Mat::buffer() function.
 */


/**
 *\enum QMat::Type
 *
 * **CV8U**
 * **CV8S**
 * **CV16U**
 * **CV16S**
 * **CV32S**
 * **CV32F**
 * **CV64F**
 */

/**
 *\brief QMat::QMat
 *
 *Params : \a parent
 */
QMat::QMat(QObject *parent):
    QObject(parent),
    m_internal(new cv::Mat){
}

/**
 *\brief QMat::QMat
 *
 *Params : 
 *\a mat 
 *\a parent
 */
QMat::QMat(cv::Mat *mat, QObject *parent):
    QObject(parent),
    m_internal(mat){
}

/**
 * \brief Returns an equivalent ArrayBuffer to access the matrix values
 * \return
 */
QByteArray QMat::buffer(){
    return QByteArray::fromRawData(
        reinterpret_cast<const char*>(m_internal->data),
        static_cast<int>(m_internal->total() * m_internal->elemSize())
    );
}

/**
 * \brief QMat::channels
 * \return
 */
int QMat::channels(){
    return m_internal->channels();
}

/**
 * \brief QMat::depth
 * \return
 */
int QMat::depth(){
    return m_internal->depth();
}

/**
 *\brief Returns the size of the matrix element
 */
QSize QMat::dimensions() const{
    return QSize(m_internal->cols, m_internal->rows);
}

/**
 *\brief Returns a shallow copied matrix that is owned by the javascript engine
 */
QMat* QMat::createOwnedObject(){
    cv::Mat* ownedObjectInternal = new cv::Mat(*m_internal);
    QMat*    ownedObject         = new QMat(ownedObjectInternal);
    QQmlEngine::setObjectOwnership(ownedObject, QQmlEngine::JavaScriptOwnership);
    return ownedObject;
}

/**
 *\brief Returns a cloned matrix that is owned by the javascript engine
 */
QMat* QMat::cloneMat() const{
    cv::Mat* clonedMat = new cv::Mat;
    m_internal->copyTo(*clonedMat);
    QMat* clonedObject = new QMat(clonedMat);
    QQmlEngine::setObjectOwnership(clonedObject, QQmlEngine::JavaScriptOwnership);
    return clonedObject;
}

/**
 *\brief QMat::~QMat
 */
QMat::~QMat(){
    delete m_internal;
}

const cv::Mat &QMat::data() const{
    return *m_internal;
}

QMat* QMat::m_nullMat = 0;

/**
 *\brief Returns a null matrix
 */
QMat*QMat::nullMat(){
    if ( !m_nullMat ){
        m_nullMat = new QMat;
        atexit(&QMat::cleanUp);
    }
    return m_nullMat;
}

/**
 *\brief Internal method used to clean up the null matrix.
 */
void QMat::cleanUp(){
    delete m_nullMat;
}

/**
 *\brief Deep clones the mat
 */
QMat *QMat::clone() const{
    cv::Mat* clonedMat = new cv::Mat;
    m_internal->copyTo(*clonedMat);
    QMat* clonedObject = new QMat(clonedMat);
    return clonedObject;
}

/**
 * \brief Relocates the matrix to a new address
 */
QMat *QMat::reloc(QMat *m){
    QMat* res = new QMat(m->m_internal);
    delete m;
    return res;
}

const cv::Mat &QMat::internal() const{
    return *m_internal;
}

cv::Mat &QMat::internal(){
    return *m_internal;
}

/*!
  \fn cv::Mat* QMat::cvMat()
  \brief Returns the contained open cv mat.
 */


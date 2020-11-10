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

#include "qumat.h"
#include "qmatop.h"

#include <QQmlEngine>

#include "live/memory.h"
#include "live/visuallog.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"

/**
 *\class QUMat
 *\ingroup plugin-lcvcore
 *\brief Open cv universal matrix wrapper.

 *The class represents the wrapper for the opencv matrix element to be passed around in the QML structure. To access
 *it's cv mat value, use the internal() function.

 *To access it's pixels within qml, use the Mat::buffer() function.
 */


/**
 *\enum QUMat::Type
 *
 * Type of the QUMat
 */

/**
 *\brief QUMat constructor
 */
QUMat::QUMat(QObject *parent)
    : lv::Shared(parent)
    , m_internal(new cv::UMat)
{
}

/**
 *\brief QUMat constructor with internal mat
 */
QUMat::QUMat(cv::UMat *mat, QObject *parent)
    : lv::Shared(parent)
    , m_internal(mat)
{
}

/**
 * \brief QUMat memory-based constructor
 * @param width
 * @param height
 * @param type
 * @param channels
 * @param parent
 */
QUMat::QUMat(int width, int height, QUMat::Type type, int channels, QObject *parent)
    : lv::Shared(parent)
    , m_internal(lv::Memory::alloc<QUMat, cv::UMat>(memorySize(width, height, type, channels), width, height, type, channels))
{
}

/**
 * \brief Returns the number of channels
 */
int QUMat::channels() const{
    return m_internal->channels();
}

/**
 * \brief Returns the matrix depth (CV8U, CV16S, ...)
 */
int QUMat::depth() const{
    return m_internal->depth();
}

/**
 * \brief Returns the size of the matrix.
 */
QSize QUMat::dimensions() const{
    return QSize(m_internal->cols, m_internal->rows);
}

/**
 *\brief Returns a shallow copied matrix that is owned by the javascript engine
 */
QUMat* QUMat::createOwnedObject(){
    cv::UMat* ownedObjectInternal = new cv::UMat(*m_internal);
    QUMat*    ownedObject         = new QUMat(ownedObjectInternal);
    QQmlEngine::setObjectOwnership(ownedObject, QQmlEngine::JavaScriptOwnership);
    return ownedObject;
}

/**
 *\brief Returns a cloned matrix that is owned by the javascript engine
 */
QUMat* QUMat::cloneMat() const{
    cv::UMat* clonedMat = new cv::UMat;
    m_internal->copyTo(*clonedMat);
    QUMat* clonedObject = new QUMat(clonedMat);
    QQmlEngine::setObjectOwnership(clonedObject, QQmlEngine::JavaScriptOwnership);
    return clonedObject;
}

QMat *QUMat::getMat() const{
    QMat* m = new QMat;
    *m->internalPtr() = internal().getMat(cv::ACCESS_READ);
    return m;
}

/**
 *\brief QUMat::~QUMat
 */
QUMat::~QUMat(){
    lv::Memory::free(this, m_internal);
}

QUMat* QUMat::m_nullMat = nullptr;

/**
 *\brief Returns a null matrix
 */
QUMat*QUMat::nullMat(){
    if ( !m_nullMat ){
        m_nullMat = new QUMat(lv::ViewContext::instance().engine());
        Shared::ownCpp(m_nullMat);
    }
    return m_nullMat;
}

cv::UMat *QUMat::memoryAlloc(int width, int height, int type, int channels){
    return new cv::UMat(height, width, CV_MAKETYPE(type, channels));
}

size_t QUMat::memorySize(int width, int height, int type, int channels){
    size_t typeSize = 1;
    if ( type == QUMat::CV16U || type == QUMat::CV16S ){
        typeSize = sizeof(int16_t);
    } else if ( type == QUMat::CV32S ){
        typeSize = sizeof(int32_t);
    } else if ( type == QUMat::CV32F ){
        typeSize = sizeof(float);
    } else if ( type == QUMat::CV64F ){
        typeSize = sizeof(double);
    }
    return static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels) * typeSize;
}

size_t QUMat::memorySize(const QUMat *m){
    return memorySize(m->internal().cols, m->internal().rows, m->internal().type(), m->channels());
}

/**
 *\brief Deep clones the mat
 */
QUMat *QUMat::clone() const{
    cv::UMat* clonedMat = new cv::UMat;
    m_internal->copyTo(*clonedMat);
    QUMat* clonedObject = new QUMat(clonedMat);
    return clonedObject;
}

/**
 * \brief Relocates the matrix to a new address
 */
QUMat *QUMat::reloc(QUMat *m){
    QUMat* res = new QUMat(m->m_internal);
    delete m;
    return res;
}

/**
 * \brief Returns the internal mat
 */
const cv::UMat &QUMat::internal() const{
    return *m_internal;
}

/**
 * \brief Returns the internal mat
 */
cv::UMat &QUMat::internal(){
    return *m_internal;
}

/**
*\brief Memory allocation
*/
void QUMat::recycleSize(int){
    lv::Memory::reserve<QUMat, cv::UMat>(this);
}

/*!
  \fn cv::UMat* QUMat::internalPtr()
  \brief Returns the contained open cv mat.
 */

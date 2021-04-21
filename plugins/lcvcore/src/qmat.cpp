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

#include "qmat.h"
#include "qmatext.h"
#include "qmatop.h"
#include <QQmlEngine>
#include "live/memory.h"
#include "live/visuallog.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"

/**
 *\class QMat
 *\ingroup plugin-lcvcore
 *\brief Open cv matrix wrapper.

 *The class represents the wrapper for the opencv matrix element to be passed around in the QML structure. To access
 *it's cv mat value, use the internal() function.

 *To access it's pixels within qml, use the Mat::buffer() function.
 */


/**
 *\enum QMat::Type
 *
 * Type of the QMat
 */

/**
 *\brief QMat constructor
 */
QMat::QMat(QObject *parent)
    : lv::Shared(parent)
    , m_internal(new cv::Mat)
{
}

/**
 *\brief QMat constructor with internal mat
 */
QMat::QMat(cv::Mat *mat, QObject *parent)
    : lv::Shared(parent)
    , m_internal(mat)
{
}

/**
 * \brief QMat memory-based constructor
 * @param width
 * @param height
 * @param type
 * @param channels
 * @param parent
 */
QMat::QMat(int width, int height, QMat::Type type, int channels, QObject *parent)
    : lv::Shared(parent)
    , m_internal(lv::Memory::alloc<QMat, cv::Mat>(memorySize(width, height, type, channels), width, height, type, channels))
{
}

/**
 * \brief Returns an equivalent ArrayBuffer to access the matrix values
 */
QByteArray QMat::buffer() const{
    return QByteArray::fromRawData(
        reinterpret_cast<const char*>(m_internal->data),
        static_cast<int>(m_internal->total() * m_internal->elemSize())
    );
}

/**
 * \brief Returns the number of channels
 */
int QMat::channels() const{
    return m_internal->channels();
}

/**
 * \brief Returns the matrix depth (CV8U, CV16S, ...)
 */
int QMat::depth() const{
    return m_internal->depth();
}

/**
 * \brief Returns the size of the matrix.
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
    lv::Memory::free(this, m_internal);
}

QMat* QMat::m_nullMat = nullptr;

/**
 *\brief Returns a null matrix
 */
QMat*QMat::nullMat(){
    if ( !m_nullMat ){
        m_nullMat = new QMat(lv::ViewContext::instance().engine());
        Shared::ownCpp(m_nullMat);
    }
    return m_nullMat;
}

cv::Mat *QMat::memoryAlloc(int width, int height, int type, int channels){
    return new cv::Mat(height, width, CV_MAKETYPE(type, channels));
}

size_t QMat::memorySize(int width, int height, int type, int channels){
    size_t typeSize = 1;
    if ( type == QMat::CV16U || type == QMat::CV16S ){
        typeSize = sizeof(int16_t);
    } else if ( type == QMat::CV32S ){
        typeSize = sizeof(int32_t);
    } else if ( type == QMat::CV32F ){
        typeSize = sizeof(float);
    } else if ( type == QMat::CV64F ){
        typeSize = sizeof(double);
    }
    return static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels) * typeSize;
}

size_t QMat::memorySize(const QMat *m){
    return memorySize(m->internal().cols, m->internal().rows, m->internal().type(), m->channels());
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

/**
 * \brief Returns the internal mat
 */
const cv::Mat &QMat::internal() const{
    return *m_internal;
}

/**
 * \brief Returns the internal mat
 */
cv::Mat &QMat::internal(){
    return *m_internal;
}

/**
*\brief Memory allocation
*/
void QMat::recycleSize(int){
    lv::Memory::reserve<QMat, cv::Mat>(this);
}

/*!
  \fn cv::Mat* QMat::internalPtr()
  \brief Returns the contained open cv mat.
 */

namespace lv{

lv::QmlObjectList* createMatList(){
    std::vector<QMat*>* data = new std::vector<QMat*>;
    return new lv::QmlObjectList(data, &typeid(std::vector<QMat*>),
        &QMatOp::ListOperations::itemCount, &QMatOp::ListOperations::itemAt, &QMatOp::ListOperations::appendItem,
        &QMatOp::ListOperations::removeItemAt, &QMatOp::ListOperations::clearItems);
}

}

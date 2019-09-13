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

#include "qkeypointvector.h"
#include "qkeypoint.h"
#include <QQmlEngine>

/**
*\class QKeyPointVector
*\ingroup plugin-lcvfeatures2d
*\brief Wrapper around  keypoints
*/

/**
*\brief QKeyPointVector constructor
*/
QKeyPointVector::QKeyPointVector(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, false);
}

/**
*\brief QKeyPointVector destructor
*/
QKeyPointVector::~QKeyPointVector(){
}

/**
*\fn lv::QKeyPointVector::setMat()
*\brief Matrix where keypoint are detected from.
*/

/**
*\fn lv::QKeyPointVector::createOwnedObject()
*\brief Creates copy of the vector.
*/

/**
*\fn QKeyPointVector::keyPointData()
*\brief Returns the internal data as a list of objects.
*/

/**
*\fn QKeyPointVector::setKeyPointData()
*\brief Sets the internal data as a list of objects.
*/

/**
*\fn QKeyPointVector::appendKeyPoint()
*\brief Appends keypoint
*/

/**
*\fn QKeyPointVector::removeKeyPoint()
*\brief Removes keypoint
*/

/**
*\fn QKeyPointVector::createKeyPoint()
*\brief Creates keypoint
*/

/**
*\fn QKeyPointVector::size()
*\brief Size of the vector
*/






QKeyPointVector* QKeyPointVector::createOwnedObject(){
    QKeyPointVector* ownedObject = new QKeyPointVector;
    ownedObject->m_keyPoints     = m_keyPoints;
    ownedObject->m_mat           = m_mat;

    QQmlEngine::setObjectOwnership(ownedObject, QQmlEngine::JavaScriptOwnership);
    return ownedObject;
}

QList<QObject *> QKeyPointVector::keyPointData(){
    QList<QObject*> data;
    for ( std::vector<cv::KeyPoint>::const_iterator it = m_keyPoints.begin(); it != m_keyPoints.end(); ++it){
        data.append(new QKeyPoint(*it));
    }
    return data;
}

void QKeyPointVector::setKeyPointData(const QList<QObject *> data){
    m_keyPoints.clear();
    for ( QList<QObject*>::const_iterator it = data.begin(); it != data.end(); ++it ){
        QKeyPoint* kp = qobject_cast<QKeyPoint*>(*it);
        if ( !kp ){
            qWarning("Cannot convert. Object not of keypoint type.");
            break;
        }
        cv::KeyPoint cvkp;
        kp->toKeyPoint(cvkp);
        m_keyPoints.push_back(cvkp);
    }
}

void QKeyPointVector::appendKeyPoint(QKeyPoint *pt){
    QKeyPoint* kp = qobject_cast<QKeyPoint*>(pt);
    if ( !kp ){
        qWarning("Cannot convert. Object not of keypoint type.");
        return;
    }
    cv::KeyPoint cvkp;
    kp->toKeyPoint(cvkp);
    m_keyPoints.push_back(cvkp);
}

void QKeyPointVector::removeKeyPoint(int position){
    if ( static_cast<size_t>(position) < m_keyPoints.size() )
        m_keyPoints.erase(m_keyPoints.begin() + position);
}

QKeyPoint* QKeyPointVector::createKeyPoint(){
    return new QKeyPoint;
}

/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "qkeypoint.h"
#include "opencv2/features2d/features2d.hpp"

QKeyPoint::QKeyPoint(QObject *parent)
    : QObject(parent)
    , m_ptX(0)
    , m_ptY(0)
    , m_size(0)
    , m_angle(0)
    , m_response(0)
    , m_octave(0)
    , m_classId(0)
{
}

QKeyPoint::QKeyPoint(const cv::KeyPoint &keypoint, QObject *parent)
    : QObject(parent)
    , m_ptX(keypoint.pt.x)
    , m_ptY(keypoint.pt.y)
    , m_size(keypoint.size)
    , m_angle(keypoint.angle)
    , m_response(keypoint.response)
    , m_octave(keypoint.octave)
    , m_classId(keypoint.class_id)
{
}

QKeyPoint::~QKeyPoint(){
}

void QKeyPoint::toKeyPoint(cv::KeyPoint &kp){
    kp.pt       = cv::Point2f(m_ptX, m_ptY);
    kp.size     = m_size;
    kp.angle    = m_angle;
    kp.response = m_response;
    kp.octave   = m_octave;
    kp.class_id = m_classId;
}


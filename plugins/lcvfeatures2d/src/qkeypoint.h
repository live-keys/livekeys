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

#ifndef QKEYPOINT_H
#define QKEYPOINT_H

#include <QObject>
#include <QPointF>

namespace cv{ class KeyPoint; }
/// \private
class QKeyPoint : public QObject{

    Q_OBJECT
    Q_PROPERTY(QPointF pt     READ pt       WRITE setPt       NOTIFY ptChanged)
    Q_PROPERTY(float size     READ size     WRITE setSize     NOTIFY sizeChanged)
    Q_PROPERTY(float angle    READ angle    WRITE setAngle    NOTIFY angleChanged)
    Q_PROPERTY(float response READ response WRITE setResponse NOTIFY responseChanged)
    Q_PROPERTY(int octave     READ octave   WRITE setOctave   NOTIFY octaveChanged)
    Q_PROPERTY(int classId    READ classId  WRITE setClassId  NOTIFY classIdChanged)

public:
    QKeyPoint(QObject* parent = 0);
    QKeyPoint(const cv::KeyPoint& keypoint, QObject* parent = 0);
    ~QKeyPoint();

    void toKeyPoint(cv::KeyPoint& kp);

    QPointF pt() const;
    float size() const;
    float angle() const;
    float response() const;
    int octave() const;
    int classId() const;

    void setPt(QPointF arg);
    void setSize(float arg);
    void setAngle(float arg);
    void setResponse(float arg);
    void setOctave(int arg);
    void setClassId(int arg);

signals:
    void ptChanged();
    void sizeChanged();
    void angleChanged();
    void responseChanged();
    void octaveChanged();
    void classIdChanged();

private:
    float  m_ptX;
    float  m_ptY;

    float  m_size;
    float  m_angle;
    float  m_response;
    int    m_octave;
    int    m_classId;
};

inline QPointF QKeyPoint::pt() const{
    return QPointF(m_ptX, m_ptY);
}

inline float QKeyPoint::size() const{
    return m_size;
}

inline float QKeyPoint::angle() const{
    return m_angle;
}

inline float QKeyPoint::response() const{
    return m_response;
}

inline int QKeyPoint::octave() const{
    return m_octave;
}

inline int QKeyPoint::classId() const{
    return m_classId;
}

inline void QKeyPoint::setPt(QPointF arg){
    if ( m_ptX == arg.x() && m_ptY == arg.y() )
        return;

    m_ptX = arg.x();
    m_ptY = arg.y();
    emit ptChanged();
}

inline void QKeyPoint::setSize(float arg){
    if (m_size == arg)
        return;

    m_size = arg;
    emit sizeChanged();
}

inline void QKeyPoint::setAngle(float arg){
    if (m_angle == arg)
        return;

    m_angle = arg;
    emit angleChanged();
}

inline void QKeyPoint::setResponse(float arg){
    if (m_response == arg)
        return;

    m_response = arg;
    emit responseChanged();
}

inline void QKeyPoint::setOctave(int arg){
    if (m_octave == arg)
        return;

    m_octave = arg;
    emit octaveChanged();
}

inline void QKeyPoint::setClassId(int arg){
    if (m_classId == arg)
        return;

    m_classId = arg;
    emit classIdChanged();
}

#endif // QKEYPOINT_H

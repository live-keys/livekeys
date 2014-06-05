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

#ifndef QMATROI_HPP
#define QMATROI_HPP

#include "QMatFilter.hpp"
#include "QLCVGlobal.hpp"

class QMatRoi : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(int regionX      READ regionX      WRITE setregionX      NOTIFY regionXChanged)
    Q_PROPERTY(int regionY      READ regionY      WRITE setregionY      NOTIFY regionYChanged)
    Q_PROPERTY(int regionWidth  READ regionWidth  WRITE setRegionWidth  NOTIFY regionWidthChanged)
    Q_PROPERTY(int regionHeight READ regionHeight WRITE setRegionHeight NOTIFY regionHeightChanged)

public:
    explicit QMatRoi(QQuickItem *parent = 0);

    virtual void transform(cv::Mat &in, cv::Mat &out);

    int regionX() const;
    int regionY() const;
    void setregionX(int regionX);
    void setregionY(int regionY);

    int regionWidth() const;
    int regionHeight() const;
    void setRegionWidth(int regionWidth);
    void setRegionHeight(int regionHeight);

signals:
    void regionXChanged();
    void regionYChanged();
    void regionWidthChanged();
    void regionHeightChanged();

private:
    int m_regionX;
    int m_regionY;
    int m_regionWidth;
    int m_regionHeight;

};

inline int QMatRoi::regionX() const{
    return m_regionX;
}

inline int QMatRoi::regionY() const{
    return m_regionY;
}

inline void QMatRoi::setregionX(int regionX){
    if ( m_regionX != regionX ){
        m_regionX = regionX;
        QMatFilter::transform();
        emit regionXChanged();
    }
}

inline void QMatRoi::setregionY(int regionY){
    if ( m_regionY != regionY ){
        m_regionY = regionY;
        QMatFilter::transform();
        emit regionYChanged();
    }
}

inline int QMatRoi::regionWidth() const{
    return m_regionWidth;
}

inline int QMatRoi::regionHeight() const{
    return m_regionHeight;
}

inline void QMatRoi::setRegionWidth(int regionWidth){
    if ( m_regionWidth != regionWidth ){
        m_regionWidth = regionWidth;
        QMatFilter::transform();
        emit regionWidthChanged();
    }
}

inline void QMatRoi::setRegionHeight(int regionHeight){
    if ( m_regionHeight != regionHeight ){
        m_regionHeight = regionHeight;
        QMatFilter::transform();
        emit regionHeightChanged();
    }
}

#endif // QMATROI_HPP

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

#ifndef LVTRIANGLE_H
#define LVTRIANGLE_H

#include <QQuickPaintedItem>

namespace lv{

/// \private
class Triangle : public QQuickPaintedItem{

    Q_OBJECT
    Q_PROPERTY(int rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QColor color READ color    WRITE setColor    NOTIFY colorChanged)
    Q_ENUMS(Rotation)

public:
    enum Rotation{
        Top = 0,
        Right,
        Bottom,
        Left
    };

public:
    explicit Triangle(QQuickItem *parent = nullptr);

    const QColor& color() const;
    void setColor(const QColor color);

    int rotation() const;
    void setRotation(int rotation);

    void paint(QPainter *painter);

signals:
    void rotationChanged();
    void colorChanged();

private:
    void updatePath();

private:
    int     m_rotation;
    QColor  m_color;
    QRect   m_lastBoundingRect;
    QPointF m_points[3];
};

inline const QColor& Triangle::color() const{
    return m_color;
}

inline void Triangle::setColor(const QColor color){
    if ( m_color != color ){
        m_color = color;
        update();
        emit colorChanged();
    }
}

inline int Triangle::rotation() const{
     return m_rotation;
}

inline void Triangle::setRotation(int rotation){
     if ( m_rotation != rotation ){
         m_rotation = rotation;
         updatePath();
         update();
         emit rotationChanged();
    }
}

}// namespace

#endif // LVTRIANGLE_H

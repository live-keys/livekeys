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

#include "triangle.h"
#include <QPainter>

namespace lv{

Triangle::Triangle(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_rotation(0)
    , m_color("#000000")
{
}

void Triangle::updatePath(){
    switch (m_rotation){
    case Triangle::Top:
        m_points[0] = QPointF(boundingRect().center().x(), boundingRect().top());
        m_points[1] = boundingRect().bottomLeft();
        m_points[2] = boundingRect().bottomRight();
        break;
    case Triangle::Right:
        m_points[0] = QPointF(boundingRect().right(), boundingRect().center().y());
        m_points[1] = boundingRect().bottomLeft();
        m_points[2] = boundingRect().topLeft();
        break;
    case Triangle::Bottom:
        m_points[0] = QPointF(boundingRect().center().x(), boundingRect().bottom());
        m_points[1] = boundingRect().topLeft();
        m_points[2] = boundingRect().topRight();
        break;
    case Triangle::Left:
        m_points[0] = QPointF(boundingRect().left(), boundingRect().center().y());
        m_points[1] = boundingRect().topRight();
        m_points[2] = boundingRect().bottomRight();
        break;
    }
}

void Triangle::paint(QPainter *painter){
    if ( m_lastBoundingRect != boundingRect() ){
        m_lastBoundingRect.setRect( boundingRect().x(), boundingRect().y(), boundingRect().width(), boundingRect().height() );
        updatePath();
    }
    painter->setPen(QPen(m_color));
    painter->setBrush(QBrush(m_color));
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPolygon(m_points, 3, Qt::OddEvenFill);
}

}// namespace

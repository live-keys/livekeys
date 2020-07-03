#include "linegrid.h"
#include <QPainter>
#include <QPen>
#include <QBrush>

namespace lv{

LineGrid::LineGrid(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_colSpacing(25)
    , m_rowSpacing(25)
    , m_thickness(1)
    , m_color(QColor("white"))
{
}

void LineGrid::paint(QPainter *painter){
    QRectF rect = boundingRect();

    painter->setPen(QPen(m_color));
    painter->setBrush(QBrush(m_color));

    for ( double i = m_colSpacing; i < rect.width(); i += m_colSpacing){
        painter->drawLine(i, 0, i, rect.height());
    }
    for ( double i = m_rowSpacing; i < rect.height(); i += m_rowSpacing){
        painter->drawLine(0, i, rect.width(), i);
    }

}

} // namespace

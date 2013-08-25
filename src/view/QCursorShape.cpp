#include "QCursorShape.hpp"
#include <QCursor>

QCursorShape::QCursorShape(QQuickItem *parent) :
    QQuickItem(parent),
    m_currentShape(-1)
{
}

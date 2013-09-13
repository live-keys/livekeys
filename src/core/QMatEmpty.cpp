#include "QMatEmpty.hpp"

QMatEmpty::QMatEmpty(QQuickItem *parent) :
    QMatDisplay(parent),
    m_matSize(0, 0),
    m_color(0, 0, 0),
    m_channels(1),
    m_type(QMat::CV8U),
    m_colorScalar(0)
{
}

QMatEmpty::~QMatEmpty(){
}

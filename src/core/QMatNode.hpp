#ifndef QMATNODE_HPP
#define QMATNODE_HPP

#include <QSGGeometryNode>
#include "QMatState.hpp"


class QMatNode : public QSGGeometryNode{

public:
    QMatNode();

private:
    QSGGeometry m_geometry;

};

#endif // QMATNODE_HPP




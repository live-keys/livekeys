#ifndef QMATSTATE_HPP
#define QMATSTATE_HPP

#include "QMat.hpp"

class QMatState{

public:
    QMat* mat;
    bool  linearFilter;
    mutable int   textureIndex;
    mutable bool  textureSync;

    QMatState();

    int compare(const QMatState *other) const;
};

#endif // QMATSTATE_HPP

#include "QKeyPointVector.hpp"

QKeyPointVector::QKeyPointVector(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, false);
}

QKeyPointVector::~QKeyPointVector(){
}

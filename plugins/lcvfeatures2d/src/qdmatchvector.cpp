#include "qdmatchvector.h"

QDMatchVector::QDMatchVector(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, false);
}

QDMatchVector::~QDMatchVector(){
}

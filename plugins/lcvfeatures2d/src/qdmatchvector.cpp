#include "qdmatchvector.h"

QDMatchVector::QDMatchVector(QQuickItem *parent)
    : QQuickItem(parent)
    , m_type(QDMatchVector::BEST_MATCH)
{
    setFlag(QQuickItem::ItemHasContents, false);
}

QDMatchVector::~QDMatchVector(){
}

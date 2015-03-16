#include "QKeyPointVector.hpp"
#include <QQmlEngine>

QKeyPointVector::QKeyPointVector(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, false);
}

QKeyPointVector::~QKeyPointVector(){
}

QKeyPointVector* QKeyPointVector::createOwnedObject(){
    QKeyPointVector* ownedObject = new QKeyPointVector;
    ownedObject->m_keyPoints     = m_keyPoints;
    ownedObject->m_mat           = m_mat;

    qDebug() << ownedObject->m_keyPoints.size();

    QQmlEngine::setObjectOwnership(ownedObject, QQmlEngine::JavaScriptOwnership);
    return ownedObject;
}

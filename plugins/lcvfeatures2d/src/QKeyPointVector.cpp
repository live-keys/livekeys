#include "QKeyPointVector.hpp"
#include "QKeyPoint.hpp"
#include <QQmlEngine>

QKeyPointVector::QKeyPointVector(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, false);
}

QKeyPointVector::~QKeyPointVector(){
}

QKeyPointVector* QKeyPointVector::createOwnedObject(){
    QKeyPointVector* ownedObject = new QKeyPointVector;
    ownedObject->m_keyPoints     = m_keyPoints;
    ownedObject->m_mat           = m_mat;

    QQmlEngine::setObjectOwnership(ownedObject, QQmlEngine::JavaScriptOwnership);
    return ownedObject;
}

QList<QObject *> QKeyPointVector::keyPointData(){
    QList<QObject*> data;
    for ( std::vector<cv::KeyPoint>::const_iterator it = m_keyPoints.begin(); it != m_keyPoints.end(); ++it){
        data.append(new QKeyPoint(*it));
    }
    return data;
}

void QKeyPointVector::setKeyPointData(const QList<QObject *> data){
    m_keyPoints.clear();
    for ( QList<QObject*>::const_iterator it = data.begin(); it != data.end(); ++it ){
        QKeyPoint* kp = qobject_cast<QKeyPoint*>(*it);
        if ( !kp ){
            qWarning("Cannot convert. Object not of keypoint type.");
            break;
        }
        cv::KeyPoint cvkp;
        kp->toKeyPoint(cvkp);
        m_keyPoints.push_back(cvkp);
    }
}

void QKeyPointVector::appendKeyPoint(QKeyPoint *pt){
    QKeyPoint* kp = qobject_cast<QKeyPoint*>(pt);
    if ( !kp ){
        qWarning("Cannot convert. Object not of keypoint type.");
        return;
    }
    cv::KeyPoint cvkp;
    kp->toKeyPoint(cvkp);
    m_keyPoints.push_back(cvkp);
}

void QKeyPointVector::removeKeyPoint(int position){
    if ( static_cast<size_t>(position) < m_keyPoints.size() )
        m_keyPoints.erase(m_keyPoints.begin() + position);
}

QKeyPoint* QKeyPointVector::createKeyPoint(){
    return new QKeyPoint;
}

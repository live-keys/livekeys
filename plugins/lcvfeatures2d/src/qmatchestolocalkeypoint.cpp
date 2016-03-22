#include "qmatchestolocalkeypoint.h"

#include <QDebug>

QMatchesToLocalKeypoint::QMatchesToLocalKeypoint(QQuickItem *parent)
    : QQuickItem(parent)
    , m_matches1to2(0)
    , m_queryKeypointVectors(0)
    , m_output(new QKeyPointToSceneMap)
{
}

QMatchesToLocalKeypoint::~QMatchesToLocalKeypoint(){
    delete m_output;
}

void QMatchesToLocalKeypoint::componentComplete(){
    QQuickItem::componentComplete();
    mapValues();
}

void QMatchesToLocalKeypoint::mapValues(){
    if ( !isComponentComplete() )
        return;
    if ( m_matches1to2 == 0 || m_queryKeypointVectors == 0 )
        return;
    if ( !m_matches1to2->matches().size() )
        return;

    m_output->resize(m_trainKeypointVectors.size());

    std::vector<cv::DMatch>& matches = m_matches1to2->matches()[0];
    for ( std::vector<cv::DMatch>::iterator it = matches.begin(); it != matches.end(); ++it ){
        cv::DMatch& match = *it;
        if ( match.imgIdx >= m_trainKeypointVectors.size() )
            return;
        QKeyPointVector* trainVector = qobject_cast<QKeyPointVector*>(m_trainKeypointVectors[match.imgIdx]);
        if ( !trainVector ){
            qWarning("Invalid keypoint vector given at %d", match.imgIdx);
            return;
        }

        m_output->mappingAt(match.imgIdx)->objectPoints.push_back(
            trainVector->keypoints().at(match.trainIdx).pt
        );
        m_output->mappingAt(match.imgIdx)->scenePoints.push_back(
            m_queryKeypointVectors->keypoints().at(match.queryIdx).pt
        );

    }
    qDebug() << m_output->mappingAt(0)->scenePoints.size();
    qDebug() << m_output->mappingAt(1)->scenePoints.size();

    emit outputChanged();
}


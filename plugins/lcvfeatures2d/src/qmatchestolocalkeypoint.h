#ifndef QMATCHESTOLOCALKEYPOINT_H
#define QMATCHESTOLOCALKEYPOINT_H

#include <QQuickItem>
#include "qdmatchvector.h"
#include "qkeypointvector.h"
#include "qkeypointtoscenemap.h"
#include "qlcvfeatures2dglobal.h"

class Q_LCVFEATURES2D_EXPORT QMatchesToLocalKeypoint : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QDMatchVector*   matches1to2          READ matches1to2          WRITE setMatches1to2          NOTIFY matches1to2Changed)
    Q_PROPERTY(QList<QObject*>  trainKeypointVectors READ trainKeypointVectors WRITE setTrainKeypointVectors NOTIFY trainKeypointVectorsChanged)
    Q_PROPERTY(QKeyPointVector* queryKeypointVector  READ queryKeypointVector  WRITE setQueryKeypointVector  NOTIFY queryKeypointVectorChanged)
    Q_PROPERTY(QKeyPointToSceneMap* output           READ output               NOTIFY outputChanged)

public:
    explicit QMatchesToLocalKeypoint(QQuickItem *parent = 0);
    ~QMatchesToLocalKeypoint();

    QDMatchVector* matches1to2();
    const QList<QObject*>& trainKeypointVectors() const;
    QKeyPointVector* queryKeypointVector();

    QKeyPointToSceneMap* output();

protected:
    void componentComplete();

signals:
    void matches1to2Changed();
    void trainKeypointVectorsChanged();
    void queryKeypointVectorChanged();
    void outputChanged();

public slots:
    void setMatches1to2(QDMatchVector* arg);
    void setTrainKeypointVectors(QList<QObject *> arg);
    void setQueryKeypointVector(QKeyPointVector* arg);

private:
    void mapValues();

    QDMatchVector*   m_matches1to2;
    QList<QObject*>  m_trainKeypointVectors;
    QKeyPointVector* m_queryKeypointVector;
    QKeyPointToSceneMap* m_output;
};

inline QDMatchVector *QMatchesToLocalKeypoint::matches1to2(){
    return m_matches1to2;
}

inline const QList<QObject *> &QMatchesToLocalKeypoint::trainKeypointVectors() const{
    return m_trainKeypointVectors;
}

inline QKeyPointVector *QMatchesToLocalKeypoint::queryKeypointVector(){
    return m_queryKeypointVector;
}

inline QKeyPointToSceneMap *QMatchesToLocalKeypoint::output(){
    return m_output;
}

inline void QMatchesToLocalKeypoint::setMatches1to2(QDMatchVector *arg){
    m_matches1to2 = arg;
    emit matches1to2Changed();
    mapValues();
}

inline void QMatchesToLocalKeypoint::setTrainKeypointVectors(QList<QObject*> arg){
    m_trainKeypointVectors = arg;
    emit trainKeypointVectorsChanged();
    mapValues();
}

inline void QMatchesToLocalKeypoint::setQueryKeypointVector(QKeyPointVector *arg){
    m_queryKeypointVector = arg;
    emit queryKeypointVectorChanged();
    mapValues();
}

#endif // QMATCHESTOLOCALKEYPOINT_H

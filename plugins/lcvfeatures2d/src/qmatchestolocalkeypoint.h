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
    Q_PROPERTY(QKeyPointVector* queryKeypointVectors READ queryKeypointVectors WRITE setQueryKeypointVectors NOTIFY queryKeypointVectorsChanged)
    Q_PROPERTY(QKeyPointToSceneMap* output           READ output               NOTIFY outputChanged)

public:
    explicit QMatchesToLocalKeypoint(QQuickItem *parent = 0);
    ~QMatchesToLocalKeypoint();

    QDMatchVector* matches1to2();
    const QList<QObject*>& trainKeypointVectors() const;
    QKeyPointVector* queryKeypointVectors();

    QKeyPointToSceneMap* output();

protected:
    void componentComplete();

signals:
    void matches1to2Changed();
    void trainKeypointVectorsChanged();
    void queryKeypointVectorsChanged();

    void outputChanged();

public slots:
    void setMatches1to2(QDMatchVector* arg);
    void setTrainKeypointVectors(QList<QObject *> arg);
    void setQueryKeypointVectors(QKeyPointVector* arg);

private:
    void mapValues();

    QDMatchVector*   m_matches1to2;
    QList<QObject*>  m_trainKeypointVectors;
    QKeyPointVector* m_queryKeypointVectors;
    QKeyPointToSceneMap* m_output;
};

inline QDMatchVector *QMatchesToLocalKeypoint::matches1to2(){
    return m_matches1to2;
}

inline const QList<QObject *> &QMatchesToLocalKeypoint::trainKeypointVectors() const{
    return m_trainKeypointVectors;
}

inline QKeyPointVector *QMatchesToLocalKeypoint::queryKeypointVectors(){
    return m_queryKeypointVectors;
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

inline void QMatchesToLocalKeypoint::setQueryKeypointVectors(QKeyPointVector *arg){
    m_queryKeypointVectors = arg;
    emit queryKeypointVectorsChanged();
    mapValues();
}

#endif // QMATCHESTOLOCALKEYPOINT_H

#ifndef QDRAWMATCHES_HPP
#define QDRAWMATCHES_HPP

#include "QMatDisplay.hpp"
#include "QKeyPointVector.hpp"
#include "QDMatchVector.hpp"

class QDrawMatches : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QKeyPointVector* keypoints1  READ keypoints1  WRITE setKeypoints1  NOTIFY keypoints1Changed)
    Q_PROPERTY(QKeyPointVector* keypoints2  READ keypoints2  WRITE setKeypoints2  NOTIFY keypoints2Changed)
    Q_PROPERTY(QDMatchVector*   matches1to2 READ matches1to2 WRITE setMatches1to2 NOTIFY matches1to2Changed)
    Q_PROPERTY(int              matchIndex  READ matchIndex  WRITE setMatchIndex  NOTIFY matchIndexChanged)

public:
    explicit QDrawMatches(QQuickItem *parent = 0);
    virtual ~QDrawMatches();

    QKeyPointVector* keypoints1();
    void setKeypoints1(QKeyPointVector* keypoints1);

    QKeyPointVector* keypoints2();
    void setKeypoints2(QKeyPointVector* keypoints2);

    QDMatchVector* matches1to2();
    void setMatches1to2(QDMatchVector* matches1to2);

    int matchIndex() const;
    void setMatchIndex(int matchIndex);


protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

signals:
    void keypoints1Changed();
    void keypoints2Changed();
    void matches1to2Changed();
    void matchIndexChanged();

public slots:

private:
    QKeyPointVector*  m_keypoints1;
    QKeyPointVector*  m_keypoints2;
    QDMatchVector*    m_matches;
    std::vector<char> m_mask;

    bool              m_matchSurfaceDirty;
    int               m_matchIndex;
};

inline QKeyPointVector *QDrawMatches::keypoints1(){
    return m_keypoints1;
}

inline void QDrawMatches::setKeypoints1(QKeyPointVector *keypoints1){
    m_keypoints1 = keypoints1;
    emit keypoints1Changed();
    m_matchSurfaceDirty = true;
    update();
}

inline QKeyPointVector *QDrawMatches::keypoints2(){
    return m_keypoints2;
}

inline void QDrawMatches::setKeypoints2(QKeyPointVector *keypoints2){
    m_keypoints2 = keypoints2;
    emit keypoints2Changed();
    m_matchSurfaceDirty = true;
    update();
}

inline QDMatchVector *QDrawMatches::matches1to2(){
    return m_matches;
}

inline void QDrawMatches::setMatches1to2(QDMatchVector *matches1to2){
    m_matches = matches1to2;
    emit matches1to2Changed();
    m_matchSurfaceDirty = true;
    update();
}

inline int QDrawMatches::matchIndex() const{
    return m_matchIndex;
}

inline void QDrawMatches::setMatchIndex(int matchIndex){
    m_matchIndex = matchIndex;
    emit matchIndexChanged();
    m_matchSurfaceDirty = true;
    update();
}

#endif // QDRAWMATCHES_HPP

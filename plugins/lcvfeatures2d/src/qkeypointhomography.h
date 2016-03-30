#ifndef QKEYPOINTHOMOGRAPHY_H
#define QKEYPOINTHOMOGRAPHY_H

#include "qmatdisplay.h"
#include "qkeypointtoscenemap.h"
#include "qlcvfeatures2dglobal.h"

class Q_LCVFEATURES2D_EXPORT QKeypointHomography : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QKeyPointToSceneMap* keypointsToScene READ keypointsToScene WRITE setKeypointsToScene NOTIFY keypointsToSceneChanged)
    Q_PROPERTY(QMat* queryImage READ queryImage WRITE setQueryImage NOTIFY queryImageChanged)

public:
    QKeypointHomography(QQuickItem* parent = 0);
    ~QKeypointHomography();

    QKeyPointToSceneMap* keypointsToScene() const;
    void setKeypointsToScene(QKeyPointToSceneMap* arg);

    QMat* queryImage() const
    {
        return m_queryImage;
    }

public slots:
    void setQueryImage(QMat* queryImage)
    {
        if (m_queryImage == queryImage)
            return;

        m_queryImage = queryImage;
        emit queryImageChanged(queryImage);
        update();
    }

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

signals:
    void keypointsToSceneChanged();

    void queryImageChanged(QMat* queryImage);

private:
    QKeyPointToSceneMap* m_keypointsToScene;
    QMat* m_queryImage;
};

inline QKeyPointToSceneMap *QKeypointHomography::keypointsToScene() const{
    return m_keypointsToScene;
}

inline void QKeypointHomography::setKeypointsToScene(QKeyPointToSceneMap *arg){
    if (m_keypointsToScene == arg)
        return;

    m_keypointsToScene = arg;
    emit keypointsToSceneChanged();
    update();
}

#endif // QKEYPOINTHOMOGRAPHY_H

/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef QKEYPOINTHOMOGRAPHY_H
#define QKEYPOINTHOMOGRAPHY_H

#include "qmatdisplay.h"
#include "qkeypointtoscenemap.h"
#include "qlcvfeatures2dglobal.h"

class Q_LCVFEATURES2D_EXPORT QKeypointHomography : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QKeyPointToSceneMap* keypointsToScene READ keypointsToScene WRITE setKeypointsToScene NOTIFY keypointsToSceneChanged)
    Q_PROPERTY(QMat* queryImage                      READ queryImage       WRITE setQueryImage       NOTIFY queryImageChanged)
    Q_PROPERTY(QVariantList objectCorners            READ objectCorners    WRITE setObjectCorners    NOTIFY objectCornersChanged)
    Q_PROPERTY(QVariantList objectColors             READ objectColors     WRITE setObjectColors     NOTIFY objectColorsChanged)

public:
    QKeypointHomography(QQuickItem* parent = 0);
    ~QKeypointHomography();

    QKeyPointToSceneMap* keypointsToScene() const;
    void setKeypointsToScene(QKeyPointToSceneMap* arg);

    QMat* queryImage() const;
    QVariantList objectCorners() const;

    QVariantList objectColors() const;
    void setObjectColors(const QVariantList& arg);

public slots:
    void setQueryImage(QMat* queryImage);
    void setObjectCorners(QVariantList arg);
    void appendObjectCorners(QVariantList corner);

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

signals:
    void keypointsToSceneChanged();
    void queryImageChanged();
    void objectCornersChanged();

    void objectColorsChanged(QVariantList arg);

private:
    cv::Scalar colorAt(int i) const;

    QKeyPointToSceneMap* m_keypointsToScene;
    QMat*                m_queryImage;
    QVariantList         m_objectCorners;
    QVariantList         m_objectColors;
    QList<cv::Scalar>    m_cachedObjectColors;
};

inline QKeyPointToSceneMap *QKeypointHomography::keypointsToScene() const{
    return m_keypointsToScene;
}

inline void QKeypointHomography::setKeypointsToScene(QKeyPointToSceneMap *arg){
    m_keypointsToScene = arg;
    emit keypointsToSceneChanged();
    update();
}

inline QMat *QKeypointHomography::queryImage() const{
    return m_queryImage;
}

inline QVariantList QKeypointHomography::objectCorners() const{
    return m_objectCorners;
}

inline QVariantList QKeypointHomography::objectColors() const{
    return m_objectColors;
}

inline void QKeypointHomography::setQueryImage(QMat *queryImage){
    if ( queryImage == 0 )
        return;

    cv::Mat* matData = queryImage->cvMat();
    if ( implicitWidth() != matData->cols || implicitHeight() != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }

    m_queryImage = queryImage;
    emit queryImageChanged();
    update();
}

inline void QKeypointHomography::setObjectCorners(QVariantList arg){
    m_objectCorners = arg;
    emit objectCornersChanged();
    update();
}

inline void QKeypointHomography::appendObjectCorners(QVariantList corner){
    m_objectCorners.append(QVariant::fromValue(corner));
    emit objectCornersChanged();
    update();
}

inline void QKeypointHomography::setObjectColors(const QVariantList &arg){
    if (m_objectColors == arg)
        return;

    m_objectColors = arg;
    m_cachedObjectColors.clear();
    for ( int i = 0; i < m_objectColors.size(); ++i ){
        QColor color(m_objectColors[i].toString());
        m_cachedObjectColors.append(cv::Scalar(color.blue(), color.green(), color.red(), color.alpha()));
    }

    emit objectColorsChanged(arg);
    update();
}

#endif // QKEYPOINTHOMOGRAPHY_H

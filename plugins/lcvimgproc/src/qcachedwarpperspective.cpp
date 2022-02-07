/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

#include "qcachedwarpperspective.h"

#include "opencv2/imgproc.hpp"

#include "qmatnode.h"
#include "qmatshader.h"

namespace helpers{

void perspectiveToMaps(const cv::Mat &perspectiveMat, const cv::Size &imgSize, cv::Mat &map1, cv::Mat &map2){

    // invert the matrix because the transformation maps must be
    // bird's view -> original
    cv::Mat invPerspective(perspectiveMat.inv());
    invPerspective.convertTo(invPerspective, CV_32FC1);

    // create XY 2D array
    // (((0, 0), (1, 0), (2, 0), ...),
    //  ((0, 1), (1, 1), (2, 1), ...),
    // ...)
    cv::Mat xy(imgSize, CV_32FC2);
    float *pxy = (float*)xy.data;
    for (int y = 0; y < imgSize.height; y++){
        for (int x = 0; x < imgSize.width; x++){
            *pxy++ = x;
            *pxy++ = y;
        }
    }

    // perspective transformation of the points
    cv::Mat xyTransformed;
    cv::perspectiveTransform(xy, xyTransformed, invPerspective);

    // split x/y to extra maps
    assert(xyTransformed.channels() == 2);
    cv::Mat maps[2]; // map_x, map_y
    cv::split(xyTransformed, maps);

    // remap() with integer maps is faster
    cv::convertMaps(maps[0], maps[1], map1, map2, CV_16SC2);
}

} // namespace

QCachedWarpPerspective::QCachedWarpPerspective(QQuickItem *parent)
    : QQuickItem(parent)
    , m_in(QMat::nullMat())
    , m_output(QMat::nullMat())
    , m_m(QMat::nullMat())
    , m_interpolation(cv::INTER_LINEAR)
    , m_cacheDirty(true)
    , m_cacheMap1(new cv::Mat)
    , m_cacheMap2(new cv::Mat)
{
    setFlag(ItemHasContents, true);
}

QCachedWarpPerspective::~QCachedWarpPerspective(){
    delete m_cacheMap1;
    delete m_cacheMap2;
}

void QCachedWarpPerspective::transform(){
    if ( isComponentComplete() ){
        try{
            transform(*input()->internalPtr(), *output()->internalPtr());
            emit outputChanged();
            update();
        } catch (cv::Exception& e ){
            qCritical("%s", e.msg.c_str());
        }
    }
}

void QCachedWarpPerspective::transform(const cv::Mat &in, cv::Mat &out){
    if ( in.empty() )
        return;

    if ( m_cacheDirty ){
        helpers::perspectiveToMaps(m_m->internal(), out.size(), *m_cacheMap1, *m_cacheMap2);
        m_cacheDirty = false;
    }

    cv::remap(in, out, *m_cacheMap1, *m_cacheMap2, cv::INTER_LINEAR);
}

QSGNode *QCachedWarpPerspective::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *){
    QMatNode *n = static_cast<QMatNode*>(node);
    if (!node)
        n = new QMatNode();

    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
    QMatState* state = static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state();
    state->mat          = m_output;
    state->textureSync  = false;
    state->linearFilter = false;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;
}

void QCachedWarpPerspective::componentComplete(){
    QQuickItem::componentComplete();
    transform();
}


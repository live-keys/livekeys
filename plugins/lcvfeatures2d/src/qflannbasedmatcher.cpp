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

#include "qflannbasedmatcher.h"
#include "opencv2/features2d/features2d.hpp"

#include <QJsonObject>

QFlannBasedMatcher::QFlannBasedMatcher(QQuickItem* parent)
    : QDescriptorMatcher(0, parent)
{
}

QFlannBasedMatcher::~QFlannBasedMatcher(){
}

void QFlannBasedMatcher::initialize(const QJsonObject& initParams){

    cv::flann::IndexParams* indexParams = 0;
    if ( !initParams.contains("indexParams") ){
        qWarning("FlannBasedMatcher : Undefined indexParams in initialization function. Cannot create matcher.");
        return;
    }

    QString indexParamsType = initParams["indexParams"].toString();
    if ( indexParamsType == "KDTree" ){
        int trees = 4;
        if ( !initParams.contains("trees") )
            trees = initParams["trees"].toInt();

        indexParams = new cv::flann::KDTreeIndexParams(trees);

    } else if ( indexParamsType == "HierarchicalClustering" ) {
        indexParams = new cv::flann::HierarchicalClusteringIndexParams();
    } else if ( indexParamsType == "Lsh" ){
        int tableNumber = 10, keySize = 10, multiProbeLevel = 10;

        if ( initParams.contains("tableNumber") )
            tableNumber = initParams["tableNumber"].toInt();
        if ( initParams.contains("keySize") )
            keySize     = initParams["keySize"].toInt();
        if ( initParams.contains("multiProbeLevel") )
            multiProbeLevel = initParams["multiProbeLevel"].toInt();

        indexParams = new cv::flann::LshIndexParams(tableNumber, keySize, multiProbeLevel);

    } else {
        qWarning("%s", qPrintable(QString(
            QString("FlannBasedMatcher : Unknown indexParams value in initialization function : '") +
            indexParamsType + "'. Cannot create matcher."
        )));
        return;
    }

    initializeMatcher(new cv::FlannBasedMatcher(indexParams));
}

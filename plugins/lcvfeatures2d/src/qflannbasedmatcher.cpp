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

void QFlannBasedMatcher::initialize(const QVariantMap &params){
    cv::flann::IndexParams* indexParams = 0;
    if ( !params.contains("indexParams") ){
        qWarning("FlannBasedMatcher : Undefined indexParams in initialization function. Cannot create matcher.");
        return;
    }

    QString indexParamsType = params["indexParams"].toString();
    if ( indexParamsType == "KDTree" ){
        int trees = 4;
        if ( !params.contains("trees") )
            trees = params["trees"].toInt();

        indexParams = new cv::flann::KDTreeIndexParams(trees);

    } else if ( indexParamsType == "HierarchicalClustering" ) {
        indexParams = new cv::flann::HierarchicalClusteringIndexParams();
    } else if ( indexParamsType == "Lsh" ){
        int tableNumber = 10, keySize = 10, multiProbeLevel = 10;

        if ( params.contains("tableNumber") )
            tableNumber = params["tableNumber"].toInt();
        if ( params.contains("keySize") )
            keySize     = params["keySize"].toInt();
        if ( params.contains("multiProbeLevel") )
            multiProbeLevel = params["multiProbeLevel"].toInt();

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

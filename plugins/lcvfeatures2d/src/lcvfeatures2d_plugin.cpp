/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "lcvfeatures2d_plugin.hpp"
#include "qkeypoint.h"
#include "qkeypointvector.h"
#include "qfeaturedetector.h"
#include "qfastfeaturedetector.h"
#include "qbriskfeaturedetector.h"
#include "qdensefeaturedetector.h"
#include "qgoodfeaturestotrackdetector.h"
#include "qstarfeaturedetector.h"
#include "qorbfeaturedetector.h"
#include "qmserfeaturedetector.h"
#include "qsimpleblobdetector.h"

#include "qdescriptorextractor.h"
#include "qbriefdescriptorextractor.h"
#include "qbriskdescriptorextractor.h"
#include "qorbdescriptorextractor.h"
#include "qfreakdescriptorextractor.h"

#include "qdescriptormatcher.h"
#include "qbruteforcematcher.h"
#include "qflannbasedmatcher.h"

#include "qdmatchvector.h"
#include "qdrawmatches.h"
#include "qdescriptormatchfilter.h"
#include "qkeypointtoscenemap.h"
#include "qmatchestolocalkeypoint.h"
#include "qkeypointhomography.h"

#include <qqml.h>

void Lcvfeatures2dPlugin::registerTypes(const char *uri){
    // @uri modules.lcvfeatures2d
    qmlRegisterType<QKeyPoint>(                   uri, 1, 0, "KeyPoint");
    qmlRegisterType<QKeyPointVector>(             uri, 1, 0, "KeyPointVector");
    qmlRegisterType<QFeatureDetector>(            uri, 1, 0, "FeatureDetector");
    qmlRegisterType<QFastFeatureDetector>(        uri, 1, 0, "FastFeatureDetector");
    qmlRegisterType<QBriskFeatureDetector>(       uri, 1, 0, "BriskFeatureDetector");
    qmlRegisterType<QDenseFeatureDetector>(       uri, 1, 0, "DenseFeatureDetector");
    qmlRegisterType<QGoodFeaturesToTrackDetector>(uri, 1, 0, "GoodFeaturesToTrackFeatureDetector");
    qmlRegisterType<QStarFeatureDetector>(        uri, 1, 0, "StarFeatureDetector");
    qmlRegisterType<QOrbFeatureDetector>(         uri, 1, 0, "OrbFeatureDetector");
    qmlRegisterType<QMSerFeatureDetector>(        uri, 1, 0, "MSerFeatureDetector");
    qmlRegisterType<QSimpleBlobDetector>(         uri, 1, 0, "SimpleBlobDetector");

    qmlRegisterType<QDescriptorExtractor>(        uri, 1, 0, "DescriptorExtractor");
    qmlRegisterType<QBriefDescriptorExtractor>(   uri, 1, 0, "BriefDescriptorExtractor");
    qmlRegisterType<QBriskDescriptorExtractor>(   uri, 1, 0, "BriskDescriptorExtractor");
    qmlRegisterType<QOrbDescriptorExtractor>(     uri, 1, 0, "OrbDescriptorExtractor");
    qmlRegisterType<QFreakDescriptorExtractor>(   uri, 1, 0, "FreakDescriptorExtractor");

    qmlRegisterType<QDMatchVector>(               uri, 1, 0, "DMatchVector");
    qmlRegisterType<QDescriptorMatcher>(          uri, 1, 0, "DescriptorMatcher");
    qmlRegisterType<QBruteForceMatcher>(          uri, 1, 0, "BruteForceMatcher");
    qmlRegisterType<QFlannBasedMatcher>(          uri, 1, 0, "FlannBasedMatcher");
    qmlRegisterType<QDrawMatches>(                uri, 1, 0, "DrawMatches");
    qmlRegisterType<QDescriptorMatchFilter>(      uri, 1, 0, "DescriptorMatchFilter");
    qmlRegisterType<QKeyPointToSceneMap>(         uri, 1, 0, "KeypointToSceneMap");
    qmlRegisterType<QMatchesToLocalKeypoint>(     uri, 1, 0, "MatchesToLocalKeypoint");
    qmlRegisterType<QKeypointHomography>(         uri, 1, 0, "KeypointHomography");
}



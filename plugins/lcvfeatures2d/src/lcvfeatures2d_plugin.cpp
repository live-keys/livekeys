/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "lcvfeatures2d_plugin.h"
#include "qkeypoint.h"
#include "qkeypointvector.h"
#include "qfeaturedetector.h"
#include "qfastfeaturedetector.h"
#include "qbriskfeaturedetector.h"
//#include "qdensefeaturedetector.h"
//#include "qgoodfeaturestotrackdetector.h"
#include "qorbfeaturedetector.h"
#include "qmserfeaturedetector.h"
#include "qsimpleblobdetector.h"

#include "qdescriptorextractor.h"
#include "qbriskdescriptorextractor.h"
#include "qorbdescriptorextractor.h"

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
/// \private
void Lcvfeatures2dPlugin::registerTypes(const char *uri){
    // @uri modules.lcvfeatures2d
    qmlRegisterType<QKeyPoint>(                   uri, 1, 0, "KeyPoint");
    qmlRegisterType<QKeyPointVector>(             uri, 1, 0, "KeyPointVector");
    qmlRegisterType<QFeatureDetector>(            uri, 1, 0, "FeatureDetector");
    qmlRegisterType<QFastFeatureDetector>(        uri, 1, 0, "FastFeatureDetector");
    qmlRegisterType<QBriskFeatureDetector>(       uri, 1, 0, "BriskFeatureDetector");
    qmlRegisterType<QOrbFeatureDetector>(         uri, 1, 0, "OrbFeatureDetector");
    qmlRegisterType<QMSerFeatureDetector>(        uri, 1, 0, "MSerFeatureDetector");
    qmlRegisterType<QSimpleBlobDetector>(         uri, 1, 0, "SimpleBlobDetector");

    qmlRegisterType<QDescriptorExtractor>(        uri, 1, 0, "DescriptorExtractor");
    qmlRegisterType<QBriskDescriptorExtractor>(   uri, 1, 0, "BriskDescriptorExtractor");
    qmlRegisterType<QOrbDescriptorExtractor>(     uri, 1, 0, "OrbDescriptorExtractor");

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



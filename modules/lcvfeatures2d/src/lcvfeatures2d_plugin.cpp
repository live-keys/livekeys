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
#include "QKeyPointVector.hpp"
#include "QFeatureDetector.hpp"
#include "QFastFeatureDetector.hpp"
#include "QBriskFeatureDetector.hpp"
#include "QDenseFeatureDetector.hpp"
#include "QGoodFeaturesToTrackDetector.hpp"
#include "QStarFeatureDetector.hpp"
#include "QOrbFeatureDetector.hpp"
#include "QMSerFeatureDetector.hpp"
#include "QSimpleBlobDetector.hpp"

#include "QDescriptorExtractor.hpp"
#include "QBriefDescriptorExtractor.hpp"
#include "QBriskDescriptorExtractor.hpp"
#include "QOrbDescriptorExtractor.hpp"
#include "QFreakDescriptorExtractor.hpp"

#include "QDMatchVector.hpp"
#include "QDescriptorMatcher.hpp"
#include "QBruteForceMatcher.hpp"
#include "QFlannBasedMatcher.hpp"
#include "QDrawMatches.hpp"

#include <qqml.h>

void Lcvfeatures2dPlugin::registerTypes(const char *uri){
    // @uri modules.lcvfeatures2d
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
}



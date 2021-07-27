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

#include "lcvimgproc_plugin.h"

#include <qqml.h>
#include "qdraw.h"
#include "qcachedwarpperspective.h"
#include "qgeometry.h"
#include "qshapedescriptors.h"
#include "qtransformimage.h"
#include "qtransformations.h"
#include "qcascadeclassifier.h"
#include "qfilteringoperations.h"
#include "colorspace.h"
#include "qfeaturedetection.h"
#include "qprojectionsurface.h"
#include "qprojectionmapper.h"
#include <QQmlEngine>


static QObject* drawProvider(QQmlEngine *engine, QJSEngine *){
    return new QDraw(engine);
}
static QObject* geometryProvider(QQmlEngine *engine, QJSEngine *){
    return new QGeometry(engine);
}

static QObject* shapeDescriptorsProvider(QQmlEngine *engine, QJSEngine *){
    return new QShapeDescriptors(engine);
}

static QObject* transformationsProvider(QQmlEngine *engine, QJSEngine *){
    return new QTransformations(engine);
}

static QObject* filteringOperationsProvider(QQmlEngine *engine, QJSEngine *){
    return new QFilteringOperations(engine);
}

static QObject* colorSpaceProvider(QQmlEngine *engine, QJSEngine *){
    return new ColorSpace(engine);
}

static QObject* featureDetectionProvider(QQmlEngine *engine, QJSEngine *){
    return new QFeatureDetection(engine);
}

void LcvimgprocPlugin::registerTypes(const char *uri){
    // @uri modules.lcvimgproc
    qmlRegisterType<QCachedWarpPerspective>(  uri, 1, 0, "CachedWarpPerspective");
    qmlRegisterType<QTransformImage>(         uri, 1, 0, "TransformImage");
    qmlRegisterType<QCascadeClassifier>(      uri, 1, 0, "CascadeClassifier");
    qmlRegisterType<QProjectionSurface>(      uri, 1, 0, "ProjectionSurface");
    qmlRegisterType<QProjectionMapper>(       uri, 1, 0, "ProjectionMapper");

    qmlRegisterSingletonType<QDraw>( uri, 1, 0, "Draw", &drawProvider);
    qmlRegisterSingletonType<QGeometry>( uri, 1, 0, "Geometry", &geometryProvider);
    qmlRegisterSingletonType<QShapeDescriptors>( uri, 1, 0, "ShapeDescriptors",
                                                 &shapeDescriptorsProvider);
    qmlRegisterSingletonType<QTransformations>(uri, 1, 0, "Transformations", &transformationsProvider);
    qmlRegisterSingletonType<QFilteringOperations>( uri, 1, 0, "FilteringOperations",
                                                 &filteringOperationsProvider);
    qmlRegisterSingletonType<ColorSpace>( uri, 1, 0, "ColorSpace", &colorSpaceProvider);
    qmlRegisterSingletonType<QFeatureDetection>( uri, 1, 0, "FeatureDetection", &featureDetectionProvider);

}



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

#include "lcvcore_plugin.h"
#include "qmat.h"
#include "qwritablemat.h"
#include "qmatop.h"
#include "qmatio.h"
#include "qmatext.h"
#include "qmatview.h"
#include "qimread.h"
#include "qimwrite.h"
#include "qimageview.h"
#include "qmatroi.h"
#include "qmatread.h"
#include "qcamcapture.h"
#include "qvideocapture.h"
#include "qvideowriter.h"
#include "qmatbuffer.h"
#include "qalphamerge.h"
#include "qabsdiff.h"
#include "qdrawhistogram.h"
#include "qcolorhistogram.h"
#include "qmatloader.h"
#include "qimagefile.h"
#include "qoverlapmat.h"
#include "qitemcapture.h"
#include "qgradient.h"
#include "qvideodecoder.h"

#include "videosegment.h"
#include "videosurface.h"

#include "live/viewengine.h"
#include "live/viewcontext.h"

#include <qqml.h>
#include <QQmlEngine>


static QObject* matOpProvider(QQmlEngine *engine, QJSEngine *){
    return new QMatOp(engine);
}

static QObject* matIOProvider(QQmlEngine *engine, QJSEngine *){
    return new QMatIO(engine);
}

static QObject* gradientProvider(QQmlEngine *engine, QJSEngine *){
    return new QGradient(engine);
}

void LcvcorePlugin::registerTypes(const char *uri){
    // @uri modules.lcvcore
    qmlRegisterType<QMat>(                   uri, 1, 0, "Mat");
    qmlRegisterType<QWritableMat>(           uri, 1, 0, "WritableMat");
    qmlRegisterType<QMatView>(               uri, 1, 0, "MatView");
    qmlRegisterType<QImageView>(             uri, 1, 0, "ImageView");
    qmlRegisterType<QImRead>(                uri, 1, 0, "ImRead");
    qmlRegisterType<QImWrite>(               uri, 1, 0, "ImWrite");
    qmlRegisterType<QMatRoi>(                uri, 1, 0, "MatRoi");
    qmlRegisterType<QMatRoi>(                uri, 1, 0, "Crop");
    qmlRegisterType<QMatRead>(               uri, 1, 0, "MatRead");
    qmlRegisterType<QCamCapture>(            uri, 1, 0, "CamCapture");
    qmlRegisterType<QVideoCapture>(          uri, 1, 0, "VideoCapture");
    qmlRegisterType<QVideoWriter>(           uri, 1, 0, "VideoWriter");
    qmlRegisterType<QMatBuffer>(             uri, 1, 0, "MatBuffer");
    qmlRegisterType<QAlphaMerge>(            uri, 1, 0, "AlphaMerge");
    qmlRegisterType<QAbsDiff>(               uri, 1, 0, "AbsDiff");
    qmlRegisterType<QDrawHistogram>(         uri, 1, 0, "DrawHistogram");
    qmlRegisterType<QColorHistogram>(        uri, 1, 0, "ColorHistogram");
    qmlRegisterType<QMatLoader>(             uri, 1, 0, "MatLoader");
    qmlRegisterType<QImageFile>(             uri, 1, 0, "ImageFile");
    qmlRegisterType<QOverlapMat>(            uri, 1, 0, "OverlapMat");
    qmlRegisterType<QItemCapture>(           uri, 1, 0, "ItemCapture");

    qmlRegisterType<QVideoDecoder>(          uri, 1, 0, "VideoDecoder");

    qmlRegisterType<lv::VideoSurface>(       uri, 1, 0, "VideoSurface");
    qmlRegisterType<lv::VideoSegment>(       uri, 1, 0, "VideoSegment");

    qmlRegisterSingletonType<QMatOp>(        uri, 1, 0, "MatOp",        &matOpProvider);
    qmlRegisterSingletonType<QMatIO>(        uri, 1, 0, "MatIO",        &matIOProvider);
    qmlRegisterSingletonType<QGradient>(     uri, 1, 0, "Gradient",   &gradientProvider);

}

void LcvcorePlugin::initializeEngine(QQmlEngine *, const char *){
    lv::ViewContext::instance().engine()->registerQmlTypeInfo<QMat>(
        &lv::ml::serialize<QMat>,
        &lv::ml::deserialize<QMat>,
        [](){return new QMat;},
        true
    );
}



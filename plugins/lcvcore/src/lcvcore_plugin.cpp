/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "lcvcore_plugin.h"
#include "qcvglobalobject.h"
#include "qmat.h"
#include "qmatview.h"
#include "qimread.h"
#include "qimwrite.h"
#include "qmatempty.h"
#include "qmatroi.h"
#include "qmatread.h"
#include "qmat2darray.h"
#include "qcamcapture.h"
#include "qvideocapture.h"
#include "qvideowriter.h"
#include "qmatbuffer.h"
#include "qalphamerge.h"
#include "qabsdiff.h"
#include "qdrawhistogram.h"
#include "qcolorhistogram.h"
#include "qmatloader.h"
#include "qmatlist.h"

#include <qqml.h>
#include <QQmlEngine>

void LcvcorePlugin::registerTypes(const char *uri){
    // @uri modules.lcvcore
    qmlRegisterUncreatableType<QCvGlobalObject>(
        uri, 1, 0, "CvGlobalObject", "CvGlobal Object is available through the \'cv\' property");

    qmlRegisterType<QMat>(               uri, 1, 0, "Mat");
    qmlRegisterType<QMatView>(           uri, 1, 0, "MatView");
    qmlRegisterType<QImRead>(            uri, 1, 0, "ImRead");
    qmlRegisterType<QImWrite>(           uri, 1, 0, "ImWrite");
    qmlRegisterType<QMatEmpty>(          uri, 1, 0, "MatEmpty");
    qmlRegisterType<QMatRoi>(            uri, 1, 0, "MatRoi");
    qmlRegisterType<QMatRead>(           uri, 1, 0, "MatRead");
    qmlRegisterType<QMat2DArray>(        uri, 1, 0, "Mat2DArray");
    qmlRegisterType<QCamCapture>(        uri, 1, 0, "CamCapture");
    qmlRegisterType<QVideoCapture>(      uri, 1, 0, "VideoCapture");
    qmlRegisterType<QVideoWriter>(       uri, 1, 0, "VideoWriter");
    qmlRegisterType<QMatBuffer>(         uri, 1, 0, "MatBuffer");
    qmlRegisterType<QAlphaMerge>(        uri, 1, 0, "AlphaMerge");
    qmlRegisterType<QAbsDiff>(           uri, 1, 0, "AbsDiff");
    qmlRegisterType<QDrawHistogram>(     uri, 1, 0, "DrawHistogram");
    qmlRegisterType<QColorHistogram>(    uri, 1, 0, "ColorHistogram");
    qmlRegisterType<QMatList>(           uri, 1, 0, "MatList");
    qmlRegisterType<QMatLoader>(         uri, 1, 0, "MatLoader");
}

void LcvcorePlugin::initializeEngine(QQmlEngine *engine, const char *){
    QCvGlobalObject* cvob = new QCvGlobalObject;
    engine->globalObject().setProperty("cv", engine->newQObject(cvob));
}



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

#include "lcvphoto_plugin.h"

#include <qqml.h>
#include <QQmlEngine>

#include "qstitcher.h"

#include "qalignmtb.h"
#include "qcalibratedebevec.h"
#include "qcalibraterobertson.h"
#include "qmergedebevec.h"
#include "qmergerobertson.h"

#include "qtonemap.h"
#include "qtonemapdrago.h"
#include "qtonemapmantiuk.h"
#include "qtonemapreinhard.h"

#include "qdenoising.h"
#include "qadjustments.h"

static QObject* denoisingProvider(QQmlEngine* engine, QJSEngine*){
    return new QDenoising(engine);
}

static QObject* adjustmentsProvider(QQmlEngine* engine, QJSEngine*){
    return new QAdjustments(engine);
}

void LcvphotoPlugin::registerTypes(const char *uri){
    // @uri modules.lcvphoto

    qmlRegisterType<QStitcher>(                       uri, 1, 0, "StitcherInternal");

    qmlRegisterType<QAlignMTB>(                       uri, 1, 0, "AlignMTBInternal");
    qmlRegisterType<QCalibrateDebevec>(               uri, 1, 0, "CalibrateDebevecInternal");
    qmlRegisterType<QCalibrateRobertson>(             uri, 1, 0, "CalibrateRobertsonInternal");
    qmlRegisterType<QMergeDebevec>(                   uri, 1, 0, "MergeDebevecInternal");
    qmlRegisterType<QMergeRobertson>(                 uri, 1, 0, "MergeRobertsonInternal");
    qmlRegisterType<QTonemapDrago>(                   uri, 1, 0, "TonemapDragoInternal");
    qmlRegisterType<QTonemapMantiuk>(                 uri, 1, 0, "TonemapMantiukInternal");
    qmlRegisterType<QTonemapReinhard>(                uri, 1, 0, "TonemapReinhardInternal");

    qmlRegisterSingletonType<QDenoising>(             uri, 1, 0, "Denoising", &denoisingProvider);
    qmlRegisterSingletonType<QAdjustments>(           uri, 1, 0, "Adjustments", &adjustmentsProvider);
}

/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "lcvphoto_plugin.h"

#include <qqml.h>
#include "qfastnlmeansdenoisingmulti.h"
#include "qdenoisetvl1.h"
#include "qhuesaturationlightness.h"
#include "qlevels.h"
#include "qlevelsserializer.h"
#include "qautolevels.h"
#include "qbrightnessandcontrast.h"
#include "qbrightnessandcontrastserializer.h"
#include "qstitcher.h"

#include "qalignmtb.h"
#include "qcalibratedebevec.h"
#include "qcalibraterobertson.h"
#include "qmergedebevec.h"
#include "qmergerobertson.h"

#include "qtonemap.h"
#include "qtonemapdrago.h"
#include "qtonemapdurand.h"
#include "qtonemapmantiuk.h"
#include "qtonemapreinard.h"

void LcvphotoPlugin::registerTypes(const char *uri){
    // @uri modules.lcvphoto
    qmlRegisterType<QFastNlMeansDenoising>(           uri, 1, 0, "FastNlMeansDenoising");
    qmlRegisterType<QFastNlMeansDenoisingMulti>(      uri, 1, 0, "FastNlMeansDenoisingMulti");
    qmlRegisterType<QDenoiseTvl1>(                    uri, 1, 0, "DenoiseTvl1");
    qmlRegisterType<QHueSaturationLightness>(         uri, 1, 0, "HueSaturationLightness");
    qmlRegisterType<QLevels>(                         uri, 1, 0, "Levels");
    qmlRegisterType<QAutoLevels>(                     uri, 1, 0, "AutoLevels");
    qmlRegisterType<QLevelsSerializer>(               uri, 1, 0, "LevelsSerializer");
    qmlRegisterType<QBrightnessAndContrast>(          uri, 1, 0, "BrightnessAndContrast");
    qmlRegisterType<QBrightnessAndContrastSerializer>(uri, 1, 0, "BrightnessAndContrastSerializer");
    qmlRegisterType<QStitcher>(                       uri, 1, 0, "Stitcher");

    qmlRegisterType<QAlignMTB>(                       uri, 1, 0, "AlignMTB");
    qmlRegisterType<QCalibrateDebevec>(               uri, 1, 0, "CalibrateDebevec");
    qmlRegisterType<QCalibrateRobertson>(             uri, 1, 0, "CalibrateRobertson");
    qmlRegisterType<QMergeDebevec>(                   uri, 1, 0, "MergeDebevec");
    qmlRegisterType<QMergeRobertson>(                 uri, 1, 0, "MergeRobertson");
    qmlRegisterType<QTonemap>(                        uri, 1, 0, "Tonemap");
    qmlRegisterType<QTonemapDrago>(                   uri, 1, 0, "TonemapDrago");
    qmlRegisterType<QTonemapDurand>(                  uri, 1, 0, "TonemapDurand");
    qmlRegisterType<QTonemapMantiuk>(                 uri, 1, 0, "TonemapMantiuk");
    qmlRegisterType<QTonemapReinard>(                 uri, 1, 0, "TonemapReinard");
}

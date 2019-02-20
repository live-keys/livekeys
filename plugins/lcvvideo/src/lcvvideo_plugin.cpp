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

#include "lcvvideo_plugin.h"

#include <qqml.h>
#include "qcalcopticalflowpyrlk.h"
#include "qbackgroundsubtractormog2.h"
#include "qbackgroundsubtractorknn.h"
/// \private
void LcvvideoPlugin::registerTypes(const char *uri){
    // @uri modules.lcvvideo
    qmlRegisterType<QCalcOpticalFlowPyrLK>(     uri, 1, 0, "CalcOpticalFlowPyrLK");
    qmlRegisterType<QBackgroundSubtractor>(     uri, 1, 0, "BackgroundSubtractor");
    qmlRegisterType<QBackgroundSubtractorMog2>( uri, 1, 0, "BackgroundSubtractorMog2");
    qmlRegisterType<QBackgroundSubtractorKnn>(  uri, 1, 0, "BackgroundSubtractorKnn");
}

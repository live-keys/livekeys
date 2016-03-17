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

#include "lcvimgproc_plugin.hpp"

#include <qqml.h>
#include "qblur.h"
#include "qcanny.h"
#include "qgaussianblur.h"
#include "qhoughlines.h"
#include "qhoughlinesp.h"
#include "qsobel.h"
#include "qcvtcolor.h"
#include "qfilter2d.h"
#include "qthreshold.h"
#include "qchannelselect.h"
#include "qstructuringelement.h"
#include "qmatresize.h"
#include "qdilate.h"
#include "qerode.h"
#include "qcopymakeborder.h"

void LcvimgprocPlugin::registerTypes(const char *uri){
    // @uri modules.lcvimgproc
    qmlRegisterType<QBlur>(               uri, 1, 0, "Blur");
    qmlRegisterType<QCanny>(              uri, 1, 0, "Canny");
    qmlRegisterType<QGaussianBlur>(       uri, 1, 0, "GaussianBlur");
    qmlRegisterType<QHoughLines>(         uri, 1, 0, "HoughLines");
    qmlRegisterType<QHoughLinesP>(        uri, 1, 0, "HoughLinesP");
    qmlRegisterType<QSobel>(              uri, 1, 0, "Sobel");
    qmlRegisterType<QCvtColor>(           uri, 1, 0, "CvtColor");
    qmlRegisterType<QThreshold>(          uri, 1, 0, "Threshold");
    qmlRegisterType<QChannelSelect>(      uri, 1, 0, "ChannelSelect");
    qmlRegisterType<QStructuringElement>( uri, 1, 0, "StructuringElement");
    qmlRegisterType<QMatResize>(          uri, 1, 0, "Resize");
    qmlRegisterType<QFilter2D>(           uri, 1, 0, "Filter2D");
    qmlRegisterType<QDilate>(             uri, 1, 0, "Dilate");
    qmlRegisterType<QErode>(              uri, 1, 0, "Erode");
    qmlRegisterType<QCopyMakeBorder>(     uri, 1, 0, "CopyMakeBorder");
}



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

#include "lcvcore_plugin.hpp"
#include "QMat.hpp"
#include "QMatSource.hpp"
#include "QMatEmpty.hpp"
#include "QMatRoi.hpp"
#include "QMatRead.hpp"
#include "QMat2DArray.hpp"
#include "QCamCapture.hpp"
#include "QVideoCapture.hpp"
#include "QMatDraw.hpp"
#include "QMatBuffer.hpp"

#include <qqml.h>

void LcvcorePlugin::registerTypes(const char *uri){
    // @uri modules.lcvcore
    qmlRegisterType<QMat>          (uri, 1, 0, "QMat");
    qmlRegisterType<QMat>          (uri, 1, 0, "Mat");
    qmlRegisterType<QMatSource>    (uri, 1, 0, "MatSource");
    qmlRegisterType<QMatEmpty>     (uri, 1, 0, "MatEmpty");
    qmlRegisterType<QMatRoi>       (uri, 1, 0, "MatRoi");
    qmlRegisterType<QMatRead>      (uri, 1, 0, "MatRead");
    qmlRegisterType<QMat2DArray>   (uri, 1, 0, "Mat2DArray");
    qmlRegisterType<QCamCapture>   (uri, 1, 0, "CamCapture");
    qmlRegisterType<QVideoCapture> (uri, 1, 0, "VideoCapture");
    qmlRegisterType<QMatDraw>      (uri, 1, 0, "MatDraw");
    qmlRegisterType<QMatBuffer>    (uri, 1, 0, "MatBuffer");
}



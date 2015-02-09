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

#include <qqml.h>

void Lcvfeatures2dPlugin::registerTypes(const char *uri){
    // @uri modules.lcvfeatures2d
    qmlRegisterType<QKeyPointVector>(     uri, 1, 0, "KeyPointVector");
    qmlRegisterType<QFeatureDetector>(    uri, 1, 0, "FeatureDetector");
    qmlRegisterType<QFastFeatureDetector>(uri, 1, 0, "FastFeatureDetector");
}



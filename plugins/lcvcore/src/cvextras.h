/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVCVEXTRAS_H
#define LVCVEXTRAS_H

#include "opencv2/core.hpp"
#include "live/exception.h"
#include "live/qmlerror.h"
#include "qlcvcoreglobal.h"

namespace lv{

class Q_LCVCORE_EXPORT CvExtras{

public:
    CvExtras();

    static void copyTo4Channels(cv::Mat& src, cv::Mat& dst);
    static void blendTo3Channels(cv::Mat& src, cv::Mat& dst);

    template<typename T>
    static T toLocalException(const cv::Exception& e, const std::string& prefix = "", StackTrace::Ptr stack = nullptr);

    static lv::QmlError toLocalError(const cv::Exception& e, ViewEngine* engine, QObject *object, const std::string& prefix = "", StackTrace::Ptr stack = nullptr);
};

inline QmlError CvExtras::toLocalError(const cv::Exception &e, ViewEngine *engine, QObject* object, const std::string &prefix, StackTrace::Ptr stack){
    lv::Exception ex = lv::CvExtras::toLocalException<lv::Exception>(e, prefix, stack);
    return QmlError(engine, ex, object);
}

template<typename T>
T CvExtras::toLocalException(const cv::Exception &e, const std::string &prefix, StackTrace::Ptr stack){
    return lv::Exception::create<T>(
        prefix + e.what(), e.code, e.file, e.line, e.func, stack
    );
}

}// namespace

#endif // CVEXTRAS_H

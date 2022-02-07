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

#ifndef QMAT_EXT_H
#define QMAT_EXT_H

#include "qmat.h"
#include "live/visuallog.h"
#include "live/mlnode.h"
#include "qlcvcoreglobal.h"

namespace lv{ namespace ml{

template<>
inline void serialize<QMat>(const QMat& v, MLNode& node){
    node = {
        {"cols", v.internal().cols},
        {"rows", v.internal().rows},
        {"channels", v.internal().channels()},
        {"depth", v.internal().depth()},
        {"data", MLNode::BytesType(v.internal().data, v.internal().total() * v.internal().elemSize())}
    };
}

template<>
inline void deserialize<QMat>(const MLNode& node, QMat& v){
    *v.internalPtr() = cv::Mat(
        cv::Size(node["cols"].asInt(), node["rows"].asInt()),
        CV_MAKETYPE(node["depth"].asInt(), node["channels"].asInt()),
        cv::Mat::AUTO_STEP
    );
    lv::MLNode::BytesType bt = node["data"].asBytes();
    memcpy(v.internalPtr()->data, bt.data(), bt.size());
}

} // namespace ml

class QmlObjectList;
Q_LCVCORE_EXPORT QmlObjectList* createMatList();

inline lv::VisualLog& operator << (lv::VisualLog& vl, const QMat& v){
    vl.asObject("QMat", v);
    vl.asView("lcvcore/MatLog.qml", [&v](){ return QVariant::fromValue(v.clone()); });
    return vl << "Mat[" << v.internal().cols << "x" << v.internal().rows << "," << v.internal().channels() << "]";
}

} // namespace lv

#endif // QMAT_EXT_H

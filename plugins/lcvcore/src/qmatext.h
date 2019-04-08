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

#ifndef QMAT_EXT_H
#define QMAT_EXT_H

#include "qmat.h"
#include "live/visuallog.h"
#include "live/mlnode.h"

namespace lv{ namespace ml{

template<>
inline void serialize<QMat>(const QMat& v, MLNode& node){
    node = {
        {"cols", v.data().cols},
        {"rows", v.data().rows},
        {"channels", v.data().channels()},
        {"depth", v.data().depth()},
        {"data", MLNode::BytesType(v.data().data, v.data().total() * v.data().elemSize())}
    };
}

template<>
inline void deserialize<QMat>(const MLNode& node, QMat& v){
    *v.cvMat() = cv::Mat(
        cv::Size(node["cols"].asInt(), node["rows"].asInt()),
        CV_MAKETYPE(node["depth"].asInt(), node["channels"].asInt()),
        cv::Mat::AUTO_STEP
    );
    lv::MLNode::BytesType bt = node["data"].asBytes();
    memcpy(v.cvMat()->data, bt.data(), bt.size());
}

} // namespace ml

class QmlObjectList;
Q_LCVCORE_EXPORT QmlObjectList* createMatList();

inline lv::VisualLog& operator << (lv::VisualLog& vl, const QMat& v){
    vl.asObject("QMat", v);
    vl.asView("lcvcore/MatLog.qml", [&v](){ return QVariant::fromValue(v.clone()); });
    return vl << "Mat[" << v.data().cols << "x" << v.data().rows << "," << v.data().channels() << "]";
}

} // namespace lv

#endif // QMAT_EXT_H

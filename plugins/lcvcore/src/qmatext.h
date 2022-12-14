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
        {"data", MLNode::BytesType(reinterpret_cast<const MLNode::BytesType::Byte*>(v.internal().data), v.internal().total() * v.internal().elemSize())}
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


class QMatLogBehavior{
public:
    static std::string typeId(const QMat&){
        return "lcvcore@QMat";
    }
    static std::string defaultViewDelegate(const QMat&){
        return "lcvcore@MatLog";
    }
    static bool hasViewObject(){
        return true;
    }
    static bool hasTransport(){
        return true;
    }
    static lv::VisualLog::ViewObject* toView(const QMat& m){
        return m.clone();
    }
    static bool toTransport(const QMat& m, lv::MLNode& output){
        lv::ml::serialize<QMat>(m, output);
        return true;
    }
    static void toStream(lv::VisualLog& vl, const QMat& v){
        vl << "Mat[" << v.internal().cols << "x" << v.internal().rows << "," << v.internal().channels() << "]";
    }
};

inline lv::VisualLog& operator << (lv::VisualLog& vl, const QMat& v){
    return vl.behavior<QMatLogBehavior>(v);
}

} // namespace lv

#endif // QMAT_EXT_H

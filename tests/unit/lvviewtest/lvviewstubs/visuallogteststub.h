/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#ifndef VISUALLOGTESTSTUB_H
#define VISUALLOGTESTSTUB_H

#include <QObject>
#include "live/mlnode.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"

class VisualLogTestStub : public QObject{

    Q_OBJECT

public:
    explicit VisualLogTestStub(QObject *parent = 0);
    VisualLogTestStub(int field1, const QString& field2, QObject *parent = 0);
    ~VisualLogTestStub();

    friend void lv::ml::serialize<VisualLogTestStub>(const VisualLogTestStub& d, lv::MLNode& node);
    friend void lv::ml::deserialize<VisualLogTestStub>(const lv::MLNode& node, VisualLogTestStub& d);
    friend lv::VisualLog& operator << (lv::VisualLog& vl, const VisualLogTestStub& v);

public:
    class LogBehavior{
    public:
        static std::string typeId(const VisualLogTestStub&){
            return "@VisualLogTestStub";
        }
        static std::string defaultViewDelegate(const VisualLogTestStub&){
            return std::string();
        }
        static bool hasViewObject(){
            return false;
        }
        static bool hasTransport(){
            return true;
        }
        static lv::VisualLog::ViewObject* toView(const VisualLogTestStub&){
            return nullptr;
        }
        static bool toTransport(const VisualLogTestStub& m, lv::MLNode& output);
        static void toStream(lv::VisualLog& vl, const VisualLogTestStub& v){
            vl << v.m_field1 << " " << v.m_field2;
        }
    };

private:
    int     m_field1;
    QString m_field2;
};

namespace lv{ namespace ml{

template<>
inline void serialize<VisualLogTestStub>(const VisualLogTestStub& v, MLNode& node){
    node = {
        {"field1", v.m_field1},
        {"field2", v.m_field2.toStdString()}
    };
}

template<>
inline void deserialize<VisualLogTestStub>(const MLNode& node, VisualLogTestStub& v){
    v.m_field1 = node["field1"].asInt();
    v.m_field2 = QString::fromStdString(node["field2"].asString());
}

}// namespace ml
}// namespace lv

inline bool VisualLogTestStub::LogBehavior::toTransport(const VisualLogTestStub &m, lv::MLNode &output){
    lv::ml::serialize(m, output);
    return true;
}


inline lv::VisualLog& operator << (lv::VisualLog& vl, const VisualLogTestStub& v){
    return vl.behavior<VisualLogTestStub::LogBehavior>(v);
}

#endif // VISUALLOGTESTSTUB_H

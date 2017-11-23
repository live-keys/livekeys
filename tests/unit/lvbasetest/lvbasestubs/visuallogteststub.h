/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef VISUALLOGTESTSTUB_H
#define VISUALLOGTESTSTUB_H

#include <QObject>
#include "live/mlnode.h"
#include "live/visuallog.h"

class VisualLogTestStub : public QObject{

    Q_OBJECT

public:
    explicit VisualLogTestStub(QObject *parent = 0);
    VisualLogTestStub(int field1, const QString& field2, QObject *parent = 0);
    ~VisualLogTestStub();

    friend void lv::ml::serialize<VisualLogTestStub>(lv::MLNode& node, const VisualLogTestStub& d);
    friend void lv::ml::deserialize<VisualLogTestStub>(const lv::MLNode& node, VisualLogTestStub& d);
    friend lv::VisualLog& operator << (lv::VisualLog& vl, const VisualLogTestStub& v);

private:
    int     m_field1;
    QString m_field2;
};

inline void lv::ml::serialize<VisualLogTestStub>(MLNode& node, const VisualLogTestStub& v){
    node = {
        {"field1", v.m_field1},
        {"field2", v.m_field2.toStdString()}
    };
}

inline void lv::ml::deserialize<VisualLogTestStub>(const MLNode& node, VisualLogTestStub& v){
    v.m_field1 = node["field1"].asInt();
    v.m_field2 = QString::fromStdString(node["field2"].asString());
}

inline lv::VisualLog& operator << (lv::VisualLog& vl, const VisualLogTestStub& v){
    vl.asObject("VisualLogTestStub", v);
    return vl << v.m_field1 << " " << v.m_field2;
}

#endif // VISUALLOGTESTSTUB_H

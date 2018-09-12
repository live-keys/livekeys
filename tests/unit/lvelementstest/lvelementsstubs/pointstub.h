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

#ifndef POINTSTUB_H
#define POINTSTUB_H

#include "live/elements/element.h"

class PointStub : public lv::el::Element{

public:
    PointStub(lv::el::Engine* engine);

    int x() const;
    int y() const;
    void setX(int x);
    void setY(int y);

    double distanceTo(PointStub* ps);

//    static const lv::script::MetaObject& metaObject();

private:
    int m_x;
    int m_y;
};

inline int PointStub::x() const{
    return m_x;
}

inline int PointStub::y() const{
    return m_y;
}

inline void PointStub::setX(int x){
    m_x = x;
}

inline void PointStub::setY(int y){
    m_y = y;
}

//const lv::script::MetaObject &PointStub::metaObject(){
//    static lv::script::MetaObject mo;



//    return mo;
//}

#endif // POINTSTUB_H

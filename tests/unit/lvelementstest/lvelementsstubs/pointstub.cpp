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

#include "pointstub.h"
#include "math.h"

PointStub::PointStub(lv::el::Engine *engine)
    : Element(engine), m_x(0), m_y(0){

}


double PointStub::distanceTo(PointStub *ps){
    return sqrt(pow(m_x - ps->x(), 2) - pow(m_y - ps->y(), 2));
}

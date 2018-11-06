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

#include "typeinfo.h"

namespace lv{

TypeInfo::TypeInfo(const QByteArray &name)
    : m_name(name)
    , m_serialize(nullptr)
    , m_log(nullptr)
{
}

TypeInfo::~TypeInfo(){

}

TypeInfo::Ptr TypeInfo::create(const QByteArray &name){
    return TypeInfo::Ptr(new TypeInfo(name));
}

}// namespace

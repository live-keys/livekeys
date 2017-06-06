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

#include "qlivepalette.h"

namespace lcv{

QLivePalette::QLivePalette(QObject *parent)
    : QCodeConverter(parent)
    , m_item(0)
    , m_codeChange(false)
{
}

QLivePalette::~QLivePalette(){
}

void QLivePalette::initPallete(const QVariant &value){
    m_codeChange = true;
    m_value = value;
    emit init(value);
    m_codeChange = false;
}

void QLivePalette::setValueFromCode(const QVariant &value){
    m_codeChange = true;
    m_value = value;
    emit init(value);
    m_codeChange = false;
}

}// namespace

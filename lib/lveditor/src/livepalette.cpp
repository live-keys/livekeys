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

#include "live/livepalette.h"
#include <QFileInfo>

namespace lv{

LivePalette::LivePalette(QObject *parent)
    : CodeConverter(parent)
    , m_codeChange(false)
    , m_item(0)
{
}

LivePalette::~LivePalette(){
}

void LivePalette::initPallete(const QVariant &value){
    m_codeChange = true;
    m_value = value;
    emit init(value);
    m_codeChange = false;
}

void LivePalette::setValueFromCode(const QVariant &value){
    m_codeChange = true;
    m_value = value;
    emit init(value);
    m_codeChange = false;
}

QString LivePalette::name() const{
    return QFileInfo(m_path).baseName();
}

}// namespace

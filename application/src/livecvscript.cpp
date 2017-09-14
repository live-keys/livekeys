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

#include "livecvscript.h"
#include "environment.h"
#include "live/qprojectdocument.h"
#include "live/qprojectfile.h"

namespace lcv{

LiveCVScript::LiveCVScript(const QStringList &argvTail, QObject *parent)
    : QObject(parent)
    , m_argvTail(argvTail)
    , m_environment(new Environment(this))
{
    m_argv.append("");
    m_argv.append(m_argvTail);
}

LiveCVScript::~LiveCVScript(){
    delete m_environment;
}

void LiveCVScript::scriptChanged(QProjectDocument *active){
    if ( active ){
        m_argv[0] = active->file()->name();
    } else {
        m_argv[0] = "";
    }
}

}// namespace

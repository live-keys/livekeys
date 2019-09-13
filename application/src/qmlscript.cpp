/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "qmlscript.h"
#include "environment.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/runnable.h"

#include <QFileInfo>

namespace lv{

QmlScript::QmlScript(const QStringList &argvTail, QObject *parent)
    : QObject(parent)
    , m_argvTail(argvTail)
    , m_environment(new Environment(this))
{
    m_argv.append("");
    m_argv.append(m_argvTail);
}

QmlScript::QmlScript(const std::vector<std::string> &argvTail, QObject *parent)
    : QObject(parent)
    , m_environment(new Environment(this))
{
    for ( auto it = argvTail.begin(); it != argvTail.end(); ++it ){
        m_argvTail.append(QString::fromStdString(*it));
    }
    m_argv.append("");
    m_argv.append(m_argvTail);
}

QmlScript::~QmlScript(){
    delete m_environment;
}

void QmlScript::scriptChanged(lv::Runnable *active){
    if ( active ){
        QFileInfo finfo(active->path());
        m_argv[0] = finfo.fileName();
    } else {
        m_argv[0] = "";
    }
}

}// namespace

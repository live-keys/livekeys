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

#include "qmlscript.h"
#include "environment.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/runnable.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include <QFileInfo>
#include <QQmlEngine>

namespace lv{

QmlScript::QmlScript(QQmlEngine* engine, const std::vector<std::string> &argvTail, QObject *parent)
    : QObject(parent)
    , m_environment(new Environment(this))
{
    m_argv = engine->newArray(static_cast<quint32>(argvTail.size() + 1));
    m_argvTail = engine->newArray(static_cast<quint32>(argvTail.size()));

    for ( quint32 i = 0; i < static_cast<quint32>(argvTail.size()); ++i ){
        m_argv.setProperty(i + 1, QString::fromStdString(argvTail[static_cast<size_t>(i)]));
        m_argvTail.setProperty(i, QString::fromStdString(argvTail[static_cast<size_t>(i)]));
    }
}

QmlScript::~QmlScript(){
    delete m_environment;
}

void QmlScript::scriptChanged(lv::Runnable *active){
    if ( active ){
        QFileInfo finfo(active->path());
        m_argv.setProperty(0, finfo.fileName());
    } else {
        m_argv.setProperty(0, "");
    }
}

}// namespace

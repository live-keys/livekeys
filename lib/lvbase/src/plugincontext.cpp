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

#include "live/plugincontext.h"
#include "live/exception.h"
#include "live/engine.h"
#include "live/settings.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QCoreApplication>
#include <QDir>

namespace lv{

QString       PluginContext::m_applicationFilePath = "";
lv::Engine*   PluginContext::m_engine   = 0;
lv::Settings* PluginContext::m_settings = 0;

void PluginContext::initFromEngine(QQmlEngine *engine){
    QObject* livecv = engine->rootContext()->contextProperty("livecv").value<QObject*>();
    if ( !livecv )
        THROW_EXCEPTION(lv::Exception, "Failed to load livecv context property.", 0);

    m_engine   = qobject_cast<lv::Engine*>(livecv->property("engine").value<QObject*>());
    m_settings = qobject_cast<lv::Settings*>(livecv->property("settings").value<QObject*>());
    if ( !m_engine || !m_settings )
        THROW_EXCEPTION(lv::Exception, "Failed to load properties from context", 0);
}

QString PluginContext::executableDirPath(){
    return QCoreApplication::applicationDirPath();
}

QString PluginContext::applicationPath(){
#ifdef Q_OS_DARWIN
    return QDir(QFileInfo(applicationFilePath()).path() + "/..").absolutePath();
#else
    return QFileInfo(applicationFilePath()).path();
#endif
}

QString PluginContext::applicationFilePath(){
    if ( m_applicationFilePath.isEmpty() )
        m_applicationFilePath = applicationFilePathImpl();

    return m_applicationFilePath;
}

QString PluginContext::linkPath(){
#ifdef Q_OS_DARWIN
    return applicationPath() + "/Link";
#else
    return applicationPath() + "/link";
#endif
}

QString PluginContext::pluginPath(){
#ifdef Q_OS_DARWIN
    return applicationPath() + "/PlugIns";
#else
    return applicationPath() + "/plugins";
#endif
}

QString PluginContext::externalPath(){
#ifdef Q_OS_DARWIN
    return applicationPath() + "/External";
#else
    return applicationPath() + "/external";
#endif
}

QString PluginContext::librariesPath(){
#ifdef Q_OS_DARWIN
    return applicationPath() + "/Libraries";
#else
    return applicationPath() + "/libraries";
#endif
}

QString PluginContext::developmentPath(){
#ifdef Q_OS_DARWIN
    return applicationPath() + "/Dev";
#else
    return applicationPath() + "/dev";
#endif
}

QString PluginContext::configPath(){
    return applicationPath() + "/config";
}


}// namespace

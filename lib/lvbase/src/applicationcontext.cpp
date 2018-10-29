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

#include "live/applicationcontext.h"
#include "live/exception.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QCoreApplication>
#include <QDir>

namespace lv{

std::unique_ptr<ApplicationContext> ApplicationContext::m_instance;

void ApplicationContext::initialize(){
    m_instance.reset(new ApplicationContext);
}

ApplicationContext::ApplicationContext(){
    initializePaths();
}

void ApplicationContext::initializePaths(){

    m_applicationFilePath = applicationFilePathImpl();
    QString applicationFilePath = QString::fromStdString(m_applicationFilePath);

#ifdef Q_OS_DARWIN
    m_applicationPath = QDir(QFileInfo(applicationFilePath).path() + "/..").absolutePath().toStdString();
    m_executablePath = QDir(QFileInfo(applicationFilePath).path()).absolutePath().toStdString();
    m_releasePath = QDir(QFileInfo(applicationFilePath).path() + "/../../..").absolutePath().toStdString();
    m_linkPath = applicationPath() + "/Link";
    m_pluginPath = applicationPath() + "/PlugIns";
    m_externalPath = applicationPath() + "/External";
    m_librariesPath =  applicationPath() + "/Libraries";
    m_developmentPath = applicationPath() + "/Dev";
    m_configPath = applicationPath() + "/config";

#else
    m_applicationPath = QFileInfo(applicationFilePath).path().toStdString();
    m_executablePath = QFileInfo(applicationFilePath).path().toStdString();
    m_releasePath = QFileInfo(applicationFilePath).path().toStdString();
    m_linkPath = applicationPath() + "/link";
    m_pluginPath = applicationPath() + "/plugins";
    m_externalPath = applicationPath() + "/external";
    m_librariesPath = applicationPath() + "/libraries";
    m_developmentPath = applicationPath() + "/dev";
    m_configPath = applicationPath() + "/config";
#endif
}


}// namespace

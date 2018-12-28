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

#include <QCoreApplication>
#include <QDir>

namespace lv{

/**
  \class lv::ApplicationContext
  \ingroup lvbase

  \brief Contains the application running context data.

  */

/// \private
class ApplicationContextPrivate{

public:
    static std::unique_ptr<ApplicationContext> instance;

    std::string applicationPath;
    std::string executablePath;
    std::string releasePath;
    std::string applicationFilePath;
    std::string linkPath;
    std::string pluginPath;
    std::string externalPath;
    std::string librariesPath;
    std::string developmentPath;
    std::string configPath;
};

std::unique_ptr<ApplicationContext> ApplicationContextPrivate::ApplicationContextPrivate::instance;

ApplicationContext::~ApplicationContext(){
    delete m_d;
}

void ApplicationContext::initialize(){
    ApplicationContextPrivate::instance.reset(new ApplicationContext);
}

ApplicationContext::ApplicationContext()
    : m_d(new ApplicationContextPrivate)
{
    initializePaths();
}

void ApplicationContext::initializePaths(){

    m_d->applicationFilePath = applicationFilePathImpl();
    QString applicationFilePath = QString::fromStdString(m_d->applicationFilePath);

#ifdef Q_OS_DARWIN
    m_d->applicationPath = QDir(QFileInfo(applicationFilePath).path() + "/..").absolutePath().toStdString();
    m_d->executablePath = QDir(QFileInfo(applicationFilePath).path()).absolutePath().toStdString();
    m_d->releasePath = QDir(QFileInfo(applicationFilePath).path() + "/../../..").absolutePath().toStdString();
    m_d->linkPath = applicationPath() + "/Link";
    m_d->pluginPath = applicationPath() + "/PlugIns";
    m_d->externalPath = applicationPath() + "/External";
    m_d->librariesPath =  applicationPath() + "/Libraries";
    m_d->developmentPath = applicationPath() + "/Dev";
    m_d->configPath = applicationPath() + "/config";

#else
    m_d->applicationPath = QFileInfo(applicationFilePath).path().toStdString();
    m_d->executablePath = QFileInfo(applicationFilePath).path().toStdString();
    m_d->releasePath = QFileInfo(applicationFilePath).path().toStdString();
    m_d->linkPath = applicationPath() + "/link";
    m_d->pluginPath = applicationPath() + "/plugins";
    m_d->externalPath = applicationPath() + "/external";
    m_d->librariesPath = applicationPath() + "/libraries";
    m_d->developmentPath = applicationPath() + "/dev";
    m_d->configPath = applicationPath() + "/config";
#endif
}

ApplicationContext &ApplicationContext::instance(){
    if ( !ApplicationContextPrivate::instance )
        THROW_EXCEPTION(lv::Exception, "Application context has not been initialized", 1);
    return *ApplicationContextPrivate::instance;
}

const std::string &ApplicationContext::applicationPath(){
    return m_d->applicationPath;
}

const std::string &ApplicationContext::releasePath(){
    return m_d->releasePath;
}

const std::string &ApplicationContext::applicationFilePath(){
    return m_d->applicationFilePath;
}

const std::string &ApplicationContext::linkPath(){
    return m_d->linkPath;
}

const std::string &ApplicationContext::pluginPath(){
    return m_d->pluginPath;
}

const std::string &ApplicationContext::externalPath(){
    return m_d->externalPath;
}

const std::string &ApplicationContext::librariesPath(){
    return m_d->librariesPath;
}

const std::string &ApplicationContext::developmentPath(){
    return m_d->developmentPath;
}

const std::string &ApplicationContext::configPath(){
    return m_d->configPath;
}

const std::string &ApplicationContext::executablePath(){
    return m_d->executablePath;
}

}// namespace

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

#ifndef LVAPPLICATIONCONTEXT_H
#define LVAPPLICATIONCONTEXT_H

#include <string>
#include "live/lvbaseglobal.h"
#include "live/exception.h"

class QString;

namespace lv{

class ViewEngine;
class Settings;

class LV_BASE_EXPORT ApplicationContext{

public:
    static void initialize();
    static ApplicationContext& instance();

    const std::string& applicationPath();
    const std::string& executablePath();
    const std::string& releasePath();
    const std::string& applicationFilePath();
    const std::string& linkPath();
    const std::string& pluginPath();
    const std::string& externalPath();
    const std::string& librariesPath();
    const std::string& developmentPath();
    const std::string& configPath();

private:
    ApplicationContext();
    void initializePaths();

    static std::string applicationFilePathImpl();

private:
    DISABLE_COPY(ApplicationContext);

    static std::unique_ptr<ApplicationContext> m_instance;

    std::string m_applicationPath;
    std::string m_executablePath;
    std::string m_releasePath;
    std::string m_applicationFilePath;
    std::string m_linkPath;
    std::string m_pluginPath;
    std::string m_externalPath;
    std::string m_librariesPath;
    std::string m_developmentPath;
    std::string m_configPath;
};

inline ApplicationContext &ApplicationContext::instance(){
    if ( !m_instance )
        THROW_EXCEPTION(lv::Exception, "Application context has not been initialized", 1);
    return *m_instance;
}

inline const std::string &ApplicationContext::applicationPath(){
    return m_applicationPath;
}

inline const std::string &ApplicationContext::releasePath(){
    return m_releasePath;
}

inline const std::string &ApplicationContext::applicationFilePath(){
    return m_applicationFilePath;
}

inline const std::string &ApplicationContext::linkPath(){
    return m_linkPath;
}

inline const std::string &ApplicationContext::pluginPath(){
    return m_pluginPath;
}

inline const std::string &ApplicationContext::externalPath(){
    return m_externalPath;
}

inline const std::string &ApplicationContext::librariesPath(){
    return m_librariesPath;
}

inline const std::string &ApplicationContext::developmentPath(){
    return m_developmentPath;
}

inline const std::string &ApplicationContext::configPath(){
    return m_configPath;
}

inline const std::string &ApplicationContext::executablePath(){
    return m_executablePath;
}

}// namespace

#endif // LVAPPLICATIONCONTEXT_H

/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVAPPLICATIONCONTEXT_H
#define LVAPPLICATIONCONTEXT_H

#include "live/lvbaseglobal.h"
#include <string>
#include <vector>

class QString;

namespace lv{

class MLNode;
class Settings;

class ApplicationContextPrivate;

class LV_BASE_EXPORT ApplicationContext{

public:
    ~ApplicationContext();

    static void initialize(const MLNode& defaults);
    static ApplicationContext& instance();

    const std::string& applicationPath();
    const std::string& executablePath();
    const std::string& releasePath();
    const std::string& applicationFilePath();
    const std::string& linkPath();
    const std::string& docsPath();
    const std::string& pluginPath();
    const std::string& externalPath();
    const std::string& librariesPath();
    const std::string& developmentPath();
    const std::string& configPath();

    const MLNode& startupConfiguration();

    void setScriptArguments(const std::vector<std::string>& args);
    const std::vector<std::string> scriptArguments() const;

private:
    ApplicationContext(const MLNode& defaults);
    void initializePaths();

    static std::string applicationFilePathImpl();

private:
    DISABLE_COPY(ApplicationContext);

    ApplicationContextPrivate* m_d;
};

}// namespace

#endif // LVAPPLICATIONCONTEXT_H

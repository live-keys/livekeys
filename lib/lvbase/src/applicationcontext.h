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

class QString;

namespace lv{

class ViewEngine;
class Settings;

class ApplicationContextPrivate;
class LV_BASE_EXPORT ApplicationContext{

public:
    ~ApplicationContext();

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

    ApplicationContextPrivate* m_d;
};

}// namespace

#endif // LVAPPLICATIONCONTEXT_H

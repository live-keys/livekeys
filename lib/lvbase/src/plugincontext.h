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

#ifndef LVPLUGINCONTEXT_H
#define LVPLUGINCONTEXT_H

#include "live/lvbaseglobal.h"

class QQmlEngine;

namespace lv{

class Engine;
class Settings;

class LV_BASE_EXPORT PluginContext{

public:
    static void initFromEngine(QQmlEngine* engine);

    static lv::Engine*   engine();
    static lv::Settings* settings();

    static QString executableDirPath();
    static QString applicationPath();
    static QString applicationFilePath();
    static QString linkPath();
    static QString pluginPath();
    static QString librariesPath();
    static QString developmentPath();
    static QString configPath();

private:
    static QString applicationFilePathImpl();

    PluginContext(){}

    static lv::Engine*   m_engine;
    static lv::Settings* m_settings;

    static QString m_applicationFilePath;
};

inline Engine *PluginContext::engine(){
    return m_engine;
}

inline Settings *PluginContext::settings(){
    return m_settings;
}

}// namespace

#endif // LVPLUGINCONTEXT_H

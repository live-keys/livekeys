#ifndef LVPLUGINCONTEXT_H
#define LVPLUGINCONTEXT_H

#include "live/lvbaseglobal.h"

class QQmlEngine;

namespace lcv{

class Engine;
class Settings;

class LVBASE_EXPORT PluginContext{

public:
    static void initFromEngine(QQmlEngine* engine);

    static lcv::Engine*   engine();
    static lcv::Settings* settings();

private:
    PluginContext(){}

    static lcv::Engine*   m_engine;
    static lcv::Settings* m_settings;
};

inline Engine *PluginContext::engine(){
    return m_engine;
}

inline Settings *PluginContext::settings(){
    return m_settings;
}

}// namespace

#endif // LVPLUGINCONTEXT_H

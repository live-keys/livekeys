#include "live/plugincontext.h"
#include "live/exception.h"
#include "live/engine.h"
#include "live/settings.h"
#include <QQmlEngine>
#include <QQmlContext>

namespace lcv{

lcv::Engine*   PluginContext::m_engine   = 0;
lcv::Settings* PluginContext::m_settings = 0;

void PluginContext::initFromEngine(QQmlEngine *engine){
    QObject* livecv = engine->rootContext()->contextProperty("livecv").value<QObject*>();
    if ( !livecv )
        throw lcv::Exception("Failed to load livecv context property.");

    m_engine   = qobject_cast<lcv::Engine*>(livecv->property("engine").value<QObject*>());
    m_settings = qobject_cast<lcv::Settings*>(livecv->property("settings").value<QObject*>());
    if ( !m_engine || !m_settings )
        throw lcv::Exception("Failed to load properties from context");
}

}// namespace

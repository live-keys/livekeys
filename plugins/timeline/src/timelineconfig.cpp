#include "timelineconfig.h"
#include "track.h"

#include "live/settings.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"

#include "live/visuallogqt.h"

#include <QQmlEngine>

namespace lv{

TimelineConfig::TimelineConfig(QObject *parent)
    : QObject(parent)
{
    Settings* settings = ViewContext::instance().settings();

    m_settings = qobject_cast<TimelineSettings*>(settings->file("timeline"));
    if (!m_settings ){
        m_settings = new TimelineSettings(settings->path());
        settings->addConfigFile("timeline", m_settings);
    }
}

TimelineConfig::~TimelineConfig(){
}

void TimelineConfig::addLoader(const QString &key, const QString &path){
    m_settings->addLoader(key, path);
}

QJSValue TimelineConfig::loaders() const{
    QQmlEngine* engine = qmlEngine(parent());
    if ( !engine )
        return QJSValue();


    QJSValue result = engine->newObject();
    for ( auto it = m_settings->loaders().begin(); it != m_settings->loaders().end(); ++it ){
        result.setProperty(it.key(), it.value());
    }
    return result;
}

QJSValue TimelineConfig::trackMenu(QObject *track){
    Track* t = qobject_cast<Track*>(track);
    if ( !t )
        return QJSValue();

    ViewEngine* ve = ViewEngine::grab(track);

    return m_settings->trackMenu(t->typeReference(), ve);
}

}// namespace

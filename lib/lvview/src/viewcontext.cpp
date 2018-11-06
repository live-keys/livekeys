#include "viewcontext.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/settings.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

QScopedPointer<ViewContext> ViewContext::m_instance;

void ViewContext::initFromEngine(QQmlEngine *engine){
    QObject* livecv = engine->rootContext()->contextProperty("livecv").value<QObject*>();
    if ( !livecv )
        THROW_EXCEPTION(lv::Exception, "Failed to load livecv context property.", 1);

    ViewEngine* e = qobject_cast<lv::ViewEngine*>(livecv->property("engine").value<QObject*>());
    Settings* s = qobject_cast<lv::Settings*>(livecv->property("settings").value<QObject*>());
    if ( !e || !s )
        THROW_EXCEPTION(lv::Exception, "Failed to load properties from context", 2);

    m_instance.reset(new ViewContext(e, s));
}

ViewContext::ViewContext(ViewEngine *engine, Settings *settings)
    : m_engine(engine)
    , m_settings(settings)
{
}

}// namespace

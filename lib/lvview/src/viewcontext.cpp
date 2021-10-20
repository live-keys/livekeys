#include "viewcontext.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/settings.h"
#include "live/memory.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

QScopedPointer<ViewContext> ViewContext::m_instance;

void ViewContext::initFromEngine(QQmlEngine *engine){
    if ( m_instance )
        return;

    QObject* livekeys = engine->rootContext()->contextProperty("lk").value<QObject*>();
    if ( !livekeys )
        THROW_EXCEPTION(lv::Exception, "Failed to load lk context property.", 1);

    ViewEngine* e = ViewEngine::grabFromQmlEngine(engine);
    Settings* s = qobject_cast<lv::Settings*>(livekeys->property("settings").value<QObject*>());
    Memory* m = e->memory();
    if ( !e || !s || !m )
        THROW_EXCEPTION(lv::Exception, "Failed to load properties from context", 2);

    m_instance.reset(new ViewContext(e, s, m));
}

ViewContext::ViewContext(ViewEngine *engine, Settings *settings, Memory* memory)
    : m_engine(engine)
    , m_settings(settings)
    , m_memory(memory)
{
}

}// namespace

#ifndef LVVIEWCONTEXT_H
#define LVVIEWCONTEXT_H

#include "live/lvviewglobal.h"
#include "live/exception.h"
#include <QScopedPointer>

class QQmlEngine;

namespace lv{

class Memory;
class Settings;
class ViewEngine;

/// \private
class LV_VIEW_EXPORT ViewContext{

public:
    ~ViewContext(){}

    static void initFromEngine(QQmlEngine* engine);
    static ViewContext& instance();

    ViewEngine* engine();
    Settings* settings();
    Memory* memory();

private:
    ViewContext(ViewEngine* engine, Settings* settings, lv::Memory *memory);

    static QScopedPointer<ViewContext> m_instance;

    ViewEngine* m_engine;
    Settings*   m_settings;
    Memory*     m_memory;

};

inline ViewContext &ViewContext::instance(){
    if ( m_instance.isNull() )
        THROW_EXCEPTION(lv::Exception, "View context is not initialize. Initialize from engine first.", 1);
    return *m_instance;
}

inline ViewEngine *ViewContext::engine(){
    return m_engine;
}

inline Settings *ViewContext::settings(){
    return m_settings;
}

inline Memory *ViewContext::memory(){
    return m_memory;
}

}// namespace

#endif // LVVIEWCONTEXT_H

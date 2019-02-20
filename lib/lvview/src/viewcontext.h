#ifndef LVVIEWCONTEXT_H
#define LVVIEWCONTEXT_H

#include "live/lvviewglobal.h"
#include "live/exception.h"
#include <QScopedPointer>

class QQmlEngine;

namespace lv{

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

private:
    ViewContext(ViewEngine* engine, Settings* settings);

    static QScopedPointer<ViewContext> m_instance;

    ViewEngine* m_engine;
    Settings*   m_settings;

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

}// namespace

#endif // LVVIEWCONTEXT_H

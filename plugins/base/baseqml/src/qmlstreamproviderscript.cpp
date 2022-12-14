#include "qmlstreamproviderscript.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/qmlerror.h"

namespace lv{

QmlStreamProviderScript::QmlStreamProviderScript(ViewEngine *ve, const QJSValue &options, const QJSValue &step, QObject *parent)
    : QObject(parent)
    , m_engine(ve)
    , m_stream(new QmlStream(this, nullptr))
    , m_step(step)
    , m_wait(0)
{
    Shared::ref(m_stream);
    m_args = QJSValueList() << ve->engine()->newQObject(this) << options;
}


QmlStreamProviderScript::~QmlStreamProviderScript(){
    Shared::unref(m_stream);
}

void QmlStreamProviderScript::wait(){
    ++m_wait;
}

void QmlStreamProviderScript::resume(){
    if ( m_wait > 0 )
        --m_wait;
    if ( m_wait == 0 ){
        next();
    }
}

void QmlStreamProviderScript::next(){
    while ( !m_wait ){
        QJSValue result = m_step.call(m_args);
        if ( result.isError() ){
            m_engine->throwError(result, this);
            m_stream->close();
            return;
        }

        if ( result.isBool() && result.toBool() == false ){
            return;
        }
    }
}

void QmlStreamProviderScript::push(const QJSValue &val){
    m_stream->push(val);
}

void QmlStreamProviderScript::close(){
    m_stream->close();
}

}// namespace

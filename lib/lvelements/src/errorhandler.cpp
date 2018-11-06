#include "errorhandler.h"
#include "element_p.h"

namespace lv{ namespace el{

ErrorHandler::ErrorHandler(Engine *engine)
    : Element(engine)
    , m_target(nullptr)
{
}

ErrorHandler::~ErrorHandler(){
}

void ErrorHandler::setTarget(Element *target){
    if ( target == m_target )
        return;

    if ( m_target != nullptr ){
        v8::Local<v8::Object> lo = ElementPrivate::localObject(m_target);
        v8::Local<v8::String> ehKey = v8::String::NewFromUtf8(
            engine()->isolate(), "__errorhandler__", v8::String::kInternalizedString
        );

        if ( lo->Has(ehKey) )
            lo->Delete(ehKey);
    }
    if ( target != nullptr ){
        v8::Local<v8::Object> lo = ElementPrivate::localObject(target);
        v8::Local<v8::String> ehKey = v8::String::NewFromUtf8(
            engine()->isolate(), "__errorhandler__", v8::String::kInternalizedString
        );

        lo->Set(ehKey, ElementPrivate::localObject(this));
    }

    m_target = target;
    targetChanged();
}

void ErrorHandler::rethrow(LocalValue e){
    Element* current = m_target;
    if ( current )
        current = m_target->parent();

    v8::Local<v8::String> ehKey = v8::String::NewFromUtf8(
        engine()->isolate(), "__errorhandler__", v8::String::kInternalizedString
    );

    while ( current ){
        v8::Local<v8::Object> lo = ElementPrivate::localObject(current);
        if ( lo->Has(ehKey) ){
            Element* elem = ElementPrivate::elementFromObject(lo->Get(ehKey)->ToObject());
            ErrorHandler* ehandler = elem->cast<ErrorHandler>();
            ehandler->handlerError(e.data(), m_lastError);
            break;
        }

        current = current->parent();
    }

    if ( !current ){
        engine()->handleError(m_lastError.message, m_lastError.stack, m_lastError.fileName, m_lastError.line);
    }

}

void ErrorHandler::handlerError(const v8::Local<v8::Value> &exception, const ErrorData &ed){
    m_lastError = ed;
    error(LocalValue(exception));
}

}} // namespace lv, el

/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

    auto isolate = engine()->isolate();
    auto context = isolate->GetCurrentContext();

    if ( m_target != nullptr ){
        v8::Local<v8::Object> lo = ElementPrivate::localObject(m_target);
        v8::Local<v8::String> ehKey = v8::String::NewFromUtf8(
            engine()->isolate(), "__errorhandler__", v8::NewStringType::kInternalized
        ).ToLocalChecked();

        if ( lo->Has(context, ehKey).ToChecked() )
            lo->Delete(context, ehKey).IsNothing();
    }
    if ( target != nullptr ){
        v8::Local<v8::Object> lo = ElementPrivate::localObject(target);
        v8::Local<v8::String> ehKey = v8::String::NewFromUtf8(
            engine()->isolate(), "__errorhandler__", v8::NewStringType::kInternalized
        ).ToLocalChecked();

        lo->Set(context, ehKey, ElementPrivate::localObject(this)).IsNothing();
    }

    m_target = target;
    targetChanged();
}

void ErrorHandler::rethrow(ScopedValue e){
    Element* current = m_target;
    if ( current )
        current = m_target->parent();

    auto isolate = engine()->isolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::String> ehKey = v8::String::NewFromUtf8(
        engine()->isolate(), "__errorhandler__", v8::NewStringType::kInternalized
    ).ToLocalChecked();

    while ( current ){
        v8::Local<v8::Object> lo = ElementPrivate::localObject(current);
        if ( lo->Has(context, ehKey).ToChecked() ){
            Element* elem = ElementPrivate::elementFromObject(lo->Get(context, ehKey).ToLocalChecked()->ToObject(context).ToLocalChecked());
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
    error(ScopedValue(exception));
}

}} // namespace lv, el

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

#include "script.h"
#include "context_p.h"
#include "element_p.h"
#include "errorhandler.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "modulefile.h"
#include "v8nowarnings.h"

#include <QFileInfo>

namespace lv{ namespace el{

class ScriptPrivate{
public:
    ScriptPrivate(Engine* e, const v8::Local<v8::Script>& d)
        : engine(e)
    {
        data.Reset(engine->isolate(), d);
    }

    Engine*                    engine;
    std::string                path;
    v8::Persistent<v8::Script> data;
};


const char* Script::encloseStart = "(function(){";
const char* Script::encloseEnd   = "})()";

const char* Script::moduleEncloseStart = "(function(module, imports, __NAME__, __FILE__){\n";
const char* Script::moduleEncloseEnd   = "})";


Value Script::run(){
    v8::HandleScope handle(m_d->engine->isolate());
    v8::Local<v8::Context> context = m_d->engine->currentContext()->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Script> ld = m_d->data.Get(m_d->engine->isolate());
    v8::MaybeLocal<v8::Value> result = ld->Run(context);
    if ( result.IsEmpty() )
        return ScopedValue(m_d->engine).toValue(m_d->engine);
    else
        return ScopedValue(result.ToLocalChecked()).toValue(m_d->engine);

    return Value();
}

Object Script::loadAsModule(ModuleFile* file){
    auto isolate = m_d->engine->isolate();
    auto context = isolate->GetCurrentContext();
    v8::HandleScope handle(isolate);
    v8::Context::Scope context_scope(context);

    if ( m_d->engine->tryCatchNesting() == 0){
        v8::TryCatch tc(m_d->engine->isolate());
        m_d->engine->incrementTryCatchNesting();

        Object m = loadAsModuleImpl(file, context);

        if ( tc.HasCaught() ){
            Engine::CatchData cd(m_d->engine, &tc);

            v8::Local<v8::String> ehKey = v8::String::NewFromUtf8(
                m_d->engine->isolate(), "__errorhandler__", v8::NewStringType::kInternalized
            ).ToLocalChecked();

            Element* current = cd.object();
            while ( current ){
                v8::Local<v8::Object> lo = ElementPrivate::localObject(current);
                if ( lo->Has(context, ehKey).ToChecked() ){
                    Element* elem = ElementPrivate::elementFromObject(lo->Get(context, ehKey).ToLocalChecked()->ToObject(context).ToLocalChecked());
                    ErrorHandler* ehandler = elem->cast<ErrorHandler>();
                    ErrorHandler::ErrorData ed;
                    ed.fileName = cd.fileName();
                    ed.stack    = cd.stack();
                    ed.message  = cd.message();
                    ed.line     = cd.lineNumber();

                    ehandler->handlerError(tc.Exception(), ed);
                    break;
                }

                current = current->parent();
            }

            if ( !current ){
                m_d->engine->handleError(cd.message(), cd.stack(), cd.fileName(), cd.lineNumber());
            }
        }

        m_d->engine->decrementTryCatchNesting();

        return m;

    } else {
        return loadAsModuleImpl(file, context);
    }
}

Script::~Script(){
    m_d->data.Reset();
    delete m_d;
}

Object Script::loadAsModuleImpl(ModuleFile *, const v8::Local<v8::Context> &){
    return Object(m_d->engine);
}

Script::Script(Engine *engine, const v8::Local<v8::Script> &value, const std::string &path)
    : m_d(new ScriptPrivate(engine, value))
{
    m_d->path = path;
}

}} // namespace lv, el

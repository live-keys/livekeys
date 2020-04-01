#include "script.h"
#include "context_p.h"
#include "element_p.h"
#include "errorhandler.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "modulefile.h"
#include "imports_p.h"
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
    v8::HandleScope handle(m_d->engine->isolate());
    v8::Local<v8::Context> context = m_d->engine->currentContext()->asLocal();
    v8::Context::Scope context_scope(context);

    if ( m_d->engine->tryCatchNesting() == 0){
        v8::TryCatch tc(m_d->engine->isolate());
        m_d->engine->incrementTryCatchNesting();

        Object m = loadAsModuleImpl(file, context);

        if ( tc.HasCaught() ){
            Engine::CatchData cd(m_d->engine, &tc);

            v8::Local<v8::String> ehKey = v8::String::NewFromUtf8(
                m_d->engine->isolate(), "__errorhandler__", v8::String::kInternalizedString
            );

            Element* current = cd.object();
            while ( current ){
                v8::Local<v8::Object> lo = ElementPrivate::localObject(current);
                if ( lo->Has(ehKey) ){
                    Element* elem = ElementPrivate::elementFromObject(lo->Get(ehKey)->ToObject());
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

Object Script::loadAsModuleImpl(ModuleFile *mf, const v8::Local<v8::Context> &context){
    v8::Local<v8::Script> ld = m_d->data.Get(m_d->engine->isolate());
    v8::MaybeLocal<v8::Value> result = ld->Run(context);
    if ( result.IsEmpty() ){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception,
            mf->filePath() + ": Failed to run. Result empty.",
            lv::Exception::toCode("~Run")
        );
        m_d->engine->throwError(&e, nullptr);
        return Object(m_d->engine);
    }

    v8::Local<v8::Function> loadFunction = v8::Local<v8::Function>::Cast(result.ToLocalChecked());

    ScopedValue module = mf->createObject(m_d->engine);

    v8::Local<v8::String> name = v8::String::NewFromUtf8(
        m_d->engine->isolate(), mf->name().c_str(), v8::String::kInternalizedString);
    v8::Local<v8::String> filePath = v8::String::NewFromUtf8(
        m_d->engine->isolate(), mf->filePath().c_str(), v8::String::kInternalizedString);

    v8::Local<v8::Value> args[4];
    args[0] = module.data();
    args[1] = mf->imports()->object();
    args[2] = name;
    args[3] = filePath;

    loadFunction->Call(context->Global(), 4, args);


    v8::Local<v8::Object> moduleValue = v8::Local<v8::Object>::Cast(module.data());
    v8::Local<v8::String> exportsKey = v8::String::NewFromUtf8(m_d->engine->isolate(), "exports");

    v8::Local<v8::Value> exportsValue = moduleValue->Get(exportsKey);
    if ( !exportsValue->IsObject() ){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception,
            mf->filePath() + ": module.exports is not an object type.",
            lv::Exception::toCode("~Key")
        );
        m_d->engine->throwError(&e, nullptr);
        return module.toObject(m_d->engine);
    }

    v8::Local<v8::Object> exportsObject = v8::Local<v8::Object>::Cast(exportsValue);

    v8::Local<v8::Array> propertyNames = exportsObject->GetOwnPropertyNames();
    for (uint32_t i = 0; i < propertyNames->Length(); ++i) {
        v8::Local<v8::Value> key = propertyNames->Get(i);

        if ( !key->IsString() ){
            lv::Exception e = CREATE_EXCEPTION(
                lv::Exception,
                mf->filePath() + ": Given an export key that is not of string type.",
                lv::Exception::toCode("~Key")
            );
            m_d->engine->throwError(&e, nullptr);
            return module.toObject(m_d->engine);
        }

        v8::String::Utf8Value utf8_key(key);

        if ( strcmp(*utf8_key, "__shared__") != 0 && strlen(*utf8_key) > 0 ){
            v8::Local<v8::Value> value = exportsObject->Get(key);

            if ( value->IsFunction() ){
                v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);

                Callable c(m_d->engine, func);
                if ( !c.isComponent() ){
                    lv::Exception e = CREATE_EXCEPTION(
                        lv::Exception,
                        mf->filePath() + ": Export at \'" + *utf8_key + "\' is not a component. Class must extend element.",
                        lv::Exception::toCode("~Component")
                    );
                    m_d->engine->throwError(&e, nullptr);

                    return module.toObject(m_d->engine);
                }

            } else if ( value->IsObject() ){
                if ( !ScopedValue(value).isElement() ){
                    lv::Exception e = CREATE_EXCEPTION(
                        lv::Exception,
                        mf->filePath() + ": Export at \'" + *utf8_key + "\' is an object that's not an element type.",
                        lv::Exception::toCode("~Element")
                    );
                    m_d->engine->throwError(&e, nullptr);
                    return module.toObject(m_d->engine);
                }
            } else {
                lv::Exception e = CREATE_EXCEPTION(
                    lv::Exception,
                    mf->filePath() + ": Export at \'" + *utf8_key + "\' is not a component nor a singleton.",
                    lv::Exception::toCode("~Component")
                );
                m_d->engine->throwError(&e, nullptr);
                return module.toObject(m_d->engine);
            }
        }
    }

    return module.toObject(m_d->engine);
}

Script::Script(Engine *engine, const v8::Local<v8::Script> &value, const std::string &path)
    : m_d(new ScriptPrivate(engine, value))
{
    m_d->path = path;
}

}} // namespace lv, el

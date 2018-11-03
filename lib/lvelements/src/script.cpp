#include "script.h"
#include "context_p.h"
#include "element_p.h"
#include "errorhandler.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "v8.h"
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
        return LocalValue(m_d->engine).toValue(m_d->engine);
    else
        return LocalValue(result.ToLocalChecked()).toValue(m_d->engine);

    return Value();
}

Object Script::loadAsModule(){
    v8::HandleScope handle(m_d->engine->isolate());
    v8::Local<v8::Context> context = m_d->engine->currentContext()->asLocal();
    v8::Context::Scope context_scope(context);

    if ( !m_d->engine->hasGlobalErrorHandler() ){
        v8::TryCatch tc(m_d->engine->isolate());
        m_d->engine->setGlobalErrorHandler(true);

        Object m = loadAsModuleImpl(context);

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

        m_d->engine->setGlobalErrorHandler(false);

        return m;

    } else {
        return loadAsModuleImpl(context);
    }
}

Script::~Script(){
    m_d->data.Reset();
    delete m_d;
}

Object Script::loadAsModuleImpl(const v8::Local<v8::Context> &context){
    v8::Local<v8::Script> ld = m_d->data.Get(m_d->engine->isolate());
    v8::MaybeLocal<v8::Value> result = ld->Run(context);
    if ( result.IsEmpty() )
        throw std::exception();

    v8::Local<v8::Value> localResult = result.ToLocalChecked();
    v8::Local<v8::Function> loadFunction = v8::Local<v8::Function>::Cast(result.ToLocalChecked());

    LocalValue module = Module::createObject(m_d->engine, m_d->path);

    //TODO: Receive from package
    v8::Local<v8::Object> importsObject = v8::Object::New(m_d->engine->isolate());

    QFileInfo finfo(QString::fromStdString(m_d->path));

    v8::Local<v8::String> name = v8::String::NewFromUtf8(
        m_d->engine->isolate(), finfo.baseName().toStdString().c_str(), v8::String::kInternalizedString);
    v8::Local<v8::String> file = v8::String::NewFromUtf8(
        m_d->engine->isolate(), finfo.filePath().toStdString().c_str(), v8::String::kInternalizedString);

    v8::Local<v8::Value> args[4];
    args[0] = module.data();
    args[1] = importsObject;
    args[2] = name;
    args[3] = file;

    loadFunction->Call(context->Global(), 4, args);

    return module.toObject(m_d->engine);
}

Script::Script(Engine *engine, const v8::Local<v8::Script> &value, const std::string &path)
    : m_d(new ScriptPrivate(engine, value))
{
    m_d->path = path;
}

}} // namespace lv, script

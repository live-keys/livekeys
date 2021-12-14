#include "jsmodule.h"
#include "v8nowarnings.h"
#include "live/elements/engine.h"

#include <QFileInfo>

namespace lv{ namespace el{

class JsModulePrivate{
public:
    v8::Persistent<v8::Module> module;
};

JsModule::Ptr JsModule::create(Engine* engine, const std::string &file, const v8::Local<v8::Module> &mod){
    JsModule* jm = new JsModule(engine, file, mod);

    JsModule::Ptr jmptr = JsModule::Ptr(jm);
    return jmptr;
}

v8::Local<v8::Module> JsModule::localModule() const{
    return d_ptr->module.Get(m_engine->isolate());
}

v8::Local<v8::Value> JsModule::localModuleNamespace(){
    return localModule()->GetModuleNamespace();
}

v8::Local<v8::Value> JsModule::evaluate(v8::Local<v8::Context> ctx){
    v8::Local<v8::Value> retValue;

    v8::Local<v8::Module> mod = localModule();
    if (!mod->Evaluate(ctx).ToLocal(&retValue)) {
        THROW_EXCEPTION(lv::Exception, "Failed to evaluate module: " + m_file, lv::Exception::toCode("~Module"));
    }

    return retValue;
}

ScopedValue JsModule::moduleNamespace(){
    return ScopedValue(localModule()->GetModuleNamespace());
}

ScopedValue JsModule::evaluate(){
    auto isolate = m_engine->isolate();
    auto context = isolate->GetCurrentContext();

    return ScopedValue(evaluate(context));
}

const std::string &JsModule::name() const{
    return m_name;
}

JsModule::Status JsModule::status() const{
    v8::Local<v8::Module> mod = localModule();
    if ( mod->GetStatus() == v8::Module::kEvaluated )
        return JsModule::Ready;
    else if ( mod->GetStatus() == v8::Module::kEvaluating )
        return JsModule::Evaluating;
    return JsModule::Initialized;
}

JsModule::JsModule(Engine* engine, const std::string& file, const v8::Local<v8::Module>& mod)
    : d_ptr(new JsModulePrivate)
    , m_file(file)
    , m_moduleFile(nullptr)
    , m_engine(engine)
{
    m_name = QFileInfo(QString::fromStdString(file)).baseName().toStdString();
    d_ptr->module.Reset(engine->isolate(), mod);
}

JsModule::~JsModule(){
    d_ptr->module.Reset();
    delete d_ptr;
}

}}// namespace lv, el

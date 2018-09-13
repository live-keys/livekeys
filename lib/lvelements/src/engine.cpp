#include "engine.h"
#include "element.h"
#include "element_p.h"
#include "value_p.h"
#include "context_p.h"

#include "libplatform/libplatform.h"
#include "v8.h"

namespace lv{ namespace el{

class ComponentTemplate{

public:
    ComponentTemplate(Engine* e, const v8::Local<v8::FunctionTemplate>& v){
        data.Reset(e->isolate(), v);
    }

    ~ComponentTemplate(){
        data.Reset();
    }

public:
    v8::Persistent<v8::FunctionTemplate> data;
};


class ExecutionContext{

public:
    ExecutionContext(Engine* engine)
        : handle_scope(engine->isolate())
        , context(v8::Context::New(engine->isolate()))
        , context_scope(context)
    {
    }

    v8::HandleScope handle_scope;
    v8::Local<v8::Context> context;
    v8::Context::Scope context_scope;
};


class EnginePrivate{

public:
    EnginePrivate() : context(nullptr), platform(nullptr), isolate(nullptr), elementTemplate(nullptr){}

    Context*                   context;
    v8::Platform*              platform;
    v8::Isolate::CreateParams* createParams;
    v8::Isolate*               isolate;
    v8::Isolate::Scope*        mainIsolateScope;

    ComponentTemplate*         elementTemplate;
    std::map<const MetaObject*, ComponentTemplate*> registeredTemplates;

    v8::Persistent<v8::FunctionTemplate> pointTemplate;
    v8::Persistent<v8::FunctionTemplate> sizeTemplate;
    v8::Persistent<v8::FunctionTemplate> rectangleTemplate;

public: // helpers
    bool isElementConstructor(
        Engine *engine,
        const v8::Local<v8::Function>& fnc,
        const v8::Local<v8::Function> &elementFnc
    );

};

bool EnginePrivate::isElementConstructor(
        Engine* engine,
        const v8::Local<v8::Function> &fnc,
        const v8::Local<v8::Function> &elementFnc)
{
    v8::Local<v8::Value> proto = fnc->Get(v8::String::NewFromUtf8(engine->isolate(), "__proto__"));
    if ( proto->Equals(elementFnc) )
        return true;

    v8::Local<v8::Value> v = fnc->GetPrototype();
    if ( !v->IsFunction() )
        return false;

    return isElementConstructor(engine, v8::Local<v8::Function>::Cast(v), elementFnc);
}

// Engine Initialization
// --------------------------------------------------------------------------------------------

class Engine::InitializeData{
public:
    v8::Platform* platform;
    bool disposeAtExit;
};

Engine::InitializeData* Engine::m_initializeData = nullptr;

void Engine::initialize(const std::string &defaultLocation){
    if ( !isInitialized() ){
        v8::V8::InitializeICUDefaultLocation(defaultLocation.c_str());
        v8::V8::InitializeExternalStartupData(defaultLocation.c_str());

        m_initializeData = new Engine::InitializeData;
        m_initializeData->platform = v8::platform::CreateDefaultPlatform();
        m_initializeData->disposeAtExit = false;

        v8::V8::InitializePlatform(m_initializeData->platform);
        v8::V8::Initialize();
    }
}

void Engine::dispose(){
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete m_initializeData;
    m_initializeData = nullptr;
}

bool Engine::isInitialized(){
    return m_initializeData != nullptr;
}

void Engine::disposeAtExit(){
    if ( !m_initializeData->disposeAtExit ){
        m_initializeData->disposeAtExit = true;
        atexit(&Engine::dispose);
    }
}

// Engine Implementation
// --------------------------------------------------------------------------------------------

Engine::Engine()
    : m_d(new EnginePrivate)
{
    if ( !isInitialized() )
        throw std::exception(); //TODO

    // Create a new Isolate and make it the current one.

    m_d->createParams = new v8::Isolate::CreateParams;
    m_d->createParams->array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    m_d->isolate = v8::Isolate::New(*m_d->createParams);
    if ( m_d->isolate->GetNumberOfDataSlots() == 1 ){
        throw std::exception(); //TODO: Replace with lvbase one, and collect garbage
    }
    m_d->isolate->SetData(0, this);
    m_d->mainIsolateScope = new v8::Isolate::Scope(m_d->isolate);

    v8::HandleScope handle_scope(isolate());
    v8::Local<v8::Context> context = v8::Context::New(isolate());
    m_d->context = new Context(this, context);

    importInternals();
}

Engine::~Engine(){
    delete m_d->context;
    delete m_d->mainIsolateScope;
    m_d->isolate->Dispose();
}

v8::Isolate *Engine::isolate(){
    return m_d->isolate;
}

Context *Engine::currentContext() const{
    return m_d->context;
}

Script::Ptr Engine::compile(const std::string &str){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate(), str.c_str());

    v8::MaybeLocal<v8::Script> r = v8::Script::Compile(context, source);

    Script::Ptr sc(new Script(this, r.ToLocalChecked()));
    return sc;
}

/**
 * @brief Compiles the script into an enclosed function.
 *
 * Wraps the given string into a function and compiles it. You can use the return value to
 * capture values from the function:
 *
 * @code
 * compileEnclosed("return 20")
 * @endcode
 *
 * Will basically compile the code into:
 *
 * @code
 * compile("(function(){ return 20; )()");
 * @endcode
 *
 * @param str
 * @return
 */
Script::Ptr Engine::compileEnclosed(const std::string &str){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate(), (Script::encloseStart + str + Script::encloseEnd).c_str());

    Script::Ptr sc(new Script(this, v8::Script::Compile(context, source).ToLocalChecked()));
    return sc;
}

ComponentTemplate *Engine::registerTemplate(const MetaObject *t){
    auto it = m_d->registeredTemplates.find(t);
    if ( it != m_d->registeredTemplates.end() )
        return it->second;

    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate());
    tpl->SetCallHandler(t->constructor()->ptr());
    tpl->SetClassName(v8::String::NewFromUtf8(isolate(), t->name().c_str()));

    v8::Local<v8::ObjectTemplate> tplInstance = tpl->InstanceTemplate();
    tplInstance->SetInternalFieldCount(1);

    // Index Access
    if ( t->hasIndexAccess() ){
        tplInstance->SetIndexedPropertyHandler(
            &Property::ptrIndexGetImplementation,
            &Property::ptrIndexSetterImplementation
        );
    }

    // Inheritance
    if ( t->base() ){
        ComponentTemplate* base = registerTemplate(t->base());
        v8::Local<v8::FunctionTemplate> baseTpl = base->data.Get(isolate());
        tpl->Inherit(baseTpl);
    }

    // Properties
    for ( auto propIt = t->ownPropertiesBegin(); propIt != t->ownPropertiesEnd(); ++propIt ){
//        qDebug() << "Adding property:" << propIt->first.c_str();
        Property* p = propIt->second;
        v8::Local<v8::External> pdata = v8::External::New(isolate(), p);

        if ( p->isWritable() ){
            tplInstance->SetAccessor(
                v8::String::NewFromUtf8(isolate(), propIt->first.c_str()),
                &Property::ptrGetImplementation,
                &Property::ptrSetImplementation,
                pdata
            );
        } else {
            tplInstance->SetAccessor(
                v8::String::NewFromUtf8(isolate(), propIt->first.c_str()),
                &Property::ptrGetImplementation,
                0,
                pdata
            );
        }
    }

    // Methods
    v8::Local<v8::Template> tplPrototype = tpl->PrototypeTemplate(); // for methods and events
    for ( auto funcIt = t->ownMethodsBegin(); funcIt != t->ownMethodsEnd(); ++funcIt ){
//        qDebug() << "Adding method:" << funcIt->first.c_str();
        Function* f = funcIt->second;
        v8::Local<v8::External> fdata = v8::External::New(isolate(), f);
        tplPrototype->Set(isolate(), funcIt->first.c_str(), v8::FunctionTemplate::New(isolate(), f->ptr(), fdata));
    }

    // Events
    for ( auto eventIt = t->ownEventsBegin(); eventIt != t->ownEventsEnd(); ++eventIt ){
//        qDebug() << "Adding event:" << eventIt->first.c_str();
        Function* f = eventIt->second;
        v8::Local<v8::External> fdata = v8::External::New(isolate(), f);
        tplPrototype->Set(isolate(), eventIt->first.c_str(), v8::FunctionTemplate::New(isolate(), f->ptr(), fdata));
    }

    // Functions
    v8::Local<v8::Function> ftpl = tpl->GetFunction();
    for ( auto funcIt = t->functionsBegin(); funcIt != t->functionsEnd(); ++funcIt ){
//        qDebug() << "Adding function:" << funcIt->first.c_str();
        Function* f = funcIt->second;
        v8::Local<v8::External> fdata = v8::External::New(isolate(), f);
        ftpl->Set(
            v8::String::NewFromUtf8(isolate(), funcIt->first.c_str()),
            v8::FunctionTemplate::New(isolate(), f->ptr(), fdata)->GetFunction()
        );
    }

    ComponentTemplate* compTpl = new ComponentTemplate(this, tpl);
    m_d->registeredTemplates[t] = compTpl;
    return compTpl;
}

v8::Local<v8::FunctionTemplate> Engine::pointTemplate(){
    return m_d->pointTemplate.Get(isolate());
}

v8::Local<v8::FunctionTemplate> Engine::sizeTemplate(){
    return m_d->sizeTemplate.Get(isolate());
}

v8::Local<v8::FunctionTemplate> Engine::rectangleTemplate(){
    return m_d->rectangleTemplate.Get(isolate());
}

bool Engine::isElementConstructor(const Callable &c){
    v8::Local<v8::Function> f = c.data();
    v8::Local<v8::Function> elemf = m_d->elementTemplate->data.Get(isolate())->GetFunction();
    v8::Local<v8::Function> elemfproto = v8::Local<v8::Function>::Cast(elemf->GetPrototype());
    return m_d->isElementConstructor(this, f, elemfproto);
}

void Engine::importInternals(){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    m_d->elementTemplate = registerTemplate(&Element::metaObject());

    v8::Local<v8::FunctionTemplate> tpl = m_d->elementTemplate->data.Get(isolate());
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Element"),   tpl->GetFunction());

    m_d->pointTemplate.Reset(isolate(), Point::functionTemplate(isolate()));
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Point"), pointTemplate()->GetFunction());

    m_d->sizeTemplate.Reset(isolate(), Size::functionTemplate(isolate()));
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Size"), sizeTemplate()->GetFunction());

    m_d->rectangleTemplate.Reset(isolate(), Rectangle::functionTemplate(isolate()));
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Rectangle"), rectangleTemplate()->GetFunction());
}

void Engine::wrapScriptObject(Element *element){
    ComponentTemplate* ctpl = registerTemplate(&element->typeMetaObject());
    v8::Local<v8::FunctionTemplate> tpl = ctpl->data.Get(isolate());
    v8::Local<v8::Object> instance = tpl->InstanceTemplate()->NewInstance();
    element->setLifeTimeWithObject(instance);
}


// Module caching

Object Engine::require(Module::Ptr module){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> base = v8::Object::New(m_d->isolate);
    Object exportsObject(this, base);

    for ( auto it = module->begin(); it != module->end(); ++it ){
        const MetaObject* t = it->second;

        ComponentTemplate* ctpl = registerTemplate(t);
        v8::Local<v8::FunctionTemplate> tpl = ctpl->data.Get(isolate());

        base->Set(v8::String::NewFromUtf8(isolate(), t->name().c_str()), tpl->GetFunction());
    }

    return exportsObject;
}

void Engine::scope(const std::function<void()> &f){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    f();
}

Engine::InitializeScope::InitializeScope(const std::string &defaultLocation){
    Engine::initialize(defaultLocation);
}

Engine::InitializeScope::~InitializeScope(){
    Engine::dispose();
}

}} // namespace lv, script

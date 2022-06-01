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

#include "engine.h"
#include "element.h"
#include "element_p.h"
#include "value_p.h"
#include "context_p.h"
#include "container.h"
#include "live/exception.h"
#include "live/visuallog.h"
#include "live/modulecontext.h"
#include "visuallogjsobject.h"
#include "errorhandler.h"
#include "tuple.h"
#include "component.h"
#include "modulefile.h"
#include "modulefilejsdata.h"

#include "v8nowarnings.h"

#include <sstream>
#include <iomanip>
#include <unordered_map>

#include <QFileInfo>
#include <QDateTime>

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
    EnginePrivate()
        : context(nullptr)
        , platform(nullptr)
        , isolate(nullptr)
        , elementTemplate(nullptr)
        , tryCatchNesting(0)
        , pendingExceptionNesting(-1)
        , hasGlobalErrorHandler(false)
        , moduleFileType(Engine::Lv)
    {}

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

    int  tryCatchNesting;
    int  pendingExceptionNesting;
    bool hasGlobalErrorHandler;

    Engine::ModuleFileType moduleFileType;

    Compiler::Ptr compiler;

    Engine::FileInterceptor* fileInterceptor;

    std::map<std::string, JsModule::Ptr>             pathToModule;
    std::unordered_multimap<uint32_t, JsModule::Ptr> hashToModule;


public: // helpers
    bool isElementConstructor(
        Engine *engine,
        const v8::Local<v8::Function>& fnc,
        const v8::Local<v8::Function> &elementFnc
    );

    JsModule::Ptr getFromModule(const v8::Local<v8::Module>& mod);

    static v8::MaybeLocal<v8::Promise> resolveModuleDynamically(
        v8::Local<v8::Context> context,
        v8::Local<v8::ScriptOrModule> referrer,
        v8::Local<v8::String> specifier,
        v8::Local<v8::FixedArray>
    );
    static v8::MaybeLocal<v8::Module> resolveModule(
        v8::Local<v8::Context> context,
        v8::Local<v8::String> specifier,
        v8::Local<v8::Module> referrer
    );
    static void resolveModuleMeta(
        v8::Local<v8::Context> context,
        v8::Local<v8::Module> module,
        v8::Local<v8::Object> meta
    );

    static void messageListener(v8::Local<v8::Message> message, v8::Local<v8::Value> data);

};

JsModule::Ptr EnginePrivate::getFromModule(const v8::Local<v8::Module> &mod){
    auto range = hashToModule.equal_range(mod->GetIdentityHash());
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second->localModule() == mod)
            return it->second;
    }
    return nullptr;
}

v8::MaybeLocal<v8::Promise> EnginePrivate::resolveModuleDynamically(
        v8::Local<v8::Context> context,
        v8::Local<v8::ScriptOrModule> referrer,
        v8::Local<v8::String> specifier,
        v8::Local<v8::FixedArray>)
{
    v8::Local<v8::Promise::Resolver> resolver =
        v8::Promise::Resolver::New(context).ToLocalChecked();
    v8::MaybeLocal<v8::Promise> promise(resolver->GetPromise());

    auto engine = reinterpret_cast<Engine*>(context->GetIsolate()->GetData(0));

    v8::String::Utf8Value name(engine->isolate(), specifier);

    std::string path = *name;
    if ( path.length() >= 2 && path[0] == '.' && path[1] == '/' ){
        path = path.substr(2);

        auto val = referrer->GetResourceName();
        v8::String::Utf8Value name(engine->isolate(), val);
        std::string referrerPath = *name;

        for ( auto it = engine->m_d->hashToModule.begin(); it != engine->m_d->hashToModule.end(); ++it ){
            if ( it->second->filePath() == referrerPath ){
                QFileInfo finfo(QString::fromStdString(it->second->filePath()));
                std::string fileParent = finfo.path().toStdString();
                path = fileParent + "/" + path;
                break;
            }
        }
    } else {
        auto packagePaths = engine->compiler()->packageImportPaths();
        for ( auto it = packagePaths.begin(); it != packagePaths.end(); ++it ){
            QFileInfo finfo(QString::fromStdString(*it + "/" + path));
            if ( finfo.exists() ){
                path = *it + "/" + path;
            }
        }
    }

    try {
        JsModule::Ptr module = engine->loadJsModule(path);
        module->evaluate(context);
        auto localModule = module->localModule();
        engine->m_d->hashToModule.insert(std::make_pair(localModule->GetIdentityHash(), module));

        v8::Local<v8::Value> moduleNamespace = module->localModuleNamespace();
        auto result = resolver->Resolve(context, moduleNamespace);
        if ( result.IsNothing() )
            return v8::MaybeLocal<v8::Promise>();

        return promise;

    } catch (lv::Exception& e) {
        engine->throwError(&e, nullptr);
    }

    return v8::MaybeLocal<v8::Promise>();
}

v8::MaybeLocal<v8::Module> EnginePrivate::resolveModule(
        v8::Local<v8::Context> context,
        v8::Local<v8::String> specifier,
        v8::Local<v8::Module> referrer)
{
    auto engine = reinterpret_cast<Engine*>(context->GetIsolate()->GetData(0));
    v8::String::Utf8Value name(engine->isolate(), specifier);

    std::string path = *name;

    QFileInfo finfo(QString::fromStdString(path));
    if ( finfo.isRelative() ){
        if ( path.length() >= 2 && path[0] == '.' && path[1] == '/' ){
            path = path.substr(2);

            JsModule::Ptr module = engine->m_d->getFromModule(referrer);
            if ( module ){
                QFileInfo finfo(QString::fromStdString(module->filePath()));
                QString fullPath = finfo.path() + "/" + QString::fromStdString(path);
                path = QFileInfo(fullPath).absoluteFilePath().toStdString();
            }
        } else if ( path.length() >= 2 && path[0] == '.' && path[1] == '.' ){
            JsModule::Ptr module = engine->m_d->getFromModule(referrer);
            if ( module ){
                QFileInfo finfo(QString::fromStdString(module->filePath()));
                QString fullPath = finfo.path() + "/" + QString::fromStdString(path);
                path = QFileInfo(fullPath).absoluteFilePath().toStdString();
            }

        } else {
            auto packagePaths = engine->compiler()->packageImportPaths();
            for ( auto it = packagePaths.begin(); it != packagePaths.end(); ++it ){
                QFileInfo finfo(QString::fromStdString(*it + "/" + path));
                if ( finfo.exists() ){
                    path = *it + "/" + path;
                }
            }
        }
    }

    try {
        JsModule::Ptr module = engine->loadJsModule(path);
        auto localModule = module->localModule();

        engine->m_d->hashToModule.insert(std::make_pair(localModule->GetIdentityHash(), module));

        return localModule;
    } catch (lv::Exception& e) {
        engine->throwError(&e, nullptr);
    }
    return v8::MaybeLocal<v8::Module>();
}

void EnginePrivate::resolveModuleMeta(
        v8::Local<v8::Context> context,
        v8::Local<v8::Module> referrer,
        v8::Local<v8::Object> meta)
{
    auto engine = reinterpret_cast<Engine*>(context->GetIsolate()->GetData(0));
    JsModule::Ptr module = engine->m_d->getFromModule(referrer);

    if ( module->m_moduleFile ){

        v8::Maybe<bool> result = meta->Set(
            context,
            v8::String::NewFromUtf8(context->GetIsolate(), "url").ToLocalChecked(),
            v8::String::NewFromUtf8(context->GetIsolate(), module->m_moduleFile->filePath().c_str()).ToLocalChecked()
        );
        if ( result.IsNothing() ){
            //TODO: engine warning
        }

        std::string moduleUri = module->m_moduleFile->module()->module()->context()->importId.data();
        v8::Maybe<bool> moduleResult = meta->Set(
            context,
            v8::String::NewFromUtf8(context->GetIsolate(), "module").ToLocalChecked(),
            v8::String::NewFromUtf8(context->GetIsolate(), moduleUri.c_str()).ToLocalChecked()
        );
        if ( moduleResult.IsNothing() ){
            //TODO: engine warning
        }
    } else {
        v8::Maybe<bool> result = meta->Set(
            context,
            v8::String::NewFromUtf8(context->GetIsolate(), "url").ToLocalChecked(),
            v8::String::NewFromUtf8(context->GetIsolate(), module->filePath().c_str()).ToLocalChecked()
        );
        if ( result.IsNothing() ){
            //TODO: engine warning
        }
    }
}

void EnginePrivate::messageListener(v8::Local<v8::Message> message, v8::Local<v8::Value> data){
    v8::Local<v8::External> engineData = v8::Local<v8::External>::Cast(data);
    Engine* engine = reinterpret_cast<Engine*>(engineData->Value());

    v8::String::Utf8Value msg(engine->isolate(), message->Get());
    v8::String::Utf8Value file(engine->isolate(), v8::Local<v8::String>::Cast(message->GetScriptResourceName()));
    int line = message->GetScriptOrigin().LineOffset();

    engine->handleError(*msg, "", *file, line);
}

// Engine Initialization
// --------------------------------------------------------------------------------------------

class Engine::InitializeData{
public:
    std::unique_ptr<v8::Platform> platform;
    bool disposeAtExit;
};

Engine::InitializeData* Engine::m_initializeData = nullptr;

void Engine::initialize(const std::string &defaultLocation){
    if ( !isInitialized() ){
        v8::V8::InitializeICUDefaultLocation(defaultLocation.c_str());
        v8::V8::InitializeExternalStartupData(defaultLocation.c_str());

        m_initializeData = new Engine::InitializeData;
        m_initializeData->platform = v8::platform::NewDefaultPlatform();
        m_initializeData->disposeAtExit = false;

        v8::V8::InitializePlatform(m_initializeData->platform.get());
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

// Engine Input
// -------------------------------------------------------------------------------------------

Engine::FileInterceptor::FileInterceptor(LockedFileIOSession::Ptr fileInput)
    : m_fileInput(fileInput)
{
    if ( !m_fileInput )
        m_fileInput = LockedFileIOSession::createInstance();
}

Engine::FileInterceptor::~FileInterceptor(){
}

std::string Engine::FileInterceptor::readFile(const std::string &path) const{
    return m_fileInput->readFromFile(path);
}

bool Engine::FileInterceptor::writeToFile(const std::string &path, const std::string &data) const{
    return m_fileInput->writeToFile(path, data);
}

const LockedFileIOSession::Ptr &Engine::FileInterceptor::fileInput() const{
    return m_fileInput;
}

// Engine Implementation
// --------------------------------------------------------------------------------------------

Engine::Engine(const Compiler::Ptr &compiler)
    : m_d(new EnginePrivate)
{
    if ( !isInitialized() )
        THROW_EXCEPTION(lv::Exception, "Call Engine::Initialize() before creating a new engine.", 0);

    m_d->compiler = compiler;

    // Create a new Isolate and make it the current one.

    m_d->createParams = new v8::Isolate::CreateParams;
    m_d->createParams->array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    m_d->isolate = v8::Isolate::New(*m_d->createParams);
    if ( m_d->isolate->GetNumberOfDataSlots() == 1 ){
        delete m_d->createParams;
        THROW_EXCEPTION(lv::Exception, "Not enough data slots allocated in the isolate.", 1);
    }
    m_d->isolate->SetData(0, this);
    m_d->mainIsolateScope = new v8::Isolate::Scope(m_d->isolate);

    v8::HandleScope handle_scope(isolate());
    v8::Local<v8::Context> context = v8::Context::New(isolate());
    m_d->context = new Context(this, context);

    v8::Local<v8::External> listenerData = v8::External::New(isolate(), this);
    m_d->isolate->AddMessageListener(&EnginePrivate::messageListener, listenerData);

    m_d->isolate->SetHostImportModuleDynamicallyCallback(&EnginePrivate::resolveModuleDynamically);
    m_d->isolate->SetHostInitializeImportMetaObjectCallback(&EnginePrivate::resolveModuleMeta);

    m_d->fileInterceptor = new Engine::FileInterceptor;

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

Script::Ptr Engine::compileJs(const std::string &str){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate(), str.c_str()).ToLocalChecked();

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
Script::Ptr Engine::compileJsEnclosed(const std::string &str){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate(), (Script::encloseStart + str + Script::encloseEnd).c_str()).ToLocalChecked();

    Script::Ptr sc(new Script(this, v8::Script::Compile(context, source).ToLocalChecked()));
    return sc;
}

Script::Ptr Engine::compileJsModuleFile(const std::string &path){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
    if ( !file.is_open() )
        THROW_EXCEPTION(Exception, "Failed to open file: " + path, Exception::toCode("~ScriptFile"));

    std::stringstream sstr;
    sstr << Script::moduleEncloseStart << file.rdbuf() << Script::moduleEncloseEnd;

    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate(), (sstr.str()).c_str()).ToLocalChecked();

    v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, source);
    if ( script.IsEmpty() )
        THROW_EXCEPTION(Exception, "Failed to compile script: " + path, Exception::toCode("~CompileScript"));

    Script::Ptr sc(new Script(this, script.ToLocalChecked(), path));
    return sc;
}

JsModule::Ptr Engine::loadJsModule(const std::string &path){
    auto foundIt = m_d->pathToModule.find(path);
    if (foundIt != m_d->pathToModule.end() ){
        if ( foundIt->second && foundIt->second->status() == JsModule::Ready ){
            return foundIt->second;
        } else {
            THROW_EXCEPTION(
                lv::Exception,
                "Attempting to load a module while the module is being loaded. This might be due to a dependency cycle.",
                lv::Exception::toCode("JsCycle")
            );
        }
    }

    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
    if ( !file.is_open() )
        THROW_EXCEPTION(Exception, "Failed to open file: " + path, Exception::toCode("~ScriptFile"));

    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    std::string code(size, ' ');
    file.seekg(0);
    file.read(&code[0], size);

    v8::Local<v8::String> vcode =
        v8::String::NewFromUtf8(isolate(), code.c_str()).ToLocalChecked();

    v8::ScriptOrigin origin(
        v8::String::NewFromUtf8(isolate(), path.c_str()).ToLocalChecked(),
        v8::Integer::New(isolate(), 0),
        v8::Integer::New(isolate(), 0), v8::False(isolate()),
        v8::Local<v8::Integer>(), v8::Local<v8::Value>(),
        v8::False(isolate()), v8::False(isolate()),
        v8::True(isolate())
    );

    v8::ScriptCompiler::Source source(vcode, origin);
    v8::MaybeLocal<v8::Module> maybeModule = v8::ScriptCompiler::CompileModule(isolate(), &source);

    v8::Local<v8::Module> mod;
    if (!maybeModule.ToLocal(&mod)) {
        THROW_EXCEPTION(lv::Exception, "Failed to load module:" + path, Exception::toCode("~Module"));
    }

    JsModule::Ptr jsModule = JsModule::create(this, path, mod);
    m_d->hashToModule.insert(std::make_pair(mod->GetIdentityHash(), jsModule));
    m_d->pathToModule.insert(std::make_pair(jsModule->filePath(), jsModule));

    std::string pluginPath = QFileInfo(QString::fromStdString(path)).path().toStdString();

    auto module = m_d->compiler->findLoadedModuleByPath(pluginPath);
    if ( module ){
        auto mf = module->findModuleFileByName(jsModule->name() + ".lv");
        if ( mf ){
            jsModule->m_moduleFile = mf;
            mf->setCompilationData(new ModuleFileJsData(jsModule));
        }
    }

    v8::TryCatch tc(m_d->isolate);

    v8::Maybe<bool> result = mod->InstantiateModule(context, &EnginePrivate::resolveModule);

    if ( tc.HasCaught() ){
        CatchData cd(this, &tc);
        handleError(cd.message(), cd.stack(), cd.fileName(), cd.lineNumber());
    }

    if (result.IsNothing()) {
        THROW_EXCEPTION(lv::Exception, "Can't instantiate module: " + path, lv::Exception::toCode("~Module"));
    }

    return jsModule;
}

Element *Engine::runFile(const std::string &path){
    ElementsModule::Ptr module = Compiler::compile(m_d->compiler, path, this);
    ModuleFile* mf = module->moduleFileBypath(path);
    JsModule::Ptr jsMf = loadJsModule(mf->jsFilePath());
    jsMf->evaluate();

    ScopedValue sv = jsMf->moduleNamespace();

    Object::Accessor oa(sv);

    ScopedValue rootValue = oa.get(this, mf->name());
    if ( !rootValue.isElement() ){
        THROW_EXCEPTION(lv::Exception, "File needs to have a single default root item: " + path, lv::Exception::toCode("~Root"));
    }
    Element* root = rootValue.toElement(this);

    return root;
}

ComponentTemplate *Engine::registerTemplate(const MetaObject *t){
    auto it = m_d->registeredTemplates.find(t);
    if ( it != m_d->registeredTemplates.end() )
        return it->second;

    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate());
    if ( t->constructor() ){
        tpl->SetCallHandler(t->constructor()->ptr());
    } else {
        tpl->SetCallHandler(&Constructor::nullImplementation);
    }
    tpl->SetClassName(v8::String::NewFromUtf8(isolate(), t->name().c_str()).ToLocalChecked());

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
        Property* p = propIt->second;
        v8::Local<v8::External> pdata = v8::External::New(isolate(), p);

        if ( p->isWritable() ){
            tplInstance->SetAccessor(
                v8::String::NewFromUtf8(isolate(), propIt->first.c_str()).ToLocalChecked(),
                &Property::ptrGetImplementation,
                &Property::ptrSetImplementation,
                pdata
            );
        } else {
            tplInstance->SetAccessor(
                v8::String::NewFromUtf8(isolate(), propIt->first.c_str()).ToLocalChecked(),
                &Property::ptrGetImplementation,
                nullptr,
                pdata
            );
        }
    }

    // Methods
    v8::Local<v8::Template> tplPrototype = tpl->PrototypeTemplate(); // for methods and events
    for ( auto funcIt = t->ownMethodsBegin(); funcIt != t->ownMethodsEnd(); ++funcIt ){
        Function* f = funcIt->second;
        v8::Local<v8::External> fdata = v8::External::New(isolate(), f);
        tplPrototype->Set(isolate(), funcIt->first.c_str(), v8::FunctionTemplate::New(isolate(), f->ptr(), fdata));
    }

    // Events
    for ( auto eventIt = t->ownEventsBegin(); eventIt != t->ownEventsEnd(); ++eventIt ){
        Function* f = eventIt->second;
        v8::Local<v8::External> fdata = v8::External::New(isolate(), f);
        tplPrototype->Set(isolate(), eventIt->first.c_str(), v8::FunctionTemplate::New(isolate(), f->ptr(), fdata));
    }

    // Functions
    v8::Local<v8::Function> ftpl = tpl->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked();
    for ( auto funcIt = t->functionsBegin(); funcIt != t->functionsEnd(); ++funcIt ){
        Function* f = funcIt->second;
        v8::Local<v8::External> fdata = v8::External::New(isolate(), f);
        ftpl->Set(
            isolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(isolate(), funcIt->first.c_str()).ToLocalChecked(),
            v8::FunctionTemplate::New(isolate(), f->ptr(), fdata)->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
        ).IsNothing();
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

// A inherits B, means A.prototype.__proto__ == B.prototype
// js implementation:
// function inherits(Child, Parent) {
//    _ = Child.prototype;
//    while (_ != null) {
//       if (_ == Parent.prototype)
//          return true;
//       _ = _.__proto__;
//    }
//    return false;
// }
bool Engine::isElementConstructor(const Callable &c){
    v8::Local<v8::Function> elemClass = m_d->elementTemplate->data.Get(isolate())->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Function> childClass = c.data();

    v8::Local<v8::Value> elemClassPrototype = elemClass->Get(isolate()->GetCurrentContext(), v8::String::NewFromUtf8(isolate(), "prototype").ToLocalChecked()).ToLocalChecked();
    v8::Local<v8::Value> childClassPrototype = childClass->Get(isolate()->GetCurrentContext(), v8::String::NewFromUtf8(isolate(), "prototype").ToLocalChecked()).ToLocalChecked();

    while (childClassPrototype->IsObject()){
        v8::Maybe<bool> eq = childClassPrototype->Equals(m_d->context->asLocal(), elemClassPrototype);
        if ( eq.IsJust() && eq.ToChecked()){
            return true;
        }

        v8::Local<v8::Object> fncPrototypeCast = v8::Local<v8::Object>::Cast(childClassPrototype);
        childClassPrototype = fncPrototypeCast->Get(isolate()->GetCurrentContext(), v8::String::NewFromUtf8(isolate(), "__proto__").ToLocalChecked()).ToLocalChecked();
    }

    return false;
}

/**
 * @brief Throws a javascript error within the engine
 * @param exception
 * @param object
 *
 * You are not allowed to call any js functions until the engine returns to the Js
 * execution context or exists a TryCatch block.
 *
 * TryCatch blocks remove the exception when they exit if their nesting is smaller
 * than that of the exception.
 */
void Engine::throwError(const Exception *exception, Element *object){
    v8::Local<v8::Value> e;
    auto se = dynamic_cast<const SyntaxException*>(exception);
    if (se)
    {
        e = v8::Exception::SyntaxError(
            v8::String::NewFromUtf8(m_d->isolate, exception->message().c_str()).ToLocalChecked());
    }
    else {
        e = v8::Exception::Error(
            v8::String::NewFromUtf8(m_d->isolate, exception->message().c_str()).ToLocalChecked());
    }

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(e);
    std::stringstream stackCapture;

    v8::Local<v8::StackTrace> st = v8::StackTrace::CurrentStackTrace(isolate(), 128, v8::StackTrace::kScriptName);
    int jsStackFrameSize = st->GetFrameCount();
    for (int i = 0; i<jsStackFrameSize; ++i)
    {
        if (i!=0) stackCapture << "\n";
        v8::Local<v8::StackFrame> sf = st->GetFrame(isolate(), i);
        v8::String::Utf8Value scriptName(isolate(), sf->GetScriptName());
        v8::String::Utf8Value functionName(isolate(), sf->GetFunctionName());

        stackCapture << "at ";
        if (functionName.length() != 0)
            stackCapture << *functionName;
        stackCapture << "(";
        if (scriptName.length() != 0)
            stackCapture << *scriptName;
        stackCapture << ":" <<  sf->GetLineNumber() << ")";
    }

    if ( exception->hasStackTrace() ){

        StackTrace::Ptr est = exception->stackTrace();
        for ( auto it = est->begin(); it != est->end(); ++it ){
            if ( it != est->begin() || jsStackFrameSize > 0)
                stackCapture << "\n";

            const StackFrame& sf = *it;
            if ( sf.line() ){
                stackCapture << "at " << sf.functionName().c_str() << "(" << sf.fileName().c_str() << ":" << sf.line()
                   << ")" << "[0x" << std::setbase(16) << sf.address() << "]" << std::setbase(10);
            } else {
                stackCapture << "at " << sf.functionName().c_str() << "[0x" << std::setbase(16) << sf.address()
                             << std::setbase(10) << "]";
            }
        }

    }

    v8::Local<v8::String> stackKey = v8::String::NewFromUtf8(isolate(), "stack", v8::NewStringType::kInternalized).ToLocalChecked();
    v8::Local<v8::String> stackValue = v8::String::NewFromUtf8(isolate(), stackCapture.str().c_str(), v8::NewStringType::kInternalized).ToLocalChecked();

    o->Set(isolate()->GetCurrentContext(), stackKey, stackValue).IsNothing();

    if (se){
        v8::Local<v8::String> lineNumberKey = v8::String::NewFromUtf8(isolate(), "lineNumber", v8::NewStringType::kInternalized).ToLocalChecked();
        o->Set(isolate()->GetCurrentContext(), lineNumberKey, v8::Integer::New(isolate(), se->parsedLine())).IsNothing();


        v8::Local<v8::String> columnKey = v8::String::NewFromUtf8(isolate(), "column", v8::NewStringType::kInternalized).ToLocalChecked();
        o->Set(isolate()->GetCurrentContext(), columnKey, v8::Integer::New(isolate(), se->parsedColumn())).IsNothing();

        v8::Local<v8::String> offsetKey = v8::String::NewFromUtf8(isolate(), "offset", v8::NewStringType::kInternalized).ToLocalChecked();
        o->Set(isolate()->GetCurrentContext(), offsetKey, v8::Integer::New(isolate(), se->parsedColumn())).IsNothing();

        v8::Local<v8::String> fileNameKey = v8::String::NewFromUtf8(isolate(), "fileName", v8::NewStringType::kInternalized).ToLocalChecked();
        o->Set(isolate()->GetCurrentContext(), fileNameKey, v8::String::NewFromUtf8(isolate(), se->fileName().c_str(), v8::NewStringType::kInternalized).ToLocalChecked()).IsNothing();
    } else {
        if ( object ){
            v8::Local<v8::String> objectKey = v8::String::NewFromUtf8(isolate(), "object", v8::NewStringType::kInternalized).ToLocalChecked();
            o->Set(isolate()->GetCurrentContext(), objectKey, ElementPrivate::localObject(object)).IsNothing();
        }

        v8::Local<v8::String> fileNameKey = v8::String::NewFromUtf8(isolate(), "fileName", v8::NewStringType::kInternalized).ToLocalChecked();
        o->Set(isolate()->GetCurrentContext(), fileNameKey, v8::String::NewFromUtf8(isolate(), exception->file().c_str(), v8::NewStringType::kInternalized).ToLocalChecked()).IsNothing();

        v8::Local<v8::String> lineNumberKey = v8::String::NewFromUtf8(isolate(), "lineNumber", v8::NewStringType::kInternalized).ToLocalChecked();
        o->Set(isolate()->GetCurrentContext(), lineNumberKey, v8::Integer::New(isolate(), exception->line())).IsNothing();
    }

    m_d->pendingExceptionNesting = m_d->tryCatchNesting;
    m_d->isolate->ThrowException(e);
}

/**
 * @brief Engine::tryCatch
 * @param f
 * @param c
 */
void Engine::tryCatch(const std::function<void ()> &f, const std::function<void(const Engine::CatchData &)> &c){
    v8::TryCatch tc(m_d->isolate);
    incrementTryCatchNesting();
    f();
    decrementTryCatchNesting();

    if ( tc.HasCaught() ){
        c(CatchData(this, &tc));
    }
}

bool Engine::hasPendingException(){
    return m_d->pendingExceptionNesting >= 0;
}

void Engine::incrementTryCatchNesting(){
    m_d->tryCatchNesting++;
}

/**
 * @brief Decrement the nesting depth of trycatch block. Will remove any exception
 * nested below the block
 */
void Engine::decrementTryCatchNesting(){
    if ( m_d->pendingExceptionNesting >= m_d->tryCatchNesting )
        m_d->pendingExceptionNesting = -1;
    m_d->tryCatchNesting--;
}

void Engine::clearPendingException(){
    m_d->pendingExceptionNesting = -1;
}

void Engine::handleError(const std::string &message, const std::string &stack, const std::string &file, int line){
    vlog("engine").e() << "Uncaught exception occured:" << message.c_str() << " at " << file.c_str() << "(" << line
               << ")\n" << stack.c_str();
}

Engine::ModuleFileType Engine::moduleFileType() const{
    return m_d->moduleFileType;
}

void Engine::setModuleFileType(Engine::ModuleFileType type){
    m_d->moduleFileType = type;
}

const LanguageParser::Ptr &Engine::parser() const{
    return m_d->compiler->parser();
}

const Compiler::Ptr &Engine::compiler() const{
    return m_d->compiler;
}

const Engine::FileInterceptor *Engine::fileInterceptor() const{
    return m_d->fileInterceptor;
}

void Engine::setFileInterceptor(Engine::FileInterceptor *fileInterceptor){
    delete m_d->fileInterceptor;
    m_d->fileInterceptor = fileInterceptor;
}

void Engine::importInternals(){
    m_d->compiler->configureImplicitType("vlog");
    m_d->compiler->configureImplicitType("console");
    m_d->compiler->configureImplicitType("import");
    m_d->compiler->configureImplicitType("Object");

    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    m_d->elementTemplate = registerTemplate(&Element::metaObject());
    ComponentTemplate* listTemplate = registerTemplate(&List::metaObject());
    ComponentTemplate* containerTemplate = registerTemplate(&Container::metaObject());
    ComponentTemplate* errorHandlerTemplate = registerTemplate(&ErrorHandler::metaObject());
    ComponentTemplate* tupleTemplate = registerTemplate(&Tuple::metaObject());

    v8::Local<v8::FunctionTemplate> tpl = m_d->elementTemplate->data.Get(isolate());
    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "Element").ToLocalChecked(),
        tpl->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    v8::Local<v8::FunctionTemplate> listTpl = listTemplate->data.Get(isolate());
    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "List").ToLocalChecked(),
        listTpl->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    v8::Local<v8::FunctionTemplate> containerTpl = containerTemplate->data.Get(isolate());
    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "Container").ToLocalChecked(),
        containerTpl->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    v8::Local<v8::FunctionTemplate> errorHandlerTpl = errorHandlerTemplate->data.Get(isolate());
    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "ErrorHandler").ToLocalChecked(),
        errorHandlerTpl->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    v8::Local<v8::FunctionTemplate> tupleTpl = tupleTemplate->data.Get(isolate());
    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "Tuple").ToLocalChecked(),
        tupleTpl->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    m_d->pointTemplate.Reset(isolate(), Point::functionTemplate(isolate()));
    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "Point").ToLocalChecked(),
        pointTemplate()->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    m_d->sizeTemplate.Reset(isolate(), Size::functionTemplate(isolate()));
    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "Size").ToLocalChecked(),
        sizeTemplate()->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    m_d->rectangleTemplate.Reset(isolate(), Rectangle::functionTemplate(isolate()));
    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "Rectangle").ToLocalChecked(),
        rectangleTemplate()->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "vlog").ToLocalChecked(),
        VisualLogJsObject::functionTemplate(isolate())->InstanceTemplate()->NewInstance(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();

    context->Global()->Set(
        isolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(isolate(), "linkError").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate(), &linkError)->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
    ).IsNothing();
}

void Engine::wrapScriptObject(Element *element){
    ComponentTemplate* ctpl = registerTemplate(&element->typeMetaObject());
    v8::Local<v8::FunctionTemplate> tpl = ctpl->data.Get(isolate());
    v8::Local<v8::Object> instance = tpl->InstanceTemplate()->NewInstance(isolate()->GetCurrentContext()).ToLocalChecked();
    element->setLifeTimeWithObject(instance);
}

int Engine::tryCatchNesting(){
    return m_d->tryCatchNesting;
}

void Engine::setGlobalErrorHandler(bool value){
    m_d->hasGlobalErrorHandler = value;
}

// Module caching

Object Engine::require(ModuleLibrary *module, const Object& o){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> base = o.data();
    Object exportsObject(this, base);

    for ( auto it = module->typesBegin(); it != module->typesEnd(); ++it ){
        const MetaObject* t = it->second;

        ComponentTemplate* ctpl = registerTemplate(t);
        v8::Local<v8::FunctionTemplate> tpl = ctpl->data.Get(isolate());

        base->Set(
            isolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(isolate(), t->name().c_str()).ToLocalChecked(),
            tpl->GetFunction(isolate()->GetCurrentContext()).ToLocalChecked()
        ).IsNothing();
    }

    for ( auto it = module->instancesBegin(); it != module->instancesEnd(); ++it ){
        std::string name = it->first;
        Element* e       = it->second;
        v8::Local<v8::Object> lo = ElementPrivate::localObject(e);
        base->Set(
            isolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(isolate(), name.c_str()).ToLocalChecked(),
            lo
        ).IsNothing();
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

bool Engine::CatchData::canContinue() const{
    return m_tryCatch->CanContinue();
}

bool Engine::CatchData::hasTerminated() const
{
    return m_tryCatch->HasTerminated();
}

void Engine::CatchData::rethrow(){
    m_tryCatch->ReThrow();
}

std::string Engine::CatchData::message() const{
    auto isolate = m_engine->isolate();
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::String> messageKey = v8::String::NewFromUtf8(isolate, "message", v8::NewStringType::kInternalized).ToLocalChecked();
    v8::Local<v8::Object> exceptionObj = m_tryCatch->Exception()->ToObject(context).ToLocalChecked();
    if ( exceptionObj->Has(context, messageKey).ToChecked() ){
        v8::String::Utf8Value result(isolate, exceptionObj->Get(context, messageKey).ToLocalChecked());
        return *result;
    } else {
        v8::String::Utf8Value msg(isolate, m_tryCatch->Message()->Get());
        return *msg;
    }
}

std::string Engine::CatchData::stack() const{
    auto context = m_engine->isolate()->GetCurrentContext();
    v8::String::Utf8Value msg(m_engine->isolate(), m_tryCatch->StackTrace(context).ToLocalChecked());
    return *msg;
}

std::string Engine::CatchData::fileName() const{
    auto isolate = m_engine->isolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::String> fileNameKey = v8::String::NewFromUtf8(m_engine->isolate(), "fileName", v8::NewStringType::kInternalized).ToLocalChecked();
    v8::Local<v8::Object> exceptionObj = m_tryCatch->Exception()->ToObject(context).ToLocalChecked();
    if ( exceptionObj->Has(context, fileNameKey).ToChecked() ){
        v8::String::Utf8Value result(isolate, exceptionObj->Get(context, fileNameKey).ToLocalChecked());
        return *result;
    } else {
        v8::String::Utf8Value file(isolate, m_tryCatch->Message()->GetScriptResourceName()->ToString(context).ToLocalChecked());
        return *file;
    }
}

int Engine::CatchData::lineNumber() const{
    auto isolate = m_engine->isolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::String> lineNumberKey = v8::String::NewFromUtf8(m_engine->isolate(), "lineNumber", v8::NewStringType::kInternalized).ToLocalChecked();
    v8::Local<v8::Object> exceptionObj = m_tryCatch->Exception()->ToObject(context).ToLocalChecked();
    if ( exceptionObj->Has(context, lineNumberKey).ToChecked() ){
        return exceptionObj->Get(context, lineNumberKey).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value();
    }

    return m_tryCatch->Message()->GetLineNumber(context).ToChecked();
}

Element *Engine::CatchData::object() const{
    auto isolate = m_engine->isolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(m_tryCatch->Exception());
    v8::Local<v8::String> oKey = v8::String::NewFromUtf8(m_engine->isolate(), "object", v8::NewStringType::kInternalized).ToLocalChecked();

    if ( o->Has(context, oKey).ToChecked() ){
        v8::Local<v8::Object> obj = o->Get(context, oKey).ToLocalChecked()->ToObject(context).ToLocalChecked();
        if ( obj->InternalFieldCount() == 1 ){
            v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(obj->GetInternalField(0));
            void* ptr = wrap->Value();
            return reinterpret_cast<Element*>(ptr);
        }
    }

    return nullptr;
}

Engine::CatchData::CatchData(Engine *engine, v8::TryCatch *tc)
    : m_engine(engine), m_tryCatch(tc)
{
}

}} // namespace lv, el


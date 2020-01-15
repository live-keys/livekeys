#include "engine.h"
#include "element.h"
#include "element_p.h"
#include "value_p.h"
#include "context_p.h"
#include "container.h"
#include "live/exception.h"
#include "live/visuallog.h"
#include "visuallogjsobject.h"
#include "errorhandler.h"
#include "imports_p.h"
#include "tuple.h"

#include <sstream>
#include <iomanip>
#include <QFileInfo>
#include "v8nowarnings.h"
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
        , parser(LanguageParser::createForElements())
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
    v8::Persistent<v8::FunctionTemplate> importsTemplate;

    int  tryCatchNesting;
    int  pendingExceptionNesting;
    bool hasGlobalErrorHandler;

    Engine::ModuleFileType moduleFileType;

    LanguageParser::Ptr parser;

    PackageGraph* packageGraph;
    std::map<std::string, ElementsPlugin::Ptr> loadedPlugins;

public: // helpers
    bool isElementConstructor(
        Engine *engine,
        const v8::Local<v8::Function>& fnc,
        const v8::Local<v8::Function> &elementFnc
    );

    static void messageListener(v8::Local<v8::Message> message, v8::Local<v8::Value> data);

};

void EnginePrivate::messageListener(v8::Local<v8::Message> message, v8::Local<v8::Value> data){
    v8::Local<v8::External> engineData = v8::Local<v8::External>::Cast(data);
    Engine* engine = reinterpret_cast<Engine*>(engineData->Value());

    v8::String::Utf8Value msg(message->Get());
    v8::String::Utf8Value file(v8::Local<v8::String>::Cast(message->GetScriptResourceName()));
    int line = message->GetScriptOrigin().ResourceLineOffset()->Int32Value();

    engine->handleError(*msg, "", *file, line);
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

Engine::Engine(PackageGraph *pg)
    : m_d(new EnginePrivate)
{
    if ( !isInitialized() )
        THROW_EXCEPTION(lv::Exception, "Call Engine::Initialize() before creating a new engine.", 0);

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

    m_d->packageGraph = (pg == nullptr) ? new PackageGraph : pg;

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
Script::Ptr Engine::compileJsEnclosed(const std::string &str){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate(), (Script::encloseStart + str + Script::encloseEnd).c_str());

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

    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate(), (sstr.str()).c_str());

    v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, source);
    if ( script.IsEmpty() )
        THROW_EXCEPTION(Exception, "Failed to compile script: " + path, Exception::toCode("~CompileScript"));

    Script::Ptr sc(new Script(this, script.ToLocalChecked(), path));
    return sc;
}

Script::Ptr Engine::compileModuleFile(const std::string &path){
    if ( moduleFileType() == Engine::JsOnly ){
            return compileJsModuleFile(path + ".js");
    }

    if (m_d->moduleFileType == Engine::JsOnly)
    {
        std::string resPath = path;
        if (path.substr(path.length()-3) != ".js")
            resPath += ".js";
        QFileInfo fileInfo = QFileInfo(QString(resPath.c_str()));
        if (!fileInfo.exists())
        {
            fileInfo = QFileInfo(QString((path).c_str()));
            resPath = path;
            if (!fileInfo.exists())
                THROW_EXCEPTION(Exception, "Failed to open .lv file: " + path, Exception::toCode("~ScriptFile"));
        }
        QFile file(QString(resPath.c_str()));
        file.open(QFile::Text | QFile::ReadOnly);
        QString text = file.readAll();
        return compileModuleSource(resPath, text.toStdString());
    } else if (m_d->moduleFileType == Engine::Lv){
        QFileInfo fileInfo(QString((path).c_str()));
        if (!fileInfo.exists()) return nullptr;
        QFile file(QString(path.c_str()));
        file.open(QFile::Text | QFile::ReadOnly);
        QString text = file.readAll();
        return compileModuleSource(path, text.toStdString());
    } else if (m_d->moduleFileType == Engine::LvOrJs){
        QFileInfo fi1(path.c_str());
        QFileInfo fi2((path+".js").c_str());
        if (!fi1.exists())
            THROW_EXCEPTION(Exception, "Failed to open .lv file: " + path, Exception::toCode("~ScriptFile"));
        if (!fi2.exists())
        {
            QFile file(QString(path.c_str()));
            file.open(QFile::Text | QFile::ReadOnly);
            QString text = file.readAll();
            return compileModuleSource(path, text.toStdString());
        }
        if (fi2.lastModified() > fi1.lastModified())
        {
            QFile file(QString((path+".js").c_str()));
            file.open(QFile::Text | QFile::ReadOnly);
            QString text = file.readAll();
            return compileModuleSource(path+".js", text.toStdString());
        }
        QFile file(QString(path.c_str()));
        file.open(QFile::Text | QFile::ReadOnly);
        QString text = file.readAll();
        return compileModuleSource(path, text.toStdString());
    }

    return nullptr;
}

Script::Ptr Engine::compileModuleSource(const std::string &path, const std::string &source){
    std::string jssource = m_d->parser->toJs(source, QFileInfo(QString::fromStdString(path)).baseName().toStdString());

    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
    if ( !file.is_open() )
        THROW_EXCEPTION(Exception, "Failed to open file: " + path, Exception::toCode("~ScriptFile"));

    std::string jssourceEnclosed = Script::moduleEncloseStart + jssource + Script::moduleEncloseEnd;

    v8::Local<v8::String> sourceLocal = v8::String::NewFromUtf8(isolate(), jssourceEnclosed.c_str());

    v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, sourceLocal);
    if ( script.IsEmpty() )
        THROW_EXCEPTION(Exception, "Failed to compile script: " + path, Exception::toCode("~CompileScript"));

    Script::Ptr sc(new Script(this, script.ToLocalChecked(), path));
    return sc;
}

Object Engine::loadJsFile(const std::string &path){
    QFileInfo finfo(QString::fromStdString(path));
    std::string pluginPath = finfo.path().toStdString();
    std::string fileName = finfo.fileName().toStdString();
    std::string baseName = finfo.baseName().toStdString();

    Plugin::Ptr plugin(nullptr);
    if ( Plugin::existsIn(pluginPath) ){
        plugin = Plugin::createFromPath(pluginPath);
        Package::Ptr package = Package::createFromPath(plugin->package());
        m_d->packageGraph->loadRunningPackageAndPlugin(package, plugin);
    } else {
        plugin = m_d->packageGraph->createRunningPlugin(pluginPath);
    }

    ElementsPlugin::Ptr epl = ElementsPlugin::create(plugin, this);
    ModuleFile* mf = ElementsPlugin::addModuleFile(epl, baseName);

    Script::Ptr sc = compileJsModuleFile(path);
    Object m = sc->loadAsModule(mf);
    if ( m.isNull() )
        return m;

    LocalObject lm(m);
    return lm.get(this, "exports").toObject(this);
}

Object Engine::loadFile(const std::string &path){
    QFileInfo finfo(QString::fromStdString(path));
    std::string pluginPath = finfo.path().toStdString();
    std::string fileName = finfo.fileName().toStdString();

    Plugin::Ptr plugin(nullptr);
    if ( Plugin::existsIn(pluginPath) ){
        plugin = Plugin::createFromPath(pluginPath);
        Package::Ptr package = Package::createFromPath(plugin->package());
        m_d->packageGraph->loadRunningPackageAndPlugin(package, plugin);
    } else {
        plugin = m_d->packageGraph->createRunningPlugin(pluginPath);
    }

    ElementsPlugin::Ptr epl = ElementsPlugin::create(plugin, this);
    ModuleFile* mf = ElementsPlugin::addModuleFile(epl, fileName);

    Script::Ptr sc = compileModuleFile(path);
    Object m = sc->loadAsModule(mf);
    if ( m.isNull() )
        return m;

    LocalObject lm(m);
    return lm.get(this, "exports").toObject(this);
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
                nullptr,
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

v8::Local<v8::FunctionTemplate> Engine::importsTemplate(){
    return m_d->importsTemplate.Get(isolate());
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
    v8::Local<v8::Function> elemClass = m_d->elementTemplate->data.Get(isolate())->GetFunction();
    v8::Local<v8::Function> childClass = c.data();

    v8::Local<v8::Value> elemClassPrototype = elemClass->Get(v8::String::NewFromUtf8(isolate(), "prototype"));
    v8::Local<v8::Value> childClassPrototype = childClass->Get(v8::String::NewFromUtf8(isolate(), "prototype"));

    while (childClassPrototype->IsObject()){
        v8::Maybe<bool> eq = childClassPrototype->Equals(m_d->context->asLocal(), elemClassPrototype);
        if ( eq.IsJust() && eq.ToChecked()){
            return true;
        }

        v8::Local<v8::Object> fncPrototypeCast = v8::Local<v8::Object>::Cast(childClassPrototype);
        childClassPrototype = fncPrototypeCast->Get(v8::String::NewFromUtf8(isolate(), "__proto__"));
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
    v8::Local<v8::Value> e = v8::Exception::Error(
        v8::String::NewFromUtf8(m_d->isolate, exception->message().c_str()));

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(e);

    if ( exception->hasStackTrace() ){
        std::stringstream stackCapture;

        StackTrace::Ptr est = exception->stackTrace();
        for ( auto it = est->begin(); it != est->end(); ++it ){
            if ( it != est->begin() )
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


        v8::Local<v8::String> stackKey = v8::String::NewFromUtf8(isolate(), "stack", v8::String::kInternalizedString);
        v8::Local<v8::String> stackValue = v8::String::NewFromUtf8(isolate(), stackCapture.str().c_str(), v8::String::kInternalizedString);

        o->Set(stackKey, stackValue);
    }

    if ( object ){
        v8::Local<v8::String> objectKey = v8::String::NewFromUtf8(isolate(), "object", v8::String::kInternalizedString);
        o->Set(objectKey, ElementPrivate::localObject(object));
    }

    v8::Local<v8::String> fileNameKey = v8::String::NewFromUtf8(isolate(), "fileName", v8::String::kInternalizedString);
    o->Set(fileNameKey, v8::String::NewFromUtf8(isolate(), exception->file().c_str(), v8::String::kInternalizedString));

    v8::Local<v8::String> lineNumberKey = v8::String::NewFromUtf8(isolate(), "lineNumber", v8::String::kInternalizedString);
    o->Set(lineNumberKey, v8::Integer::New(isolate(), exception->line()));


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

const std::vector<std::string> Engine::packageImportPaths() const{
    return m_d->packageGraph->packageImportPaths();
}

void Engine::setPackageImportPaths(const std::vector<std::string> &paths){
    m_d->packageGraph->setPackageImportPaths(paths);
}

void Engine::handleError(const std::string &message, const std::string &stack, const std::string &file, int line){
    vlog().e() << "Uncaught exception occured:" << message.c_str() << " at " << file.c_str() << "(" << line
               << ")\n" << stack.c_str();
}

Engine::ModuleFileType Engine::moduleFileType() const{
    return m_d->moduleFileType;
}

void Engine::setModuleFileType(Engine::ModuleFileType type){
    m_d->moduleFileType = type;
}

const LanguageParser::Ptr &Engine::parser() const{
    return m_d->parser;
}

void Engine::importInternals(){
    v8::HandleScope handle(isolate());
    v8::Local<v8::Context> context = m_d->context->asLocal();
    v8::Context::Scope context_scope(context);

    m_d->elementTemplate = registerTemplate(&Element::metaObject());
    ComponentTemplate* listTemplate = registerTemplate(&List::metaObject());
    ComponentTemplate* containerTemplate = registerTemplate(&Container::metaObject());
    ComponentTemplate* errorHandlerTemplate = registerTemplate(&ErrorHandler::metaObject());
    ComponentTemplate* tupleTemplate = registerTemplate(&Tuple::metaObject());

    v8::Local<v8::FunctionTemplate> tpl = m_d->elementTemplate->data.Get(isolate());
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Element"), tpl->GetFunction());

    v8::Local<v8::FunctionTemplate> listTpl = listTemplate->data.Get(isolate());
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "List"), listTpl->GetFunction());

    v8::Local<v8::FunctionTemplate> containerTpl = containerTemplate->data.Get(isolate());
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Container"), containerTpl->GetFunction());

    v8::Local<v8::FunctionTemplate> errorHandlerTpl = errorHandlerTemplate->data.Get(isolate());
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "ErrorHandler"), errorHandlerTpl->GetFunction());

    v8::Local<v8::FunctionTemplate> tupleTpl = tupleTemplate->data.Get(isolate());
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Tuple"), tupleTpl->GetFunction());

    m_d->pointTemplate.Reset(isolate(), Point::functionTemplate(isolate()));
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Point"), pointTemplate()->GetFunction());

    m_d->sizeTemplate.Reset(isolate(), Size::functionTemplate(isolate()));
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Size"), sizeTemplate()->GetFunction());

    m_d->rectangleTemplate.Reset(isolate(), Rectangle::functionTemplate(isolate()));
    context->Global()->Set(v8::String::NewFromUtf8(isolate(), "Rectangle"), rectangleTemplate()->GetFunction());

    m_d->importsTemplate.Reset(isolate(), Imports::functionTemplate(isolate()));

    context->Global()->Set(
        v8::String::NewFromUtf8(isolate(), "vlog"),
        VisualLogJsObject::functionTemplate(isolate())->InstanceTemplate()->NewInstance());

    context->Global()->Set(
        v8::String::NewFromUtf8(isolate(), "linkError"),
        v8::FunctionTemplate::New(isolate(), &linkError)->GetFunction());
}

void Engine::wrapScriptObject(Element *element){
    ComponentTemplate* ctpl = registerTemplate(&element->typeMetaObject());
    v8::Local<v8::FunctionTemplate> tpl = ctpl->data.Get(isolate());
    v8::Local<v8::Object> instance = tpl->InstanceTemplate()->NewInstance();
    element->setLifeTimeWithObject(instance);
}

int Engine::tryCatchNesting(){
    return m_d->tryCatchNesting;;
}

void Engine::setGlobalErrorHandler(bool value){
    m_d->hasGlobalErrorHandler = value;
}

// Module caching

Object Engine::require(ModuleLibrary *module){
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

ElementsPlugin::Ptr Engine::require(const std::string &importKey, Plugin::Ptr requestingPlugin){
    auto foundEp = m_d->loadedPlugins.find(importKey);
    if ( foundEp == m_d->loadedPlugins.end() ){
        Plugin::Ptr plugin = m_d->packageGraph->loadPlugin(importKey, requestingPlugin);
        return ElementsPlugin::create(plugin, this);
    } else {
        return foundEp->second;
    }
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
    v8::Local<v8::String> messageKey = v8::String::NewFromUtf8(m_engine->isolate(), "message", v8::String::kInternalizedString);
    v8::Local<v8::Object> exceptionObj = m_tryCatch->Exception()->ToObject();
    if ( exceptionObj->Has(messageKey) ){
        v8::String::Utf8Value result(exceptionObj->Get(messageKey)->ToString());
        return *result;
    } else {
        v8::String::Utf8Value msg(m_tryCatch->Message()->Get());
        return *msg;
    }
}

std::string Engine::CatchData::stack() const{
    v8::String::Utf8Value msg(m_tryCatch->StackTrace()->ToString());
    return *msg;
}

std::string Engine::CatchData::fileName() const{
    v8::Local<v8::String> fileNameKey = v8::String::NewFromUtf8(m_engine->isolate(), "fileName", v8::String::kInternalizedString);
    v8::Local<v8::Object> exceptionObj = m_tryCatch->Exception()->ToObject();
    if ( exceptionObj->Has(fileNameKey) ){
        v8::String::Utf8Value result(exceptionObj->Get(fileNameKey)->ToString());
        return *result;
    } else {
        v8::String::Utf8Value file(m_tryCatch->Message()->GetScriptResourceName()->ToString());
        return *file;
    }
}

int Engine::CatchData::lineNumber() const{
    v8::Local<v8::String> lineNumberKey = v8::String::NewFromUtf8(m_engine->isolate(), "lineNumber", v8::String::kInternalizedString);
    v8::Local<v8::Object> exceptionObj = m_tryCatch->Exception()->ToObject();
    if ( exceptionObj->Has(lineNumberKey) ){
        return exceptionObj->Get(lineNumberKey)->Int32Value();
    }

    return m_tryCatch->Message()->GetLineNumber();
}

Element *Engine::CatchData::object() const{
    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(m_tryCatch->Exception());
    v8::Local<v8::String> oKey = v8::String::NewFromUtf8(m_engine->isolate(), "object", v8::String::kInternalizedString);

    if ( o->Has(oKey) ){
        v8::Local<v8::Object> obj = o->Get(oKey)->ToObject();
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

}} // namespace lv, script

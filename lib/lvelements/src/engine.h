#ifndef LVENGINE_H
#define LVENGINE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/value.h"
#include "live/elements/script.h"
#include "live/elements/elementsplugin.h"
#include "live/packagegraph.h"

namespace lv{

class Exception;

namespace el{

class Context;
class ComponentTemplate;
class EnginePrivate;
class LV_ELEMENTS_EXPORT Engine{

    friend class EnginePrivate;
    friend class Script;
    friend class Element;
    friend class ElementPrivate;

    // Initialization Functionality
    // ----------------------------

public:
    class InitializeData;
    class LV_ELEMENTS_EXPORT InitializeScope{
    public:
        InitializeScope(const std::string& defaultLocation);
        ~InitializeScope();
    };

    static void initialize(const std::string& defaultLocation);
    static void dispose();
    static bool isInitialized();
    static void disposeAtExit();

    class LV_ELEMENTS_EXPORT CatchData{

        friend class Engine;
        friend class Script;

    public:
        bool canContinue() const;
        bool hasTerminated() const;
        void rethrow();
        std::string message() const;
        std::string stack() const;
        std::string fileName() const;
        int lineNumber() const;
        Element* object() const;

    private:
        CatchData(Engine* engine, v8::TryCatch* tc);

        Engine*       m_engine;
        v8::TryCatch* m_tryCatch;
    };

private:
    static InitializeData* m_initializeData;

    // Engine
    // ------

public:
    Engine(PackageGraph* pg = nullptr);
    ~Engine();

    Object require(ModuleLibrary* module);
    ElementsPlugin::Ptr require(const std::string& importKey);

    void scope(const std::function<void()> &f);

    v8::Isolate* isolate();

    Context* currentContext() const;
    Script::Ptr compile(const std::string& str);
    Script::Ptr compileEnclosed(const std::string& str);
    Script::Ptr compileJsFile(const std::string& path);

    Script::Ptr compileElement(const std::string& source);

    ComponentTemplate* registerTemplate(const MetaObject* mo);
    v8::Local<v8::FunctionTemplate> pointTemplate();
    v8::Local<v8::FunctionTemplate> sizeTemplate();
    v8::Local<v8::FunctionTemplate> rectangleTemplate();
    v8::Local<v8::FunctionTemplate> importsTemplate();

    bool isElementConstructor(const Callable& c);

    void throwError(const lv::Exception* exception, Element* object);
    void tryCatch(const std::function<void()>& f, const std::function<void(const CatchData&)>& c);

    bool hasPendingException();
    void incrementTryCatchDepth();
    void decrementTryCatchDepth();
    void clearPendingException();

    void handleError(const std::string& message, const std::string& stack, const std::string& file, int line);

private:
    void importInternals();
    void wrapScriptObject(Element* element);

    bool hasGlobalErrorHandler();
    void setGlobalErrorHandler(bool value);

    EnginePrivate* m_d;
};

}} // namespace lv, script

#endif // LVENGINE_H

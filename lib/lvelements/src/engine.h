#ifndef LVENGINE_H
#define LVENGINE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/value.h"
#include "live/elements/script.h"
#include "live/elements/elementsmodule.h"
#include "live/elements/languageparser.h"
#include "live/elements/jsmodule.h"
#include "live/packagegraph.h"
#include "live/lockedfileiosession.h"

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

    // Exception CatchData
    // -------------------

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

    // File reading functionality
    // --------------------------

    class LV_ELEMENTS_EXPORT FileInterceptor{
    public:
        FileInterceptor(LockedFileIOSession::Ptr fileInput = nullptr);
        virtual ~FileInterceptor();

        virtual std::string readFile(const std::string& path) const;
        virtual bool writeToFile(const std::string& path, const std::string& data) const;

        const LockedFileIOSession::Ptr& fileInput() const;
    private:
        LockedFileIOSession::Ptr m_fileInput;
    };

private:
    static InitializeData* m_initializeData;

    // Engine
    // ------

public:
    enum ModuleFileType{
        Lv = 0,
        LvOrJs,
        JsOnly
    };

public:
    Engine(PackageGraph* pg = nullptr);
    ~Engine();

    Object require(ModuleLibrary* module, const Object& o);
    ElementsModule::Ptr require(const std::string& importKey, Plugin::Ptr requestingPlugin = nullptr);

    void scope(const std::function<void()> &f);

    v8::Isolate* isolate();

    Context* currentContext() const;

    Script::Ptr compileJs(const std::string& str);
    Script::Ptr compileJsEnclosed(const std::string& str);
    Script::Ptr compileJsModuleFile(const std::string& path);

    JsModule::Ptr loadJsModule(const std::string& path);
    JsModule::Ptr loadAsJsModule(ModuleFile* file);
    Element* runFile(const std::string& path);
    ElementsModule::Ptr compile(const std::string& path);

    Script::Ptr compileModuleFile(const std::string& path);
    Script::Ptr compileModuleSource(const std::string& path, const std::string& source);

    Object loadFile(const std::string& path);
    Object loadFile(const std::string& path, const std::string& content);
    Object loadJsFile(const std::string& path);

    ComponentTemplate* registerTemplate(const MetaObject* mo);
    v8::Local<v8::FunctionTemplate> pointTemplate();
    v8::Local<v8::FunctionTemplate> sizeTemplate();
    v8::Local<v8::FunctionTemplate> rectangleTemplate();
    v8::Local<v8::FunctionTemplate> importsTemplate();

    bool isElementConstructor(const Callable& c);

    void throwError(const lv::Exception* exception, Element* object);
    void tryCatch(const std::function<void()>& f, const std::function<void(const CatchData&)>& c);

    bool hasPendingException();
    void incrementTryCatchNesting();
    void decrementTryCatchNesting();
    void clearPendingException();

    const std::vector<std::string> &packageImportPaths() const;
    void setPackageImportPaths(const std::vector<std::string>& paths);

    void handleError(const std::string& message, const std::string& stack, const std::string& file, int line);

    ModuleFileType moduleFileType() const;
    void setModuleFileType(ModuleFileType type);

    const LanguageParser::Ptr& parser() const;

    const FileInterceptor* fileInterceptor() const;
    void setFileInterceptor(FileInterceptor* fileInterceptor);

private:
    void importInternals();
    void wrapScriptObject(Element* element);

    int tryCatchNesting();
    void setGlobalErrorHandler(bool value);


    EnginePrivate* m_d;
};

}} // namespace lv, el

#endif // LVENGINE_H

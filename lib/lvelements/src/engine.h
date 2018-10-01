#ifndef LVENGINE_H
#define LVENGINE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/module.h"
#include "live/elements/value.h"
#include "live/elements/script.h"

namespace lv{ namespace el{

class Context;
class ComponentTemplate;
class EnginePrivate;
class LV_ELEMENTS_EXPORT Engine{

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

private:
    static InitializeData* m_initializeData;

    // Engine
    // ------

public:
    Engine();
    ~Engine();

    Object require(Module::Ptr module);
    void scope(const std::function<void()> &f);

    v8::Isolate* isolate();

    Context* currentContext() const;
    Script::Ptr compile(const std::string& str);
    Script::Ptr compileEnclosed(const std::string& str);
    Script::Ptr compileJsFile(const std::string& path);

    ComponentTemplate* registerTemplate(const MetaObject* mo);
    v8::Local<v8::FunctionTemplate> pointTemplate();
    v8::Local<v8::FunctionTemplate> sizeTemplate();
    v8::Local<v8::FunctionTemplate> rectangleTemplate();

    bool isElementConstructor(const Callable& c);

private:
    void importInternals();
    void wrapScriptObject(Element* element);

    EnginePrivate* m_d;
    std::map<std::string, Module::Ptr> module;
};

}} // namespace lv, script

#endif // LVENGINE_H

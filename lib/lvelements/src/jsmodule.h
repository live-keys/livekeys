#ifndef JSMODULE_H
#define JSMODULE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"
#include "live/packagegraph.h"
#include "elementsmodule.h"

namespace lv{ namespace el{

class Engine;
class EnginePrivate;
class ModuleFile;
class JsModulePrivate;
class LV_ELEMENTS_EXPORT JsModule{

    DISABLE_COPY(JsModule);

public:
    friend class Engine;
    friend class EnginePrivate;

    typedef std::shared_ptr<JsModule> Ptr;


    enum Status{
        Initialized = 0,
        Evaluating,
        Ready
    };


public:
    ~JsModule();

    static JsModule::Ptr create(Engine* engine, const std::string& filePath, const v8::Local<v8::Module>& mod);

    v8::Local<v8::Module> localModule() const;
    v8::Local<v8::Value>  localModuleNamespace();

    v8::Local<v8::Value> evaluate(v8::Local<v8::Context> ctx);

    ScopedValue moduleNamespace();
    ScopedValue evaluate();

    const std::string& filePath() const;
    const std::string& name() const;
    Status status() const;

private:
    JsModule(Engine* engine, const std::string &filePath, const v8::Local<v8::Module>& mod);

    JsModulePrivate* d_ptr;
    std::string      m_file;
    std::string      m_name;
    ModuleFile*      m_moduleFile;
    Engine*          m_engine;
};

inline const std::string &JsModule::filePath() const{
    return m_file;
}

}}// namespace lv, el

#endif // JSMODULE_H

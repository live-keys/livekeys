#ifndef LVIMPORTS_H
#define LVIMPORTS_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/engine.h"
#include "element_p.h"

namespace lv{ namespace el{

class Imports{

    DISABLE_COPY(Imports);

public:
    Imports(Engine* engine, ModuleFile* mf);
    ~Imports();

    v8::Local<v8::Object> object();

    void require(const std::string& importKey);
    v8::Local<v8::Object> requireAs(const std::string& importKey);
    v8::Local<v8::Value> get(const std::string& key);

    static void get(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void require(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void requireAs(const v8::FunctionCallbackInfo<v8::Value>& info);

    static void getImport(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info);

    static v8::Local<v8::FunctionTemplate> functionTemplate(v8::Isolate* isolate);

private:
    Engine* m_engine;
    ModuleFile* m_moduleFile;
    v8::Persistent<v8::Object> m_object;
    std::map<std::string, ModuleFile*> m_exports;
    std::map<std::string, Imports*> m_imports;
};

}} // namespace lv, el

#endif // LVIMPORTS_H

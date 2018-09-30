#include "module.h"
#include "element_p.h"

namespace lv{ namespace el{

Module::Ptr Module::createLoader(const std::string &path, int versionMajor, int versionMinor){
    return std::shared_ptr<Module>(new Module(path, versionMajor, versionMinor));
}

LocalValue Module::createObject(Engine* engine, const std::string &path){
    v8::Local<v8::Object> obj = v8::Object::New(engine->isolate());
    v8::Local<v8::Object> exports = v8::Object::New(engine->isolate());

    v8::Local<v8::String> exportsKey = v8::String::NewFromUtf8(
        engine->isolate(), "exports", v8::String::kInternalizedString
    );
    v8::Local<v8::String> pathKey = v8::String::NewFromUtf8(
        engine->isolate(), "path", v8::String::kInternalizedString
    );
    v8::Local<v8::String> pathStr = v8::String::NewFromUtf8(
        engine->isolate(), path.c_str(), v8::String::kInternalizedString
    );


    obj->Set(exportsKey, exports);
    obj->Set(pathKey, pathStr);

    return LocalValue(obj);
}

Module::Module(const std::string &path, int versionMajor, int versionMinor)
    : m_path(path)
    , m_versionMajor(versionMajor)
    , m_versionMinor(versionMinor)
{

}

}} // namespace lv, el

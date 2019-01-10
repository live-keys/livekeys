#ifndef LVSCRIPT_H
#define LVSCRIPT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"
#include <memory>

namespace lv{ namespace el{

class Engine;
class ModuleFile;
class ScriptPrivate;

/**
 * @brief Holds a compiled script
 */
class LV_ELEMENTS_EXPORT Script{

    friend class Engine;

public:
    typedef std::shared_ptr<Script> Ptr;
    typedef std::shared_ptr<Script> ConstPtr;

    Value run();
    Object loadAsModule(ModuleFile* mf);


public:
    ~Script();

private:
    static const char* encloseStart;
    static const char* encloseEnd;

    static const char* moduleEncloseStart;
    static const char* moduleEncloseEnd;

private:
    DISABLE_COPY(Script);

    Object loadAsModuleImpl(ModuleFile* mf, const v8::Local<v8::Context>& context);

    Script(Engine* engine, const v8::Local<v8::Script>& value, const std::string& path = "");

    ScriptPrivate* m_d;
};

}} // namespace lv, el

#endif // LVSCRIPT_H

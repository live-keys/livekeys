#ifndef LVSCRIPT_H
#define LVSCRIPT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"
#include <memory>

namespace lv{ namespace el{

class Engine;

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
    Object loadAsModule();


public:
    ~Script();

private:
    static const std::string encloseStart;
    static const std::string encloseEnd;

    static const std::string moduleEncloseStart;
    static const std::string moduleEncloseEnd;

private:
    DISABLE_COPY(Script);

    Script(Engine* engine, const v8::Local<v8::Script>& value, const std::string& path = "");

    ScriptPrivate* m_d;
};

}} // namespace lv, el

#endif // LVSCRIPT_H

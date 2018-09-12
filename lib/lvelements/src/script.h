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

public:
    ~Script();

private:
    static const std::string encloseStart;
    static const std::string encloseEnd;

private:
    DISABLE_COPY(Script);

    Script(Engine* engine, const v8::Local<v8::Script>& value);

    ScriptPrivate* m_d;
};

}} // namespace lv, script

#endif // LVSCRIPT_H

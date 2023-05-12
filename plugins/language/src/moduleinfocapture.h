#ifndef LVMODULEINFOCAPTURE_H
#define LVMODULEINFOCAPTURE_H

#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/compiler/languageinfo.h"
#include "live/elements/languagescanner.h"
#include "languagescannerwrap.h"

namespace lv{ namespace el{

class ModuleInfoCapture : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(ModuleInfoCapture)
            .constructor<LanguageScannerWrap*>()
            .base<Element>()
            .scriptMethod("extract", &ModuleInfoCapture::extract)
            .scriptEvent("moduleReady", &ModuleInfoCapture::moduleReady)
        META_OBJECT_CLOSE
    }

public:
    ModuleInfoCapture(Engine* engine, LanguageScannerWrap* languageScanner);
    ~ModuleInfoCapture() override;

    ModuleInfo::Ptr extractModule(const std::string& uri);

public:
    Object extract(const std::string& uri);

public:
    Event moduleReady(ScopedValue value){
        static Event::Id eid = eventId(&ModuleInfoCapture::moduleReady);
        return notify(eid, value);
    }

private:
    void readModule();

    ModuleInfo::Ptr      m_current;
    std::list<Utf8>      m_dependencies;
    LanguageScannerWrap* m_languageScanner;
};

}} // namespace lv, el

#endif // MODULEINFOCAPTURE_H

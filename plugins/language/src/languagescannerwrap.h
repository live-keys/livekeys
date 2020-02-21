#ifndef LVLANGUAGESCANNERWRAP_H
#define LVLANGUAGESCANNERWRAP_H

#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/languagescanner.h"

namespace lv{ namespace el{

class LanguageScannerWrap : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(LanguageScannerWrap)
            .constructor()
            .name("LanguageScanner")
            .base<Element>()
            .scriptMethod("queueModule", &LanguageScannerWrap::queueModule)

        META_OBJECT_CLOSE
    }

public:
    explicit LanguageScannerWrap(Engine* engine);
    ~LanguageScannerWrap() override;

    void queueModule(const std::string& importUri);

    const LanguageScanner::Ptr& languageScanner() const;

//public:
//    Event moduleReady(Object o);

private:
    void moduleReady(ModuleInfo::ConstPtr mi);

    LanguageScanner::Ptr m_languageScanner;
};

inline const LanguageScanner::Ptr &LanguageScannerWrap::languageScanner() const{
    return m_languageScanner;
}

}}// namespace lv, el

#endif // LANGUAGESCANNERWRAP_H

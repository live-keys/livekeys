#include "languagescannerwrap.h"

namespace lv{ namespace el{

LanguageScannerWrap::LanguageScannerWrap(Engine* engine)
    : Element(engine)
{
    m_languageScanner = LanguageScanner::create(
        LanguageParser::createForElements(), engine->fileInterceptor()->fileInput()
    );
    m_languageScanner->setPackageImportPaths(engine->compiler()->packageImportPaths());
    m_languageScanner->onModuleReady([this](ModuleInfo::ConstPtr mi){
        moduleReady(mi);
    });
}

LanguageScannerWrap::~LanguageScannerWrap(){
}

void LanguageScannerWrap::queueModule(const std::string &importUri){
    try{
        m_languageScanner->queueModule(importUri);
    } catch ( Exception& e ){
        engine()->throwError(&e, this);
    }
}

void LanguageScannerWrap::moduleReady(ModuleInfo::ConstPtr){
}

}} // namespace lv, el

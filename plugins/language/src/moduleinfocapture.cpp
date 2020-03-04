#include "moduleinfocapture.h"
#include "live/utf8.h"
#include "live/elements/mlnodetojs.h"

namespace lv{ namespace el{

ModuleInfoCapture::ModuleInfoCapture(Engine *engine, LanguageScannerWrap *languageScanner)
    : Element(engine)
    , m_languageScanner(languageScanner)
{
    if ( languageScanner )
        languageScanner->ref();
}

ModuleInfoCapture::~ModuleInfoCapture(){
    m_languageScanner->unref();
}

ModuleInfo::Ptr ModuleInfoCapture::extractModule(const std::string &uri){
    if ( !m_languageScanner ){
        lv::Exception e = CREATE_EXCEPTION(Exception, "ModuleInfoCapture: Null LanguageScanner given.", Exception::toCode("NullReference"));
        engine()->throwError(&e, this);
        return nullptr;
    }

    m_current = m_languageScanner->languageScanner()->parseModule(uri);

    if ( m_current->totalDependencies() == 0 ){
        readModule();
        return m_current;
    }

    for ( size_t i = 0; i < m_current->totalDependencies(); ++i ){
        m_dependencies.push_back(m_current->dependencyAt(i));
    }

    //TODO: Register to languageScannerWrap event
    // When languageScannerWrap finishes, remove from dependency
    for ( size_t i = 0; i < m_current->totalDependencies(); ++i ){
        m_languageScanner->queueModule(m_current->dependencyAt(i).data());
    }

    return nullptr;
}

Object ModuleInfoCapture::extract(const std::string &uri){
    ModuleInfo::Ptr module = extractModule(uri);
    return Object::create(engine());
}

void ModuleInfoCapture::readModule(){
    ElementsPlugin::Ptr ep = engine()->require(m_current->importUri().data());
    Object epexports = ep->collectExportsObject();
    LocalObject lo(epexports);
    LocalValue lokeys = lo.ownProperties();
    LocalArray lokeysArray(engine(), lokeys);

    for ( int i = 0; i < lokeysArray.length(); ++i ){
        LocalValue exportValue = lo.get(lokeysArray.get(i));
        if ( exportValue.isElement() ){
            TypeInfo::Ptr ti = TypeInfo::extract(
                exportValue.toElement(engine())->typeMetaObject(), m_current->importUri(), true, false
            );
            m_current->addType(ti);

        } else if ( exportValue.isCallable() ){
            //TODO
        }
    }

    //TODO: resolve typeinfo through dependencies

    MLNode moduleInfoMl = m_current->toMLNode();

    LocalValue result(engine());
    ml::toJs(moduleInfoMl, result, engine());

    moduleReady(result);
}


}} // namespace lv, el

#include "languagemodule.h"
#include "live/elements/modulelibrary.h"

#include "languagescannerwrap.h"
#include "moduleinfocapture.h"

void LanguageModuleLoader::load(lv::el::ModuleLibrary *library){
    library->addType<lv::el::LanguageScannerWrap>();
    library->addType<lv::el::ModuleInfoCapture>();
}

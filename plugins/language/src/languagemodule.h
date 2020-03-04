#ifndef LVLANGUAGEMODULE_H
#define LVLANGUAGEMODULE_H

#include "live/visuallog.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/moduleloader.h"

class LanguageModuleLoader : public lv::el::ModuleLoader{

public:
    void load(lv::el::ModuleLibrary* library);
};

DECLARE_MODULE_LOADER(LanguageModuleLoader)

#endif // LVLANGUAGEMODULE_H

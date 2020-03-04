#ifndef LVBASEMODULE_H
#define LVBASEMODULE_H

#include "live/visuallog.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/moduleloader.h"

class BaseModuleLoader : public lv::el::ModuleLoader{

public:
    void load(lv::el::ModuleLibrary* library);
};

DECLARE_MODULE_LOADER(BaseModuleLoader)

#endif // LVFSMODULE_H

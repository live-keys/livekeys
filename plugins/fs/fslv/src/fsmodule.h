#ifndef LVFSMODULE_H
#define LVFSMODULE_H

#include "live/visuallog.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/moduleloader.h"

class FsModuleLoader : public lv::el::ModuleLoader{

public:
    void load(lv::el::ModuleLibrary* library);
};

DECLARE_MODULE_LOADER(FsModuleLoader)

#endif // LVFSMODULE_H

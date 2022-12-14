#include "fsmodule.h"
#include "dir.h"
#include "pathwrap.h"

#include "live/elements/modulelibrary.h"

void FsModuleLoader::load(lv::el::ModuleLibrary *library){
    library->addInstance("Dir",  new lv::el::Dir(library->engine()));
    //library->addInstance("Path", new lv::el::PathWrap(library->engine()));
}

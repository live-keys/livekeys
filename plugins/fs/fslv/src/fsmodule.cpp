#include "fsmodule.h"
#include "dir.h"
#include "path.h"

#include "live/elements/modulelibrary.h"

void FsModuleLoader::load(lv::el::ModuleLibrary *library){
    library->addInstance("Dir",  new lv::el::Dir(library->engine()));
    library->addInstance("Path", new lv::el::Path(library->engine()));
}

#ifndef LVMODULELOADER_H
#define LVMODULELOADER_H

#include "live/elements/lvelementsglobal.h"

namespace lv{ namespace el{

class ModuleLibrary;

class LV_ELEMENTS_EXPORT ModuleLoader{

public:
    typedef ModuleLoader*(*CreateLoaderFunction)();
    typedef void(*DestroyLoaderFunction)(ModuleLoader*);

    virtual ~ModuleLoader();

public:
    virtual void load(ModuleLibrary* library) = 0;
};

ModuleLoader::~ModuleLoader(){}

}} // namespace lv, el

#ifdef __cplusplus
#define DECLARE_MODULE_LOADER(_class) extern "C"{ \
    extern LV_EXPORT lv::el::ModuleLoader* createModuleLoader(){ return new _class; } \
    extern LV_EXPORT void destroyModuleLoader(_class* loader){ delete loader; } \
    }
#else
#define DECLARE_MODULE_LOADER(_class) \
    extern LV_EXPORT lv::ModuleLoader* createModuleLoader(){ return new _class; } \
    extern LV_EXPORT void destroyModuleLoader(_class* loader){ delete loader; }
#endif

#endif // LVMODULELOADER_H

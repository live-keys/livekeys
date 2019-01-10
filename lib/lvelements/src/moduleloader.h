#ifndef LVMODULELOADER_H
#define LVMODULELOADER_H

#include "live/elements/lvelementsglobal.h"

namespace lv{ namespace el{

class ModuleLibrary;

class ModuleLoader{

public:
    typedef ModuleLoader*(*CreateLoaderFunction)();
    typedef void(*DestroyLoaderFunction)(ModuleLoader*);

public:
    virtual void load(ModuleLibrary* library) = 0;
};

}} // namespace lv, el

#ifdef __cplusplus
#define DECLARE_MODULE_LOADER(_class) extern "C"{ \
    extern LV_EXPORT ModuleLoader* createLoader(){ return new _class; } \
    extern LV_EXPORT void destroyLoader(_class* loader){ delete loader; } \
    }
#else
#define DECLARE_MODULE_LOADER(_class) \
    extern LV_EXPORT ModuleLoader* createLoader(){ return new _class; } \
    extern LV_EXPORT void destroyLoader(_class* loader){ delete loader; }
#endif

#endif // LVMODULELOADER_H

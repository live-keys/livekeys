/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

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

inline ModuleLoader::~ModuleLoader(){}

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

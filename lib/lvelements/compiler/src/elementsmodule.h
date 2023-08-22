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

#ifndef LVELEMENTSPLUGIN_H
#define LVELEMENTSPLUGIN_H

#include "live/elements/compiler/lvelcompilerglobal.h"
#include "live/elements/compiler/compiler.h"
#include "live/module.h"

#include <memory>

namespace lv{ namespace el{

class Engine;
class ModuleFile;
class ModuleLibrary;

class ElementsModulePrivate;
class LV_ELEMENTS_COMPILER_EXPORT ElementsModule{

    DISABLE_COPY(ElementsModule);

public:
    typedef std::shared_ptr<ElementsModule> Ptr;

    class Export{
    public:
        enum Type{
            Component,
            Element
        };

        Export() : file(nullptr), library(nullptr){}

        bool isValid(){ return !name.empty(); }

    public:
        std::string    name;
        Type           type;
        ModuleFile*    file;
        ModuleLibrary* library;
    };

public:
    ~ElementsModule();

    static ElementsModule::Ptr create(Module::Ptr module, Compiler::Ptr compiler, Engine* engine);
    static ElementsModule::Ptr create(Module::Ptr module, Compiler::Ptr compiler);

    static ModuleFile *addModuleFile(ElementsModule::Ptr& epl, const std::string& name);

    ModuleFile* findModuleFileByName(const std::string& name) const;
    ModuleFile* moduleFileBypath(const std::string& path) const;

    const Module::Ptr &module() const;

    void compile();

    Compiler::Ptr compiler() const;
    Engine* engine() const;

    Export findExport(const std::string& name) const;

    const std::map<std::string, ModuleFile*>& fileExports() const;
    const std::list<ModuleLibrary*>& libraryModules() const;

private:
    void initializeLibraries(const std::list<std::string>& libs);

    static ElementsModule::Ptr createImpl(Module::Ptr module, Compiler::Ptr compiler, Engine* engine);
    ElementsModule(Module::Ptr module, Compiler::Ptr compiler, Engine* engine);

    ElementsModulePrivate* m_d;

};

}} // namespace lv, el

#endif // LVELEMENTSPLUGIN_H

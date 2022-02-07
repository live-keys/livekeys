#ifndef LVELEMENTSPLUGIN_H
#define LVELEMENTSPLUGIN_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/compiler/compiler.h"
#include "live/module.h"

#include <memory>

namespace lv{ namespace el{

class Engine;
class ModuleFile;
class ModuleLibrary;

class ElementsModulePrivate;
class LV_ELEMENTS_EXPORT ElementsModule{

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

    static ElementsModule::Ptr create(Module::Ptr module, Engine* engine);
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

    static ElementsModule::Ptr create(Module::Ptr module, Compiler::Ptr compiler, Engine* engine);
    ElementsModule(Module::Ptr module, Compiler::Ptr compiler, Engine* engine);

    ElementsModulePrivate* m_d;

};

}} // namespace lv, el

#endif // LVELEMENTSPLUGIN_H

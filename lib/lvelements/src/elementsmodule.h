#ifndef LVELEMENTSPLUGIN_H
#define LVELEMENTSPLUGIN_H

#include "live/elements/lvelementsglobal.h"
#include "live/plugin.h"

#include <memory>

namespace lv{ namespace el{

class Engine;
class ModuleFile;
class ModuleLibrary;
class Object;

class ElementsPluginPrivate;
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

    static ElementsModule::Ptr create(Plugin::Ptr plugin, Engine* engine);

    static ModuleFile *addModuleFile(ElementsModule::Ptr& epl, const std::string& name);
    void addModuleLibrary(ModuleLibrary* library);

    ModuleFile* findModuleFileByName(const std::string& name) const;
    ModuleFile* moduleFileBypath(const std::string& path) const;

    const Plugin::Ptr &plugin() const;

    Object collectExportsObject();
    const Object& libraryExports() const;

    void compile();

    Engine* engine() const;

    Export findExport(const std::string& name) const;

    const std::map<std::string, ModuleFile*>& fileExports() const;
    const std::list<ModuleLibrary*>& libraryModules() const;

private:
    ElementsModule(Plugin::Ptr plugin, Engine* engine);

    ElementsPluginPrivate* m_d;

};

}} // namespace lv, el

#endif // LVELEMENTSPLUGIN_H

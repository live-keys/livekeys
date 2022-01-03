#ifndef LVMODULEFILE_H
#define LVMODULEFILE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/jsmodule.h"
#include "live/packagegraph.h"
#include "elementsmodule.h"

#include <memory>
#include <list>

namespace lv{ namespace el{

class ProgramNode;
class Imports;
class ModuleFilePrivate;
class ModuleFile{

    DISABLE_COPY(ModuleFile);

public:
    friend class ElementsModule;
    friend class Engine;

    enum State{
        Initiaized = 0,
        Parsed,
        Loading,
        Ready
    };

    class Export{
    public:
        enum Type{
            Component,
            Element
        };
    public:
        std::string name;
        Type        type;
    };

    class Import{
    public:
        Import() : isRelative(false){}

    public:
        std::string uri;
        std::string as;
        bool isRelative;
        ElementsModule::Ptr module;
    };

public:
    ~ModuleFile();

    void resolveTypes();
    void compile();

    State state() const;
    const std::string& name() const;
    std::string fileName() const;
    std::string jsFileName() const;
    std::string jsFilePath() const;
    std::string filePath() const;
    const ElementsModule::Ptr& plugin() const;
    const std::list<Export>& exports() const;
    const std::list<Import>& imports() const;
    void resolveImport(const std::string& uri, ElementsModule::Ptr epl);

private:
    void addDependency(ModuleFile* to);
    void setJsModule(const JsModule::Ptr& jsModule);

    void initializeImportsExports(Engine* engine);
    bool hasDependency(ModuleFile* module, ModuleFile* dependency);
    static PackageGraph::CyclesResult<ModuleFile*> checkCycles(ModuleFile* mf);
    static PackageGraph::CyclesResult<ModuleFile*> checkCycles(ModuleFile* mf, ModuleFile* current, std::list<ModuleFile*> path);


    ModuleFile(ElementsModule::Ptr plugin, const std::string& name, const std::string& content, ProgramNode* node);

    ModuleFilePrivate* m_d;

};

}}// namespace lv, el

#endif // LVMODULEFILE_H

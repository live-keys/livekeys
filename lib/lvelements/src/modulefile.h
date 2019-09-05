#ifndef LVMODULEFILE_H
#define LVMODULEFILE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"
#include "live/packagegraph.h"
#include "elementsplugin.h"

#include <memory>
#include <list>

namespace lv{ namespace el{

class Imports;
class ModuleFilePrivate;
class ModuleFile{

    DISABLE_COPY(ModuleFile);

public:
    friend class ElementsPlugin;

    enum State{
        Initiaized = 0,
        Parsed,
        Loading,
        Ready
    };

public:
    ~ModuleFile();

    LocalValue get(Engine* engine, ModuleFile* from, const std::string& name);
    void parse(Engine *engine = nullptr);

    State state() const;
    const std::string& name() const;
    std::string fileName() const;
    std::string filePath() const;
    const ElementsPlugin::Ptr& plugin() const;
    const std::list<std::string>& exports() const;
    Imports* imports();
    LocalValue createObject(Engine *engine) const;

private:
    void initializeImportsExports(Engine* engine);
    bool hasDependency(ModuleFile* module, ModuleFile* dependency);
    PackageGraph::CyclesResult<ModuleFile*> checkCycles(ModuleFile* mf);
    PackageGraph::CyclesResult<ModuleFile*> checkCycles(ModuleFile* mf, ModuleFile* current, std::list<ModuleFile*> path);

    ModuleFile(const ElementsPlugin::Ptr& plugin, const std::string &name);

    ModuleFilePrivate* m_d;

};

}}// namespace lv, el

#endif // LVMODULEFILE_H

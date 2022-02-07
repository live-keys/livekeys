#ifndef LVMODULE_H
#define LVMODULE_H

#include "live/lvbaseglobal.h"

#include <string>
#include <memory>
#include <map>
#include <list>

namespace lv{

class MLNode;
class PackageGraph;

class ModulePrivate;
class LV_BASE_EXPORT Module{

    DISABLE_COPY(Module);

public:
    /** Shared pointer to plugin */
    typedef std::shared_ptr<Module> Ptr;

    class Context;

public:
    /** Default name for a plugin file */
    static const char* fileName;

public:
    ~Module();

    static bool existsIn(const std::string& path);
    static Module::Ptr createFromPath(const std::string& path);
    static Module::Ptr createFromNode(const std::string &path, const std::string& filePath, const MLNode& m);
    static Module::Ptr createEmpty(const std::string& name);

    const std::string& name() const;
    const std::string& path() const;
    const std::string& filePath() const;

    std::string packagePath() const;
    std::string pathFromPackage() const;

    const std::string& package() const;
    const std::list<std::string>& dependencies() const;
    const std::list<std::string>& fileModules() const;
    const std::list<std::string>& libraryModules() const;
    const std::list<std::string>& assets() const;
    const std::list<std::pair<std::string, std::string> >& palettes() const;

    void assignContext(PackageGraph* graph);
    Context* context();

private:
    Module(const std::string& path, const std::string& filePath, const std::string& name, const std::string& package);

    void addPalette(const std::string& type, const std::string& path);

    ModulePrivate* m_d;
};

} // namespace

#endif // LVMODULE_H

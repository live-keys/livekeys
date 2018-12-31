#ifndef LVPACKAGEGRAPH_H
#define LVPACKAGEGRAPH_H

#include "live/lvbaseglobal.h"
#include "live/package.h"
#include "live/plugin.h"

#include <vector>
#include <list>
#include <string>

namespace lv{

class PackageGraphPrivate;

class LV_BASE_EXPORT PackageGraph{

    DISABLE_COPY(PackageGraph);

public:
    class LibraryNode{
    public:
        LibraryNode(const Package::Library& l) : library(l){}

        Package::Library library;
        bool loaded;
    };

    template<typename T>
    class CyclesResult{

    public:
        enum Type{
            NotFound,
            Found
        };

        CyclesResult(Type t) : m_type(t){}
        CyclesResult(Type t, const std::list<T>& path) : m_type(t), m_path(path){}

        bool found(){ return m_type == CyclesResult::Found; }
        const std::list<T>& path() const{ return m_path; }

    private:
        Type m_type;
        std::list<T> m_path;
    };

public:
    PackageGraph();
    virtual ~PackageGraph();

    void loadPackage(const Package::Ptr& p, bool addLibraries = true);
    void loadPackageWithDependencies(const Package::Ptr& p, std::list<Package::Reference>& missing, bool addLibraries = true);
    void addDependency(const Package::Ptr& package, const Package::Ptr& dependsOn);
    CyclesResult<Package::Ptr> checkCycles(const Package::Ptr& p);
    CyclesResult<Plugin::Ptr> checkCycles(const Plugin::Ptr& p);

    void clearPackages();
    void clearLibraries();

    void addLibrary(const Package::Library& lib);
    void loadLibraries();

    std::string toString() const;

    Package::Ptr findPackage(Package::Reference ref);
    Package::Ptr findPackage(const std::string& packageName);
    Package::Ptr package(const std::string& name);
    const std::vector<std::string>& packageImportPaths() const;
    void setPackageImportPaths(const std::vector<std::string>& paths);

    static void addInternalPackage(const Package::Ptr& package);
    static std::map<std::string, Package::Ptr>& internals();

    static void assignInternalContext(PackageGraph *graph);
    static PackageGraph*& internalsContextOwner();

    Plugin::Ptr loadPlugin(const std::string& importSegment);
    Plugin::Ptr loadPlugin(const std::vector<std::string>& importSegment);
    void addDependency(const Plugin::Ptr& plugin, const std::string& pluginDependency);
    void addDependency(const Plugin::Ptr& plugin, const Plugin::Ptr& dependsOn);

private:
    bool hasDependency(const Package::Ptr& package, const Package::Ptr& dependency);
    bool hasDependency(const Plugin::Ptr& plugin, const Plugin::Ptr& dependency);

    PackageGraph::CyclesResult<Package::Ptr> checkCycles(const Package::Ptr &p, Package::Ptr current, std::list<Package::Ptr> path);
    PackageGraph::CyclesResult<Plugin::Ptr> checkCycles(const Plugin::Ptr &p, Plugin::Ptr current, std::list<Plugin::Ptr> path);

    std::string toString(Package::Ptr package, const std::string& indent) const;
    std::string toStringRecurse(Package::Ptr package, const std::string& indent) const;

    PackageGraphPrivate* m_d;
};

}// namespace

#endif // LVPACKAGEGRAPH_H

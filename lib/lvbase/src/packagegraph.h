#ifndef LVPACKAGEGRAPH_H
#define LVPACKAGEGRAPH_H

#include "live/lvbaseglobal.h"
#include "live/package.h"
#include "live/plugin.h"

namespace lv{

class PackageGraphPrivate;

class LV_BASE_EXPORT PackageGraph{

    DISABLE_COPY(PackageGraph);

public:
    class Node{
    public:
        Package::Ptr package;

        std::list<PackageGraph::Node*> dependents; // parents
        std::list<PackageGraph::Node*> dependencies; // children
    };

    class LibraryNode{
    public:
        LibraryNode(const Package::Library& l) : library(l){}

        Package::Library library;
        bool loaded;
    };

    class CyclesResult{

    public:
        enum Type{
            NotFound,
            Found
        };

        CyclesResult(Type t) : m_type(t){}
        CyclesResult(Type t, const std::list<PackageGraph::Node*>& path) : m_type(t), m_path(path){}

        bool found(){ return m_type == CyclesResult::Found; }
        const std::list<PackageGraph::Node*> path() const{ return m_path; }

    private:
        Type m_type;
        std::list<PackageGraph::Node*> m_path;
    };

public:
    PackageGraph();
    virtual ~PackageGraph();

    bool hasPackage(const Package::Ptr& p);
    void loadPackage(const Package::Ptr& p, bool addLibraries = true);
    void loadPackageWithDependencies(const Package::Ptr& p, std::list<Package::Reference>& missing, bool addLibraries = true);
    void addDependency(const Package::Ptr& package, const Package::Ptr& dependsOn);
    CyclesResult checkCycles(const Package::Ptr& p);

    void clearPackages();
    void clearLibraries();

    void addLibrary(const Package::Library& lib);
    void loadLibraries();

    std::string toString() const;

    Package::Ptr findPackage(Package::Reference ref);
    Package::Ptr package(const std::string& name);
    const std::vector<std::string>& packageImportPaths() const;
    void setPackageImportPaths(const std::vector<std::string>& paths);

    static void addInternalPackage(const Package::Ptr& package);
    static std::map<std::string, PackageGraph::Node*>& internals();

    Plugin::Ptr loadPlugin(const std::string& importSegment);

private:
    bool hasDependency(PackageGraph::Node* node, PackageGraph::Node* dependency);

    CyclesResult checkCycles(const Package::Ptr &p, PackageGraph::Node* n, std::list<PackageGraph::Node*> path);

    std::string toString(PackageGraph::Node* node, const std::string& indent) const;
    std::string toStringRecurse(PackageGraph::Node* node, const std::string& indent) const;

    PackageGraphPrivate* m_d;
};

}// namespace

#endif // LVPACKAGEGRAPH_H

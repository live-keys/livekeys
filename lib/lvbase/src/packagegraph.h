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

#ifndef LVPACKAGEGRAPH_H
#define LVPACKAGEGRAPH_H

#include "live/lvbaseglobal.h"
#include "live/package.h"
#include "live/module.h"

#include <vector>
#include <list>
#include <string>

namespace lv{

class PaletteContainer;
class PackageGraphPrivate;

class LV_BASE_EXPORT PackageGraph{

    DISABLE_COPY(PackageGraph);

public:
    /**
     * \class lv::PackageGraph::LibraryNode
     * \brief Struct to represent a library node
     *
     * \ingroup lvbase
     */
    class LibraryNode{
    public:
        /** Default constructor */
        LibraryNode(const Package::Library& l) : library(l){}

        /** Library within the node*/
        Package::Library library;
        /** Shows if it's loaded */
        bool loaded;
    };

    /// \private
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
    /** Adds the dependency needed for this module. If not within the same package, we load an entire package instead. */
    void addDependency(const Package::Ptr& package, const Package::Ptr& dependsOn);
    CyclesResult<Package::Ptr> checkCycles(const Package::Ptr& p);
    CyclesResult<Module::Ptr> checkCycles(const Module::Ptr& p);

    void clearPackages();
    void clearLibraries();

    void addLibrary(const Package::Library& lib);
    void loadLibraries();

    std::string toString() const;

    Package::Ptr findPackage(Package::Reference ref) const;
    Package::Ptr findPackage(const std::string& packageName) const;

    Package::Ptr findLoadedPackage(const std::string& name);
    Package::ConstPtr findLoadedPackage(const std::string& name) const;

    Package::Ptr package(const std::string& name);
    Package::ConstPtr package(const std::string& name) const;
    const std::vector<std::string>& packageImportPaths() const;
    void setPackageImportPaths(const std::vector<std::string>& paths);

    static void addInternalPackage(const Package::Ptr& package);
    static std::map<std::string, Package::Ptr>& internals();

    static void assignInternalContext(PackageGraph *graph);
    static PackageGraph*& internalsContextOwner();

    Module::Ptr createRunningModule(const std::string& path);
    void loadRunningPackageAndModule(const Package::Ptr& package, const Module::Ptr& plugin);

    Module::Ptr loadModule(const std::string& importSegment, Module::Ptr requestingPlugin = nullptr);
    Module::Ptr loadModule(const std::vector<std::string>& importSegment, Module::Ptr requestingPlugin = nullptr);
    void addDependency(const Module::Ptr& plugin, const std::string& pluginDependency);
    void addDependency(const Module::Ptr& plugin, const Module::Ptr& dependsOn);

    PaletteContainer* paletteContainer() const;

private:
    bool hasDependency(const Package::Ptr& package, const Package::Ptr& dependency);
    bool hasDependency(const Module::Ptr& plugin, const Module::Ptr& dependency);

    PackageGraph::CyclesResult<Package::Ptr> checkCycles(const Package::Ptr &p, Package::Ptr current, std::list<Package::Ptr> path);
    PackageGraph::CyclesResult<Module::Ptr> checkCycles(const Module::Ptr &p, Module::Ptr current, std::list<Module::Ptr> path);

    std::string toString(Package::Ptr package, const std::string& indent) const;
    std::string toStringRecurse(Package::Ptr package, const std::string& indent) const;

    PackageGraphPrivate* m_d;
};

}// namespace

#endif // LVPACKAGEGRAPH_H

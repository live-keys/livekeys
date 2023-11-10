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

#include "packagegraph.h"
#include "live/packagecontext.h"
#include "live/modulecontext.h"
#include "live/exception.h"
#include "live/applicationcontext.h"
#include "live/palettecontainer.h"
#include "live/libraryloadpath.h"
#include "live/visuallog.h"

#include <list>
#include <iostream>
#include <sstream>

namespace lv{

namespace{

    std::vector<std::string> splitString(const std::string &text, char sep) {
        std::vector<std::string> tokens;
        size_t start = 0, end = 0;
        while ((end = text.find(sep, start)) != std::string::npos) {
            tokens.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(text.substr(start));
        return tokens;
    }
}

/// \private
class PackageGraphPrivate{

public:
    std::vector<std::string> packageImportPaths;
    std::map<std::string, Package::Ptr> packages;
    std::map<std::string, PackageGraph::LibraryNode*> libraries;
    PaletteContainer* paletteContainer;
};

/**
 * \class lv::PackageGraph
 * \brief Structure to represent packages and their dependencies
 *
 * It also stores all the libraries, stores in the LibraryNode structure.
 * We also check for dependency cycles on multiple levels: modules, packages, Elements files
 * \ingroup lvbase
 */


/** Default constructor */
PackageGraph::PackageGraph()
    : m_d(new PackageGraphPrivate)
{
    m_d->paletteContainer = new PaletteContainer();
}

/** Default destructor */
PackageGraph::~PackageGraph(){
    clearPackages();
    clearLibraries();
    delete m_d->paletteContainer;
    delete m_d;
}

/** Loads package in the graph and makes necessary checks */
void PackageGraph::loadPackage(const Package::Ptr &p, bool addLibraries){
    auto it = m_d->packages.find(p->name());
    if ( it == m_d->packages.end() ){
        p->assignContext(this);

        if ( addLibraries ){
            for ( auto libIt = p->libraries().begin(); libIt != p->libraries().end(); ++libIt ){
                Package::Library* lib = libIt->second;
                addLibrary(*lib);
            }
        }

        m_d->packages[p->name()] = p;

        vlog("lvbase-packagegraph").v() << "Loaded package \'" + p->name() << "\' [" + p->version().toString() + "]";

    } else {
        Package::Ptr existingPackage = it->second;
        Version oldVersion = existingPackage->version();
        Version newVersion = p->version();

        if ( newVersion.majorNumber() != oldVersion.majorNumber() ){
            THROW_EXCEPTION(
                lv::Exception,
                "Incompatible package versions for package \'" + p->name() + "\': " +
                oldVersion.toString() + " vs " + newVersion.toString(), 1);
        }

        if ( newVersion > oldVersion ){
            vlog("lvbase-packagegraph").v() <<
                "Replaced package \'" << p->name() << "\' from [" << existingPackage->version().toString() << "] to [" <<
                p->version().toString() << "]";

            p->assignContext(this);
            m_d->packages[p->name()] = p;

            if ( addLibraries ){
                for ( auto it = p->libraries().begin(); it != p->libraries().end(); ++it ){
                    Package::Library* lib = it->second;
                    addLibrary(*lib);
                }
            }
        }
    }
}

/** Recursive loader that also loads the necessarys dependencies */
void PackageGraph::loadPackageWithDependencies(
    const Package::Ptr &package, std::list<Package::Reference> &missing, bool addLibraries)
{
    loadPackage(package, addLibraries);

    for ( auto it = package->dependencies().begin(); it != package->dependencies().end(); ++it ){
        Package::Reference* ref = it->second;
        Package::Ptr p = findPackage(*ref);
        if ( p != nullptr ){
            loadPackageWithDependencies(p, missing, addLibraries);
        } else {
            missing.push_back(*ref);
        }
    }
}

/** */
void PackageGraph::addDependency(const Package::Ptr &package, const Package::Ptr &dependsOn){
    auto packageit = m_d->packages.find(package->name());
    if ( packageit == m_d->packages.end() && package->name() != "." )
        THROW_EXCEPTION(lv::Exception, "Failed to find package:" + package->name(), 3);
    if ( package->contextOwner() != this )
        THROW_EXCEPTION(lv::Exception, "Package \'" + package->name() +"\' is not part of the current package graph.", 3);
    if ( dependsOn->contextOwner() != this )
        THROW_EXCEPTION(lv::Exception, "Package \'" + dependsOn->name() +"\' is not part of the current package graph.", 3);

    auto dependsOnit = m_d->packages.find(dependsOn->name());
    if ( dependsOnit == m_d->packages.end() ){
        auto internalsIt = internals().find(dependsOn->name());
        if ( internalsIt == internals().end() ){
            THROW_EXCEPTION(lv::Exception, "Failed to find package: " + dependsOn->name(), 2);
        } else {
            if ( !hasDependency(package, dependsOn) )
                package->context()->dependencies.push_back(dependsOn);
            return;
        }
    }
    if ( !hasDependency(package, dependsOn ) ){
        package->context()->dependencies.push_back(dependsOn);
        dependsOn->context()->dependents.push_back(package);

        PackageGraph::CyclesResult<Package::Ptr> cr = checkCycles(package);
        if ( cr.found() ){
            std::stringstream ss;

            for ( auto it = cr.path().begin(); it != cr.path().end(); ++it ){
                Package::Ptr n = *it;

                if ( it != cr.path().begin() ){
                    ss << " -> ";
                }

                ss << n->name() << "[" << n->version().toString() << "]";
            }

            for ( auto it = package->context()->dependencies.begin(); it != package->context()->dependencies.end(); ++it ){
                if ( *it == dependsOn ){
                    package->context()->dependencies.erase(it);
                    break;
                }
            }
            for ( auto it = dependsOn->context()->dependents.begin(); it != dependsOn->context()->dependents.end(); ++it ){
                if ( *it == package ){
                    dependsOn->context()->dependents.erase(it);
                    break;
                }
            }

            THROW_EXCEPTION(lv::Exception, "Package dependency cycle found: "  + ss.str(), lv::Exception::toCode("Cycle"));
        }
    }

}

/** Check if there are cycles between packages, starting from the given packages */
PackageGraph::CyclesResult<Package::Ptr> PackageGraph::checkCycles(const Package::Ptr &p){
    auto it = m_d->packages.find(p->name());
    if ( it == m_d->packages.end() && p->name() != ".")
        THROW_EXCEPTION(lv::Exception, "Failed to find package for cycles: " + p->name(), 2);

    std::list<Package::Ptr> path;
    path.push_back(p);

    for ( auto it = p->context()->dependencies.begin(); it != p->context()->dependencies.end(); ++it ){
        PackageGraph::CyclesResult<Package::Ptr> cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<Package::Ptr>(PackageGraph::CyclesResult<Package::Ptr>::NotFound);
}

/** Check if there are cycles between modules, starting from the given module */
PackageGraph::CyclesResult<Module::Ptr> PackageGraph::checkCycles(const Module::Ptr &p){
    if ( p->context() == nullptr || p->context()->packageGraph != this )
        THROW_EXCEPTION(lv::Exception, "Failed to find loaded module for cycles: " + p->name(), 2);

    std::list<Module::Ptr> path;
    path.push_back(p);

    for ( auto it = p->context()->localDependencies.begin(); it != p->context()->localDependencies.end(); ++it ){
        PackageGraph::CyclesResult<Module::Ptr> cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<Module::Ptr>(PackageGraph::CyclesResult<Module::Ptr>::NotFound);
}

PackageGraph::CyclesResult<Package::Ptr> PackageGraph::checkCycles(
        const Package::Ptr &p, Package::Ptr current, std::list<Package::Ptr> path)
{
    path.push_back(current);
    if ( current.get() == p.get() )
        return PackageGraph::CyclesResult<Package::Ptr>(PackageGraph::CyclesResult<Package::Ptr>::Found, path);

    for ( auto it = current->context()->dependencies.begin(); it != current->context()->dependencies.end(); ++it ){
        PackageGraph::CyclesResult<Package::Ptr> cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<Package::Ptr>(PackageGraph::CyclesResult<Package::Ptr>::NotFound);
}

PackageGraph::CyclesResult<Module::Ptr> PackageGraph::checkCycles(
        const Module::Ptr &p, Module::Ptr current, std::list<Module::Ptr> path)
{
    path.push_back(current);
    if ( current.get() == p.get() )
        return PackageGraph::CyclesResult<Module::Ptr>(PackageGraph::CyclesResult<Module::Ptr>::Found, path);

    for ( auto it = current->context()->localDependencies.begin(); it != current->context()->localDependencies.end(); ++it ){
        PackageGraph::CyclesResult<Module::Ptr> cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<Module::Ptr>(PackageGraph::CyclesResult<Module::Ptr>::NotFound);
}

/**
  \brief Adds a library for loading.

  The library is loaded if:


  \li the library hasn't been loaded yet
  \li the library currently provided is a newer version than the existing one, or has more compilation flags
  that include the existing one and the older one hasn't been cached (loaded into memory) yet

  A few conditions must be respected, otherwise an exception will be triggered:

 \li If another library already has been loaded, then the major version must be the same as the one that was loaded
 \li If another library already has been loaded, then the added library must have either more or less compilation flags
 than the one added
 \li If it has less compilation flags, than the minor version must be smaller or equal to the the added library
 \li If it has more compliation flags, than the minor version must be higher or equal to the added library
 */
void PackageGraph::addLibrary(const Package::Library &lib){
    auto it = m_d->libraries.find(lib.name.data());
    if ( it == m_d->libraries.end() ){
        PackageGraph::LibraryNode* libnode = new PackageGraph::LibraryNode(lib);
        libnode->loaded  = false;
        m_d->libraries[lib.name.data()] = libnode;

        vlog("lvbase-packagegraph").d() << "Added library \'" << lib.name.data() << "\' [" + lib.version.toString() << "]";

    } else {
        PackageGraph::LibraryNode* libnode = it->second;
        Version oldVersion = libnode->library.version;
        Version newVersion = lib.version;

        if ( newVersion.majorNumber() != oldVersion.majorNumber() ){
            THROW_EXCEPTION(
                lv::Exception,
                "Incompatible library versions for library \'" + lib.name.data() + "\': " +
                oldVersion.toString() + " vs " + newVersion.toString(), 6);
        }

        if ( newVersion > oldVersion && libnode->loaded ){
            THROW_EXCEPTION(
                lv::Exception,
                "New library version provided for library \'" + lib.name.data() + "\', however older library has already " +
                "been loaded: " + oldVersion.toString() + " vs " + newVersion.toString(), 7);
        }

        Package::Library::FlagResult flags = libnode->library.compareFlags(lib);
        if ( flags == Package::Library::Equal ){
            if ( newVersion > oldVersion ){
                libnode->library = lib;
            }
        } else if ( flags == Package::Library::Different ){
            THROW_EXCEPTION(
                lv::Exception,
                "Incompatible library compilation for library \'"  + lib.name.data() + "\'",
                8
            );
        } else if ( flags == Package::Library::HasLess ){ // current library has less
            if ( newVersion.minorNumber() < oldVersion.minorNumber() ){
                THROW_EXCEPTION(
                    lv::Exception,
                    "Newer library \'" + lib.name.data() + "\' [" + newVersion.toString() + "] has incompatible compilation " +
                    "flags than older library \'" + oldVersion.toString() + "\', ", 10);
            }

            vlog("lvbase-packagegraph").d() <<
                "Replaced library \'" << lib.name.data() << "\' from [" << libnode->library.version.toString() << "] to [" <<
                lib.version.toString() << "]";

            libnode->library = lib;
        } else if ( flags == Package::Library::HasMore ){ // current library has more, do not replace
            if ( newVersion.minorNumber() > oldVersion.minorNumber() ){
                THROW_EXCEPTION(
                    lv::Exception,
                    "Loaded library \'" + lib.name.data() + "\' [" + oldVersion.toString() + "] has incompatible compilation " +
                    "flags than newer library \'" + newVersion.toString() + "\', ", 10);
            }
        }
    }

}

/** Function that loads all the libraries from our internal libraries structure */
void PackageGraph::loadLibraries(){
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it ){
        PackageGraph::LibraryNode* libnode = it->second;
        if ( !libnode->loaded ){
            LibraryLoadPath::addRecursive(libnode->library.path.data(), ApplicationContext::instance().linkPath());
            libnode->loaded = true;
        }
    }
}

/**
 * \brief Clears all packages from the graph
 */
void PackageGraph::clearPackages(){
    m_d->packages.clear();
}

/**
 * \brief Clears all libraries from the graph
 */
void PackageGraph::clearLibraries(){
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it ){
        delete it->second;
    }
    m_d->libraries.clear();
}

/**
 * \brief Nice string representation of the PackageGraph
 */
std::string PackageGraph::toString() const{
    const PackageGraphPrivate* d = m_d;

    std::stringstream ss;

    ss << "Internals:" << std::endl;
    for ( auto it = internals().begin(); it != internals().end(); ++it ){
        ss << "  " << it->second->name() << "[" << it->second->version().toString() << "]";
    }
    ss << std::endl;

    ss << "Packages:" << std::endl;
    for ( auto it = d->packages.begin(); it != d->packages.end(); ++it ){
        ss << toStringRecurse(it->second, "  ");
    }

    ss << std::endl;

    ss << "Libraries:" << std::endl;
    for ( auto it = d->libraries.begin(); it != d->libraries.end(); ++it ){
        PackageGraph::LibraryNode* libnode = it->second;
        ss << "  " << libnode->library.name.data() << "[" << libnode->library.version.toString() << "]:" << (libnode->loaded ? "loaded" : "not loaded") << std::endl;
    }

    ss << std::endl;

    return ss.str();
}

/**
 * \brief Finds the package according to the given reference
 */
Package::Ptr PackageGraph::findPackage(Package::Reference ref) const{
    std::vector<std::string> paths = packageImportPaths();
    for ( auto it = paths.begin(); it != paths.end(); ++it ){
        std::string path = *it + "/" + ref.name.data();
        if ( Package::existsIn(path) ){
            Package::Ptr p = Package::createFromPath(path);
            if ( p->version().majorNumber() == ref.version.majorNumber() && p->version() > ref.version ){
                return p;
            }
        }

        std::string pathMajor = path + "." + std::to_string(ref.version.majorNumber());
        if ( Package::existsIn(pathMajor) ){
            Package::Ptr p = Package::createFromPath(path);
            if ( p->version().majorNumber() == ref.version.majorNumber() && p->version() > ref.version ){
                return p;
            }
        }

        std::string pathMajorMinor = pathMajor + "." + std::to_string(ref.version.minorNumber());
        if ( Package::existsIn(pathMajorMinor) ){
            Package::Ptr p = Package::createFromPath(path);
            if ( p->version().majorNumber() == ref.version.majorNumber() && p->version() > ref.version ){
                return p;
            }
        }
    }

    return Package::Ptr(nullptr);
}

/**
 * \brief Finds the package with the given name and the highest version in the package import paths
 */
Package::Ptr PackageGraph::findPackage(const std::string &packageName) const{
    Package::Ptr foundPackage(nullptr);

    std::vector<std::string> paths = packageImportPaths();
    for ( auto it = paths.begin(); it != paths.end(); ++it ){
        std::string path = *it + "/" + packageName;
        if ( Package::existsIn(path) ){
            Package::Ptr p = Package::createFromPath(path);

            if ( foundPackage == nullptr ){
                foundPackage = p;
            } else if ( p->version() > foundPackage->version() ){
                foundPackage = p;
            }
        }
    }

    return foundPackage;
}

Package::Ptr PackageGraph::findLoadedPackage(const std::string &packageName){
    PackageGraphPrivate* d = m_d;
    auto it = d->packages.find(packageName); // find in loaded packages
    if ( it != d->packages.end() )
        return it->second;

    auto internalIt = internals().find(packageName); // find in internals
    if ( internalIt != internals().end() )
        return internalIt->second;

    return Package::Ptr(nullptr);
}

Package::ConstPtr PackageGraph::findLoadedPackage(const std::string &packageName) const{
    const PackageGraphPrivate* d = m_d;
    auto it = d->packages.find(packageName); // find in loaded packages
    if ( it != d->packages.end() )
        return it->second;

    auto internalIt = internals().find(packageName); // find in internals
    if ( internalIt != internals().end() )
        return internalIt->second;

    return Package::ConstPtr(nullptr);
}

/** Returns the package with the given name internally */
Package::Ptr PackageGraph::package(const std::string &name){
    auto it = m_d->packages.find(name);
    if ( it != m_d->packages.end() )
        return it->second;
    return Package::Ptr(nullptr);
}

Package::ConstPtr PackageGraph::package(const std::string &name) const{
    auto it = m_d->packages.find(name);
    if ( it != m_d->packages.end() )
        return it->second;
    return Package::Ptr(nullptr);
}

/** Returns the package import paths */
const std::vector<std::string> &PackageGraph::packageImportPaths() const{
    return m_d->packageImportPaths;
}

/** Package import paths setter */
void PackageGraph::setPackageImportPaths(const std::vector<std::string> &paths){
    m_d->packageImportPaths = paths;
}

/**
 * \brief Adds internal package
 */
void PackageGraph::addInternalPackage(const Package::Ptr &package){
    auto it = internals().find(package->name());
    if ( it == internals().end() ){
        internals()[package->name()] = package;
        if ( internalsContextOwner() ){
            package->assignContext(internalsContextOwner());
        }
    }
}

/**
 * \brief Returns the internal packages
 */
std::map<std::string, Package::Ptr> &PackageGraph::internals(){
    static std::map<std::string, Package::Ptr> internals;
    return internals;
}


/**
 * \brief We assign the default internal context package graph for internal packages
 */
void PackageGraph::assignInternalContext(PackageGraph *graph){
    internalsContextOwner() = graph;
    for ( auto it = internals().begin(); it != internals().end(); ++it ){
        it->second->assignContext(graph);
    }
}

/** \brief Internal context package graph */
PackageGraph *&PackageGraph::internalsContextOwner(){
    static PackageGraph* pg = nullptr;
    return pg;
}

Module::Ptr PackageGraph::createRunningModule(const std::string &modulePath){
    Module::Ptr module = Module::createFromNode(modulePath, Path::join(modulePath, "live.module.json"), {
        {"name", "main"}, {"package", "."}, {"modules", "*"}
    });
    Package::Ptr package = Package::createFromNode(modulePath, Path::join(modulePath, "live.package.json"), {
        {"name", "."}, {"version", "0.1.0"}
    });

    package->assignContext(this);
    module->assignContext(this);
    module->context()->package = package;
    package->context()->modules["."] = module;

    return module;
}

void PackageGraph::loadRunningPackageAndModule(const Package::Ptr &package, const Module::Ptr &module){
    package->assignContext(this);
    module->assignContext(this);
    module->context()->package = package;
    package->context()->modules[module->name()] = module;

    std::string uriFromPackage = module->pathFromPackage();

    Utf8::replaceAll(uriFromPackage, "/",  ".");
    Utf8::replaceAll(uriFromPackage, "\\", ".");
    module->context()->importId = uriFromPackage.empty() ? package->name() : package->name() + "." + uriFromPackage;
}

/**
 * \brief Load module given the import segment
 */
Module::Ptr PackageGraph::loadModule(const std::string &importSegment, Module::Ptr requestingModule){
    return loadModule(splitString(importSegment, '.'), requestingModule);
}

/**
 * \brief Loads module given split-up import segments
 */
Module::Ptr PackageGraph::loadModule(const std::vector<std::string> &importSegments, Module::Ptr requestingModule){
    PackageGraphPrivate* d = m_d;

    if ( importSegments.empty() )
        THROW_EXCEPTION(lv::Exception, "Given import path is empty.", 5);

    std::string packageName = importSegments[0];

    Package::Ptr foundPackage = nullptr;

    if ( requestingModule ){
        Package::Ptr requestingPackage = requestingModule->context() ? requestingModule->context()->package : nullptr;

        if ( requestingPackage ){
            if ( requestingPackage->name() == packageName || (requestingPackage->name() == "." && packageName.empty() )){
                foundPackage = requestingPackage;
            }
        }
    }

    auto it = d->packages.find(packageName); // find in loaded packages
    if ( it != d->packages.end() )
        foundPackage = it->second;

    if ( foundPackage == nullptr ){
        auto internalIt = internals().find(packageName); // find in internals

        if ( internalIt != internals().end() ){
            foundPackage = internalIt->second;
        }
    }
    if ( foundPackage == nullptr ){
        Package::Ptr package = findPackage(packageName); // search for it within the paths
        if ( package ){
            loadPackage(package);
            foundPackage = package;
        }
    }

    if ( foundPackage == nullptr ){
        THROW_EXCEPTION(lv::Exception, "Failed to find package: " + packageName, 6);
    }

    if ( !foundPackage->filePath().empty() ){ // some internal packages may not have the package file
        std::string modulePath = foundPackage->path();
        std::string importId = packageName;
        for ( size_t i = 1; i < importSegments.size(); ++i ){
            modulePath += "/" + importSegments[i];
            importId += "." + importSegments[i];
        }
        auto moduleIt = foundPackage->context()->modules.find(importId);
        if ( moduleIt != foundPackage->context()->modules.end() ){
            addDependency(requestingModule, moduleIt->second);
            return moduleIt->second;
        }

        if ( !Module::existsIn(modulePath) )
            THROW_EXCEPTION(lv::Exception, Utf8("\'live.module.json\' file or .lv files have not been found in \'%\'").format(modulePath), Exception::toCode("~module"));

        Module::Ptr module = Module::createFromPath(modulePath);
        module->assignContext(this);
        module->context()->package  = foundPackage;
        module->context()->importId = importId;

        for ( auto it = module->dependencies().begin(); it != module->dependencies().end(); ++it )
            addDependency(module, *it);

        vlog("lvbase-packagegraph").v() << "Loaded module: " << importId;
        foundPackage->context()->modules[importId] = module;

        return module;
    }

    return Module::Ptr(nullptr);
}

/**
 * \brief Adds module dependency
 */
void PackageGraph::addDependency(const Module::Ptr &module, const std::string &dependency){
    Module::Ptr dependsOn = loadModule(dependency);
    if ( dependsOn == nullptr )
        THROW_EXCEPTION(
            lv::Exception,
            "Failed to find dependency \'" + dependency + "\' module for: " + module->context()->importId.data(),
            lv::Exception::toCode("~Dependency"));

    addDependency(module, dependsOn);
}

/**
 * \brief Add dependency between two given modules
 */
void PackageGraph::addDependency(const Module::Ptr& module, const Module::Ptr& dependsOn){
    if ( module.get() == dependsOn.get() )
        return;
    if ( module == nullptr )
        return;

    if ( module->context()->package.get() == dependsOn->context()->package.get() ){ // within the same package

        if ( !hasDependency(module, dependsOn) ){
            module->context()->localDependencies.push_back(dependsOn);
            dependsOn->context()->localDependents.push_back(module);

            PackageGraph::CyclesResult<Module::Ptr> cr = checkCycles(module);
            if ( cr.found() ){
                std::stringstream ss;

                for ( auto it = cr.path().begin(); it != cr.path().end(); ++it ){
                    Module::Ptr n = *it;
                    if ( it != cr.path().begin() )
                        ss << " -> ";
                    ss << n->name();
                }

                for ( auto it = module->context()->localDependencies.begin(); it != module->context()->localDependencies.end(); ++it ){
                    if ( *it == dependsOn ){
                        module->context()->localDependencies.erase(it);
                        break;
                    }
                }
                for ( auto it = dependsOn->context()->localDependents.begin(); it != dependsOn->context()->localDependents.end(); ++it ){
                    if ( *it == module ){
                        dependsOn->context()->localDependents.erase(it);
                        break;
                    }
                }

                THROW_EXCEPTION(lv::Exception, "Module dependency cycle found: "  + ss.str(), Exception::toCode("Cycle"));
            }
        }

    } else { // add package dependency instead
        addDependency(module->context()->package, dependsOn->context()->package);
    }
}

PaletteContainer *PackageGraph::paletteContainer() const{
    return m_d->paletteContainer;
}

bool PackageGraph::hasDependency(const Package::Ptr &package, const Package::Ptr &dependency){
    for ( auto it = package->context()->dependencies.begin(); it != package->context()->dependencies.end(); ++it ){
        if ( *it == dependency )
            return true;
    }
    return false;
}

bool PackageGraph::hasDependency(const Module::Ptr &module, const Module::Ptr &dependency){
    for ( auto it = module->context()->localDependencies.begin(); it != module->context()->localDependencies.end(); ++it ){
        if ( *it == dependency )
            return true;
    }
    return false;
}

std::string PackageGraph::toString(Package::Ptr package, const std::string &indent) const{
    std::stringstream ss;
    ss << indent << package->name() << "[" << package->version().toString() << "]" << std::endl;
    return ss.str();
}

std::string PackageGraph::toStringRecurse(Package::Ptr package, const std::string &indent) const{
    std::stringstream ss;
    ss << indent << package->name() << "[" << package->version().toString() << "]" << std::endl;
    for ( auto it = package->context()->dependencies.begin(); it != package->context()->dependencies.end(); ++it ){
        ss << toStringRecurse(*it, indent + "  ");
    }
    return ss.str();
}

}// namespace

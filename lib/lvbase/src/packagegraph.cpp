#include "packagegraph.h"
#include "live/packagecontext.h"
#include "live/plugincontext.h"
#include "live/exception.h"
#include "live/applicationcontext.h"
#include "live/libraryloadpath.h"
#include "live/visuallog.h"
#include "live/plugincontext.h"

#include <list>
#include <iostream>
#include <sstream>

namespace lv{

namespace{

    std::vector<std::string> splitString(const std::string &text, char sep) {
        std::vector<std::string> tokens;
        std::size_t start = 0, end = 0;
        while ((end = text.find(sep, start)) != std::string::npos) {
            tokens.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(text.substr(start));
        return tokens;
    }

}

/// @private
class PackageGraphPrivate{

public:
    std::vector<std::string> packageImportPaths;
    std::map<std::string, Package::Ptr> packages;
    std::map<std::string, PackageGraph::LibraryNode*> libraries;
};

/**
 * \class lv::PackageGraph
 * \brief Structure to represent packages and their dependencies
 *
 * It also stores all the libraries, stores in the LibraryNode structure.
 * We also check for dependency cycles on multiple levels: plugins, packages, Elements files
 * \ingroup lvbase
 */


/** Default constructor */
PackageGraph::PackageGraph()
    : m_d(new PackageGraphPrivate)
{
}

/** Default destructor */
PackageGraph::~PackageGraph(){
    clearPackages();
    clearLibraries();
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

        if ( newVersion.major() != oldVersion.major() ){
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
    if ( packageit == m_d->packages.end() )
        THROW_EXCEPTION(lv::Exception, "Failed to find package:" + package->name(), 3);
    if ( package->contextOwner() != this )
        THROW_EXCEPTION(lv::Exception, "Package \'" + package->name() +"\' is not part of the current package graph.", 3);
    if ( dependsOn->contextOwner() != this )
        THROW_EXCEPTION(lv::Exception, "Package \'" + dependsOn->name() +"\' is not part of the current package graph.", 3);

    auto dependsOnit = m_d->packages.find(dependsOn->name());
    if ( dependsOnit == m_d->packages.end() ){
        auto internalsIt = internals().find(dependsOn->name());
        if ( internalsIt == internals().end() ){
            THROW_EXCEPTION(lv::Exception, "Failed to find package:" + dependsOn->name(), 2);
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

            THROW_EXCEPTION(lv::Exception, "Package dependency cycle found :"  + ss.str(), 4);
        }
    }

}

/** Check if there are cycles between packages, starting from the given packages */
PackageGraph::CyclesResult<Package::Ptr> PackageGraph::checkCycles(const Package::Ptr &p){
    auto it = m_d->packages.find(p->name());
    if ( it == m_d->packages.end() )
        THROW_EXCEPTION(lv::Exception, "Failed to find package for cycles:" + p->name(), 2);

    std::list<Package::Ptr> path;
    path.push_back(p);

    for ( auto it = p->context()->dependencies.begin(); it != p->context()->dependencies.end(); ++it ){
        PackageGraph::CyclesResult<Package::Ptr> cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<Package::Ptr>(PackageGraph::CyclesResult<Package::Ptr>::NotFound);;
}

/** Check if there are cycles between plugins, starting from the given plugin */
PackageGraph::CyclesResult<Plugin::Ptr> PackageGraph::checkCycles(const Plugin::Ptr &p){
    if ( p->context() == nullptr || p->context()->packageGraph != this )
        THROW_EXCEPTION(lv::Exception, "Failed to loaded plugin for cycles:" + p->name(), 2);

    std::list<Plugin::Ptr> path;
    path.push_back(p);

    for ( auto it = p->context()->localDependencies.begin(); it != p->context()->localDependencies.end(); ++it ){
        PackageGraph::CyclesResult<Plugin::Ptr> cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<Plugin::Ptr>(PackageGraph::CyclesResult<Plugin::Ptr>::NotFound);;
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

PackageGraph::CyclesResult<Plugin::Ptr> PackageGraph::checkCycles(
        const Plugin::Ptr &p, Plugin::Ptr current, std::list<Plugin::Ptr> path)
{
    path.push_back(current);
    if ( current.get() == p.get() )
        return PackageGraph::CyclesResult<Plugin::Ptr>(PackageGraph::CyclesResult<Plugin::Ptr>::Found, path);

    for ( auto it = current->context()->localDependencies.begin(); it != current->context()->localDependencies.end(); ++it ){
        PackageGraph::CyclesResult<Plugin::Ptr> cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult<Plugin::Ptr>(PackageGraph::CyclesResult<Plugin::Ptr>::NotFound);
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
    auto it = m_d->libraries.find(lib.name);
    if ( it == m_d->libraries.end() ){
        PackageGraph::LibraryNode* libnode = new PackageGraph::LibraryNode(lib);
        libnode->loaded  = false;
        m_d->libraries[lib.name] = libnode;

        vlog("lvbase-packagegraph").d() << "Added library \'" << lib.name << "\' [" + lib.version.toString() << "]";

    } else {
        PackageGraph::LibraryNode* libnode = it->second;
        Version oldVersion = libnode->library.version;
        Version newVersion = lib.version;

        if ( newVersion.major() != oldVersion.major() ){
            THROW_EXCEPTION(
                lv::Exception,
                "Incompatible library versions for library \'" + lib.name + "\': " +
                oldVersion.toString() + " vs " + newVersion.toString(), 6);
        }

        if ( newVersion > oldVersion && libnode->loaded ){
            THROW_EXCEPTION(
                lv::Exception,
                "New library version provided for library \'" + lib.name + "\', however older library has already " +
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
                "Incompatible library compilation for library \'"  + lib.name + "\'",
                8
            );
        } else if ( flags == Package::Library::HasLess ){ // current library has less
            if ( newVersion.minor() < oldVersion.minor() ){
                THROW_EXCEPTION(
                    lv::Exception,
                    "Newer library \'" + lib.name + "\' [" + newVersion.toString() + "] has incompatible compilation " +
                    "flags than older library \'" + oldVersion.toString() + "\', ", 10);
            }

            vlog("lvbase-packagegraph").d() <<
                "Replaced library \'" << lib.name << "\' from [" << libnode->library.version.toString() << "] to [" <<
                lib.version.toString() << "]";

            libnode->library = lib;
        } else if ( flags == Package::Library::HasMore ){ // current library has more, do not replace
            if ( newVersion.minor() > oldVersion.minor() ){
                THROW_EXCEPTION(
                    lv::Exception,
                    "Loaded library \'" + lib.name + "\' [" + oldVersion.toString() + "] has incompatible compilation " +
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
            LibraryLoadPath::addRecursive(libnode->library.path, ApplicationContext::instance().linkPath());
            libnode->loaded = true;
        }
    }
}

/** Clesars all packages from the graph */
void PackageGraph::clearPackages(){
    m_d->packages.clear();
}

/** Clears all libraries from the graph */
void PackageGraph::clearLibraries(){
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it ){
        delete it->second;
    }
    m_d->libraries.clear();
}

/** Nice string representation of the PackageGraph */
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
        ss << "  " << libnode->library.name << "[" << libnode->library.version.toString() << "]:" << (libnode->loaded ? "loaded" : "not loaded") << std::endl;
    }

    ss << std::endl;

    return ss.str();
}

/** Finds the package according to the given reference */
Package::Ptr PackageGraph::findPackage(Package::Reference ref){
    std::vector<std::string> paths = packageImportPaths();
    for ( auto it = paths.begin(); it != paths.end(); ++it ){
        std::string path = *it + "/" + ref.name;
        if ( Package::existsIn(path) ){
            Package::Ptr p = Package::createFromPath(path);
            if ( p->version().major() == ref.version.major() && p->version() > ref.version ){
                return p;
            }
        }

        std::string pathMajor = path + "." + std::to_string(ref.version.major());
        if ( Package::existsIn(pathMajor) ){
            Package::Ptr p = Package::createFromPath(path);
            if ( p->version().major() == ref.version.major() && p->version() > ref.version ){
                return p;
            }
        }

        std::string pathMajorMinor = pathMajor + "." + std::to_string(ref.version.minor());
        if ( Package::existsIn(pathMajorMinor) ){
            Package::Ptr p = Package::createFromPath(path);
            if ( p->version().major() == ref.version.major() && p->version() > ref.version ){
                return p;
            }
        }
    }

    return Package::Ptr(nullptr);
}

/**
 * @brief Finds the package with the given name and the highest version in the package import paths
 */
Package::Ptr PackageGraph::findPackage(const std::string &packageName){
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

/** Returns the package with the given name internally */
Package::Ptr PackageGraph::package(const std::string &name){
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

/** Adds internal package */
void PackageGraph::addInternalPackage(const Package::Ptr &package){
    auto it = internals().find(package->name());
    if ( it == internals().end() ){
        internals()[package->name()] = package;
        if ( internalsContextOwner() ){
            package->assignContext(internalsContextOwner());
        }
    }
}

/** Returns the internal packages */
std::map<std::string, Package::Ptr> &PackageGraph::internals(){
    static std::map<std::string, Package::Ptr> internals;
    return internals;
}


/** We assign the default internal context package graph for internal packages */
void PackageGraph::assignInternalContext(PackageGraph *graph){
    internalsContextOwner() = graph;
    for ( auto it = internals().begin(); it != internals().end(); ++it ){
        it->second->assignContext(graph);
    }
}

/** Internal context package graph */
PackageGraph *&PackageGraph::internalsContextOwner(){
    static PackageGraph* pg = nullptr;
    return pg;
}

/** Load plugin given the import segment */
Plugin::Ptr PackageGraph::loadPlugin(const std::string &importSegment){
    return loadPlugin(splitString(importSegment, '.'));
}

/** Loads plugin given split-up import segments */
Plugin::Ptr PackageGraph::loadPlugin(const std::vector<std::string> &importSegments){
    PackageGraphPrivate* d = m_d;

    if ( importSegments.empty() )
        THROW_EXCEPTION(lv::Exception, "Given import path is empty.", 5);

    std::string packageName = importSegments[0];

    Package::Ptr foundPackage = nullptr;
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
        std::string pluginPath = foundPackage->path();
        std::string importId = packageName;
        for ( size_t i = 1; i < importSegments.size(); ++i ){
            pluginPath += "/" + importSegments[i];
            importId += "." + importSegments[i];
        }

        auto pluginIt = foundPackage->context()->plugins.find(importId);
        if ( pluginIt != foundPackage->context()->plugins.end() ){
            return pluginIt->second;
        }

        if ( !Plugin::existsIn(pluginPath) )
            THROW_EXCEPTION(lv::Exception, "No \'live.plugin.json\' file has been found in " + pluginPath, 7);

        Plugin::Ptr plugin = Plugin::createFromPath(pluginPath);
        plugin->assignContext(this);
        plugin->context()->package  = foundPackage;
        plugin->context()->importId = importId;

        for ( auto it = plugin->dependencies().begin(); it != plugin->dependencies().end(); ++it )
            addDependency(plugin, *it);

        vlog("lvbase-packagegraph").v() << "Loaded plugin: " << importId;

        foundPackage->context()->plugins[importId] = plugin;

        return plugin;
    }

    return Plugin::Ptr(nullptr);
}

/** Adds plugin dependency */
void PackageGraph::addDependency(const Plugin::Ptr &plugin, const std::string &dependency){
    Plugin::Ptr dependsOn = loadPlugin(dependency);
    if ( dependsOn == nullptr )
        THROW_EXCEPTION(
            lv::Exception,
            "Failed to find dependency \'" + dependency + "\' plugin for: " + plugin->context()->importId,
            8);

    addDependency(plugin, dependsOn);
}

/** Add dependency between two given plugins */
void PackageGraph::addDependency(const Plugin::Ptr& plugin, const Plugin::Ptr& dependsOn){
    if ( plugin.get() == dependsOn.get() )
        return;

    if ( plugin->context()->package.get() == dependsOn->context()->package.get() ){ // within the same package

        if ( !hasDependency(plugin, dependsOn) ){
            plugin->context()->localDependencies.push_back(dependsOn);
            dependsOn->context()->localDependents.push_back(plugin);

            PackageGraph::CyclesResult<Plugin::Ptr> cr = checkCycles(plugin);
            if ( cr.found() ){
                std::stringstream ss;

                for ( auto it = cr.path().begin(); it != cr.path().end(); ++it ){
                    Plugin::Ptr n = *it;
                    if ( it != cr.path().begin() )
                        ss << " -> ";
                    ss << n->name();
                }

                for ( auto it = plugin->context()->localDependencies.begin(); it != plugin->context()->localDependencies.end(); ++it ){
                    if ( *it == dependsOn ){
                        plugin->context()->localDependencies.erase(it);
                        break;
                    }
                }
                for ( auto it = dependsOn->context()->localDependents.begin(); it != dependsOn->context()->localDependents.end(); ++it ){
                    if ( *it == plugin ){
                        dependsOn->context()->localDependents.erase(it);
                        break;
                    }
                }

                THROW_EXCEPTION(lv::Exception, "Plugin dependency cycle found :"  + ss.str(), 4);
            }
        }

    } else { // add package dependency instead
        addDependency(plugin->context()->package, dependsOn->context()->package);
    }
}

bool PackageGraph::hasDependency(const Package::Ptr &package, const Package::Ptr &dependency){
    for ( auto it = package->context()->dependencies.begin(); it != package->context()->dependencies.end(); ++it ){
        if ( *it == dependency )
            return true;
    }
    return false;
}

bool PackageGraph::hasDependency(const Plugin::Ptr &plugin, const Plugin::Ptr &dependency){
    for ( auto it = plugin->context()->localDependencies.begin(); it != plugin->context()->localDependencies.end(); ++it ){
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

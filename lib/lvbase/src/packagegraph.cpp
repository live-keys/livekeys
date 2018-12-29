#include "packagegraph.h"
#include "live/exception.h"
#include "live/applicationcontext.h"
#include "live/libraryloadpath.h"
#include "live/visuallog.h"

#include <list>
#include <iostream>
#include <sstream>

namespace lv{

class PackageGraphPrivate{

public:
    std::vector<std::string> packageImportPaths;
    std::map<std::string, PackageGraph::Node*> packages;
    std::map<std::string, PackageGraph::LibraryNode*> libraries;
};

PackageGraph::PackageGraph()
    : m_d(new PackageGraphPrivate)
{
}

PackageGraph::~PackageGraph(){
    clearPackages();
    clearLibraries();
    delete m_d;
}

void PackageGraph::loadPackage(const Package::Ptr &p, bool addLibraries){
    auto it = m_d->packages.find(p->name());
    if ( it == m_d->packages.end() ){
        PackageGraph::Node* node = new PackageGraph::Node;
        node->package = p;

        if ( addLibraries ){
            for ( auto it = p->libraries().begin(); it != p->libraries().end(); ++it ){
                Package::Library* lib = it->second;
                addLibrary(*lib);
            }
        }

        m_d->packages[p->name()] = node;

        vlog("lvbase-packagegraph").d() << "Loaded package \'" + p->name() << "\' [" + p->version().toString() + "]";

    } else {
        PackageGraph::Node* node = it->second;
        Version oldVersion = node->package->version();
        Version newVersion = p->version();

        if ( newVersion.major() != oldVersion.major() ){
            THROW_EXCEPTION(
                lv::Exception,
                "Incompatible package versions for package \'" + p->name() + "\': " +
                oldVersion.toString() + " vs " + newVersion.toString(), 1);
        }

        if ( newVersion > oldVersion ){
            vlog("lvbase-packagegraph").d() <<
                "Replaced package \'" << p->name() << "\' from [" << node->package->version().toString() << "] to [" <<
                p->version().toString() << "]";

            node->package = p;

            if ( addLibraries ){
                for ( auto it = p->libraries().begin(); it != p->libraries().end(); ++it ){
                    Package::Library* lib = it->second;
                    addLibrary(*lib);
                }
            }
        }
    }
}

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

void PackageGraph::addDependency(const Package::Ptr &package, const Package::Ptr &dependsOn){
    auto packageit = m_d->packages.find(package->name());
    if ( packageit == m_d->packages.end() ){
        THROW_EXCEPTION(lv::Exception, "Failed to find package:" + package->name(), 3);
    }
    PackageGraph::Node* packageNode = packageit->second;

    auto dependsOnit = m_d->packages.find(dependsOn->name());
    if ( dependsOnit == m_d->packages.end() ){
        auto internalsIt = internals().find(dependsOn->name());
        if ( internalsIt == internals().end() ){
            THROW_EXCEPTION(lv::Exception, "Failed to find package:" + dependsOn->name(), 2);
        } else {
            PackageGraph::Node* dependsOnNode = internalsIt->second;
            if ( !hasDependency(packageNode, dependsOnNode) )
                packageNode->dependencies.push_back(dependsOnNode);
            return;
        }
    }
    PackageGraph::Node* dependsOnNode = dependsOnit->second;

    if ( !hasDependency(packageNode, dependsOnNode ) ){
        packageNode->dependencies.push_back(dependsOnNode);
        dependsOnNode->dependents.push_back(packageNode);

        PackageGraph::CyclesResult cr = checkCycles(dependsOnNode->package);
        if ( cr.found() ){
            std::stringstream ss;

            for ( auto it = cr.path().begin(); it != cr.path().end(); ++it ){
                PackageGraph::Node* n = *it;

                if ( it != cr.path().begin() ){
                    ss << " -> ";
                }

                ss << n->package->name() << "[" << n->package->version().toString() << "]";
            }

            for ( auto it = packageNode->dependencies.begin(); it != packageNode->dependencies.end(); ++it ){
                if ( *it == dependsOnNode ){
                    packageNode->dependencies.erase(it);
                    break;
                }
            }
            for ( auto it = dependsOnNode->dependents.begin(); it != dependsOnNode->dependents.end(); ++it ){
                if ( *it == dependsOnNode ){
                    dependsOnNode->dependents.erase(it);
                    break;
                }
            }

            THROW_EXCEPTION(lv::Exception, "Package dependency cycle found :"  + ss.str(), 4);
        }
    }

}

PackageGraph::CyclesResult PackageGraph::checkCycles(const Package::Ptr &p){
    auto it = m_d->packages.find(p->name());
    if ( it == m_d->packages.end() )
        THROW_EXCEPTION(lv::Exception, "Failed to find package for cycles:" + p->name(), 2);
    PackageGraph::Node* node = it->second;

    std::list<PackageGraph::Node*> path;
    path.push_back(node);

    for ( auto it = node->dependencies.begin(); it != node->dependencies.end(); ++it ){
        PackageGraph::CyclesResult cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult(PackageGraph::CyclesResult::NotFound);;
}

PackageGraph::CyclesResult PackageGraph::checkCycles(
        const Package::Ptr &p, PackageGraph::Node *node, std::list<PackageGraph::Node*> path)
{
    path.push_back(node);
    if ( node->package.get() == p.get() )
        return PackageGraph::CyclesResult(PackageGraph::CyclesResult::Found, path);

    for ( auto it = node->dependencies.begin(); it != node->dependencies.end(); ++it ){
        PackageGraph::CyclesResult cr = checkCycles(p, *it, path);
        if ( cr.found() )
            return cr;
    }
    return PackageGraph::CyclesResult(PackageGraph::CyclesResult::NotFound);
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

void PackageGraph::loadLibraries(){
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it ){
        PackageGraph::LibraryNode* libnode = it->second;
        if ( !libnode->loaded ){
            LibraryLoadPath::addRecursive(libnode->library.path, ApplicationContext::instance().linkPath());
            libnode->loaded = true;
        }
    }
}

void PackageGraph::clearPackages(){
    for ( auto it = m_d->packages.begin(); it != m_d->packages.end(); ++it ){
        delete it->second;
    }
}

void PackageGraph::clearLibraries(){
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it ){
        delete it->second;
    }
}

std::string PackageGraph::toString() const{
    const PackageGraphPrivate* d = m_d;

    std::stringstream ss;

    ss << "Internals:" << std::endl;
    for ( auto it = internals().begin(); it != internals().end(); ++it ){
        ss << "  " << it->second->package->name() << "[" << it->second->package->version().toString() << "]";
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

Package::Ptr PackageGraph::package(const std::string &name){
    auto it = m_d->packages.find(name);
    if ( it != m_d->packages.end() )
        return it->second->package;
    return Package::Ptr(nullptr);
}

const std::vector<std::string> &PackageGraph::packageImportPaths() const{
    return m_d->packageImportPaths;
}

void PackageGraph::setPackageImportPaths(const std::vector<std::string> &paths){
    m_d->packageImportPaths = paths;
}

void PackageGraph::addInternalPackage(const Package::Ptr &package){
    auto it = internals().find(package->name());
    if ( it == internals().end() ){
        PackageGraph::Node* node = new PackageGraph::Node;
        node->package = package;
        internals()[package->name()] = node;
    }
}

std::map<std::string, PackageGraph::Node*> &PackageGraph::internals(){
    static std::map<std::string, PackageGraph::Node*> internals;
    return internals;
}

Plugin::Ptr PackageGraph::loadPlugin(const std::string &importSegment){
    //TODO: find loaded package
    // if package:
        // find path based on loaded package
    // else
        // find plugin,
        // find plugin package, load package
    // load plugin
    // update plugin and package dependencies
    return Plugin::Ptr(nullptr);
}

bool PackageGraph::hasDependency(PackageGraph::Node *node, PackageGraph::Node *dependency){
    for ( auto it = node->dependencies.begin(); it != node->dependencies.end(); ++it ){
        if ( *it == dependency )
            return true;
    }
    return false;
}


std::string PackageGraph::toString(PackageGraph::Node *node, const std::string &indent) const{
    std::stringstream ss;
    ss << indent << node->package->name() << "[" << node->package->version().toString() << "]" << std::endl;
    return ss.str();
}

std::string PackageGraph::toStringRecurse(PackageGraph::Node *node, const std::string &indent) const{
    std::stringstream ss;
    ss << indent << node->package->name() << "[" << node->package->version().toString() << "]" << std::endl;
    for ( auto it = node->dependencies.begin(); it != node->dependencies.end(); ++it ){
        PackageGraph::Node* n = *it;
        ss << toString(n, indent + "  ");
    }
    return ss.str();
}

}// namespace

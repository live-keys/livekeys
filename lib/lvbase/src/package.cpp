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

#include "package.h"
#include "packagecontext.h"

#include "live/mlnodetojson.h"
#include "live/exception.h"
#include "live/visuallog.h"
#include "live/library.h"
#include "live/path.h"
#include "live/directory.h"

#include <list>
#include <map>
#include <fstream>
#include <istream>
#include <sstream>

/**
  \class lv::Package
  \brief Manages a Livekeys package.
  \ingroup lvbase
 */

namespace lv{

const char* Package::fileName = "live.package.json";

/// \private
class PackagePrivate{

public:
    std::string name;
    std::string path;
    std::string filePath;
    std::string documentation;
    std::string release;
    Version     version;
    std::map<std::string, Package::Reference*> dependencies;
    std::map<std::string, Package::Library*>   libraries;
    std::vector<std::string> internalLibraries;

    std::string workspaceLabel;
    std::vector<std::pair<std::string, std::string> > workspaceTutorialSections;
    std::vector<Package::ProjectEntry> workspaceSamples;

    Package::Context* context;
};

/** \brief Destructor of Package */
Package::~Package(){
    for ( auto it = m_d->dependencies.begin(); it != m_d->dependencies.end(); ++it )
        delete it->second;
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it )
        delete it->second;

    delete m_d->context;
    delete m_d;
}

/** Static method that checks if a package exists in the given folder */
bool Package::existsIn(const std::string &path){
    if ( !Path::exists(path) )
        return false;
    return Path::exists(Path::join(path, Package::fileName));
}

/** Creates a package pointer from that path, considering that path has a package */
Package::Ptr Package::createFromPath(const std::string &path){
    std::string packagePath = path;
    std::string packageDirPath;

    if ( Path::isDir(path) ){
        packageDirPath = Path::resolve(path);
        packagePath    = Path::join(packageDirPath, Package::fileName);
    } else {
        packageDirPath = Path::parent(path);
    }

    std::ifstream instream(packagePath, std::ifstream::in | std::ifstream::binary);
    if ( !instream.is_open() ){
        THROW_EXCEPTION(lv::Exception, Utf8("Cannot open package file: %. The file might not exist or the path might be wrong.").format(path), 1);
    }

    instream.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(instream.tellg());
    std::string buffer(size, ' ');
    instream.seekg(0);
    instream.read(&buffer[0], size);

    MLNode m;
    ml::fromJson(buffer, m);

    return createFromNode(packageDirPath, packagePath, m);
}

/** This function actually creates the path pointer that is returned to the above function */
Package::Ptr Package::createFromNode(const std::string& path, const std::string &filePath, const MLNode &m){
    if ( !m.hasKey("name") || !m.hasKey("version") )
        return Package::Ptr(nullptr);


    Package::Ptr pt(new Package(path, filePath, m["name"].asString(), Version(m["version"].asString())));

    if ( m.hasKey("dependencies") ){
        MLNode::ObjectType dep = m["dependencies"].asObject();
        for ( auto it = dep.begin(); it != dep.end(); ++it ){
            Package::Reference* dep = new Package::Reference(it->first, Version(it->second.asString()));
            pt->m_d->dependencies[dep->name.data()] = dep;
        }
    }

    if ( m.hasKey("libraries") ){
        MLNode::ObjectType libs = m["libraries"].asObject();
        for ( auto it = libs.begin(); it != libs.end(); ++it ){
            MLNode libValue = it->second;

            Package::Library* lib = new Package::Library(it->first, Version(libValue["version"].asString()));
            lib->path = path + "/" + libValue["path"].asString();
            if ( libValue.hasKey("flags") ){
                for ( auto it = libValue["flags"].asArray().begin(); it != libValue["flags"].asArray().end(); ++it ){
                    lib->flags.push_back(it->asString());
                }
            }

            pt->m_d->libraries[it->first] = lib;
        }
    }
    if ( m.hasKey("internalLibraries") ){
        MLNode::ArrayType libs = m["internalLibraries"].asArray();
        for ( auto it = libs.begin(); it != libs.end(); ++it ){
            pt->m_d->internalLibraries.push_back(it->asString());
        }
    }

    if ( m.hasKey("documentation" ) ){
        pt->m_d->documentation = m["documentation"].asString();
    }

    if ( m.hasKey("release") ){
        pt->m_d->release = m["release"].asString();
    }

    if ( m.hasKey("workspace") ){
        const MLNode& workspace = m["workspace"];
        if ( workspace.hasKey("label") )
            pt->m_d->workspaceLabel = workspace["label"].asString();

        if ( workspace.hasKey("tutorials") ){
            const MLNode& tutorials = workspace["tutorials"];

            MLNode::ArrayType sections = tutorials["sections"].asArray();
            for ( auto it = sections.begin(); it != sections.end(); ++it ){
                pt->m_d->workspaceTutorialSections.push_back(
                    std::make_pair((*it)["label"].asString(), (*it)["link"].asString())
                );
            }
        }
        if ( workspace.hasKey("samples") ){
            MLNode::ArrayType sections = workspace["samples"].asArray();
            for ( auto it = sections.begin(); it != sections.end(); ++it ){
                std::string link = (*it)["link"].asString();
                std::string label = (*it).hasKey("label") ? (*it)["label"].asString() : "";
                if ( label.empty() ){
                    size_t pos = link.find_last_of('/');
                    label = pos == std::string::npos ? link : link.substr(pos + 1);
                }
                std::string category = (*it).hasKey("category") ? (*it)["category"].asString() : "";
                std::string description = (*it).hasKey("description") ? (*it)["description"].asString() : "";
                std::string icon = (*it).hasKey("icon") ? (*it)["icon"].asString() : "";

                pt->m_d->workspaceSamples.push_back(
                    ProjectEntry(link, label, category, description, icon)
                );
            }
        }
    }

    return pt;
}

/** \brief Returns the package name */
const std::string &Package::name() const{
    return m_d->name;
}

/** \brief Returns the package path */
const std::string &Package::path() const{
    return m_d->path;
}

/** \brief Returns the filepath of the package */
const std::string &Package::filePath() const{
    return m_d->filePath;
}

/** \brief Returns the package documentation */
const std::string &Package::documentation() const{
    return m_d->documentation;
}

/** \brief Returns the package release */
const std::string &Package::release() const{
    return m_d->release;
}

/** \brief Returns the package version */
const Version &Package::version() const{
    return m_d->version;
}

/** \brief Returns a map of dependencies with string keys */
const std::map<std::string, Package::Reference*>& Package::dependencies() const{
    return m_d->dependencies;
}


/** \brief Returns a map of libraries with string keys */
const std::map<std::string, Package::Library *>& Package::libraries() const{
    return m_d->libraries;
}

const std::vector<std::string> Package::internalLibraries() const{
    return m_d->internalLibraries;
}

/** \brief Returns a list of all the module paths within this package */
std::vector<std::string> Package::allModules() const{
    return findModules(path());
}

/** \brief Assigns a new context to this package. */
void Package::assignContext(PackageGraph *graph){
    if ( m_d->context ){
        if ( m_d->context->packageGraph == graph )
            return;
        delete m_d->context;
    }

    for ( auto it = m_d->internalLibraries.begin(); it != m_d->internalLibraries.end(); ++it ){
        lv::Library::handleReference(m_d->path + "/" + *it);
    }

    m_d->context = new Package::Context;
    m_d->context->packageGraph = graph;
}

/** \brief Package graph */
PackageGraph *Package::contextOwner(){
    if ( m_d->context )
        return m_d->context->packageGraph;
    return nullptr;
}

/** \brief Returns the current context if any has been assigned, nullptr otherwise. */
Package::Context *Package::context(){
    return m_d->context;
}

bool Package::hasWorkspace() const{
    return m_d->workspaceTutorialSections.size() > 0;
}

const std::string &Package::workspaceLabel() const{
    return m_d->workspaceLabel;
}

const std::vector<std::pair<std::string, std::string> > &Package::workspaceTutorialsSections() const{
    return m_d->workspaceTutorialSections;
}

const std::vector<Package::ProjectEntry> &Package::workspaceSamples(){
    return m_d->workspaceSamples;
}

std::vector<std::string> Package::findModules(const std::string &path){
    std::vector<std::string> result;

    if ( Module::existsIn(path) )
        result.push_back(path);

    Directory::Iterator dit = Directory::iterate(path);
    while ( !dit.isEnd() ){
        if ( Path::isDir(dit.path()) ){
            auto scanResult = findModules(dit.path());
            result.insert(result.end(), scanResult.begin(), scanResult.end());
        }
        dit.next();
    }

    return result;
}

Package::Package(const std::string &path, const std::string& filePath, const std::string &name, const Version &version)
    : m_d(new PackagePrivate)
{
    m_d->path     = path;
    m_d->filePath = filePath;
    m_d->name     = name;
    m_d->version  = version;
    m_d->context  = nullptr;
}

/** Function that compares two sets of flags in order for us to select a better library */
Package::Library::FlagResult Package::Library::compareFlags(const Package::Library &other){
    bool thisHasMore = false;
    bool otherHasMore = false;
    for ( auto it = flags.begin(); it != flags.end(); ++it ){
        bool found = false;
        for ( auto oit = other.flags.begin(); oit != other.flags.end(); ++oit ){
            if ( *oit == *it ){
                found = true;
                break;
            }
        }
        if ( !found ){
            thisHasMore = true;
            break;
        }
    }

    for ( auto oit = other.flags.begin(); oit != other.flags.end(); ++oit ){
        bool found = false;
        for ( auto it = flags.begin(); it != flags.end(); ++it ){
            if ( *oit == *it ){
                found = true;
                break;
            }
        }
        if ( !found ){
            otherHasMore = true;
            break;
        }
    }

    if ( thisHasMore && otherHasMore ){
        return Package::Library::Different;
    } else if ( thisHasMore ) {
        return Package::Library::HasMore;
    } else if ( otherHasMore ){
        return Package::Library::HasLess;
    } else {
        return Package::Library::Equal;
    }
}

} // namespace

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

#include "module.h"
#include "modulecontext.h"

#include "live/mlnodetojson.h"
#include "live/exception.h"
#include "live/package.h"
#include "live/packagegraph.h"
#include "live/palettecontainer.h"
#include "live/visuallog.h"
#include "live/path.h"
#include "live/directory.h"
#include <list>
#include <map>

#include <fstream>
#include <istream>
#include <sstream>


/**
  \class lv::Module
  \brief Manages a Livekeys module.
  \ingroup lvbase
 */

namespace lv{

const char* Module::fileName = "live.module.json";

/// \private
class ModulePrivate{

public:
    std::string name;
    std::string path;
    std::string filePath;
    std::string package;
    std::list<std::pair<std::string, std::string> > palettes;
    std::list<std::string> dependencies;
    std::list<std::string> modules;
    std::list<std::string> libraryModules;
    std::list<std::string> assets;
    Module::Context* context;
};

/** Default destructor */
Module::~Module(){
    delete m_d;
}

/** Checks if live.module.json exists in the given path */
bool Module::existsIn(const std::string &path){
    if ( Module::fileExistsIn(path) )
        return true;
    return Module::lvFilesExistIn(path);
}

bool Module::fileExistsIn(const std::string &path){
    if ( !Path::exists(path) )
        return false;
    return Path::exists(Path::join(path, Module::fileName));
}

/** Creates module from a given path */
Module::Ptr Module::createFromPath(const std::string &path){
    std::string modulePath = path;
    std::string moduleDirPath;

    if ( Path::isDir(modulePath) ){
        moduleDirPath = Path::resolve(modulePath);
        modulePath    = Path::join(moduleDirPath, Module::fileName);
    } else {
        moduleDirPath = Path::parent(modulePath);
    }

    if ( Path::exists(modulePath) ){
        std::ifstream instream(modulePath, std::ifstream::in | std::ifstream::binary);
        if ( !instream.is_open() ){
            THROW_EXCEPTION(lv::Exception, Utf8("Cannot open file: '%'").format(path), lv::Exception::toCode("~file"));
        }

        try{
            instream.seekg(0, std::ios::end);
            size_t size = static_cast<size_t>(instream.tellg());
            std::string buffer(size, ' ');
            instream.seekg(0);
            instream.read(&buffer[0], static_cast<std::streamsize>(size));

            MLNode m;
            ml::fromJson(buffer, m);

            return createFromNode(moduleDirPath, modulePath, m);
        } catch ( lv::Exception& e ){
            THROW_EXCEPTION(lv::Exception, Utf8("Error when parsing file %: %").format(modulePath, e.message()), lv::Exception::toCode("parse"));
        } catch ( std::exception& e ){
            THROW_EXCEPTION(lv::Exception, Utf8("Error when parsing file %: %").format(modulePath, e.what()), lv::Exception::toCode("parse"));
        }
    } else {
        std::string name = Path::name(moduleDirPath);
        std::string package = Module::findPackageFrom(moduleDirPath);
        if ( package.empty() || !Path::exists(package))
            THROW_EXCEPTION(lv::Exception, Utf8("Package not found for module: \'%\'.").format(moduleDirPath), Exception::toCode("~Keys"));
        if ( Path::isRelative(package) )
            package = Path::resolve(Path::join(path, package));
        if ( Path::isDir(package) )
            package = Path::join(package, Package::fileName);

        Module::Ptr pt(new Module(moduleDirPath, modulePath, name, package));

        auto files = Module::scanLvFiles(path);
        for ( auto it = files.begin(); it != files.end(); ++it )
            pt->m_d->modules.push_back(*it);

        return pt;
    }

    return nullptr;
}

/** Creates plugin from a given MLNode*/
Module::Ptr Module::createFromNode(const std::string &path, const std::string &filePath, const MLNode &m){
    std::string name = m.hasKey("name") ? m["name"].asString() : Path::name(path);
    std::string package = m.hasKey("package") ? m["package"].asString() : Module::findPackageFrom(path);

    if ( package.empty() || !Path::exists(package))
        THROW_EXCEPTION(lv::Exception, Utf8("Package not found for module: \'%\'.").format(filePath), Exception::toCode("~Keys"));

    if ( Path::isRelative(package) )
        package = Path::resolve(Path::join(path, package));
    if ( Path::isDir(package) )
        package = Path::join(package, Package::fileName);

    Module::Ptr pt(new Module(path, filePath, name, package));

    if ( m.hasKey("palettes") ){
        MLNode::ObjectType pal = m["palettes"].asObject();
        for ( auto it = pal.begin(); it != pal.end(); ++it ){
            if ( it->second.type() == MLNode::Array ){
                const MLNode::ArrayType& itArray = it->second.asArray();
                for ( auto itItem = itArray.begin(); itItem != itArray.end(); ++itItem ){
                    pt->m_d->palettes.push_back(std::make_pair(it->first, itItem->asString()));
                    pt->addPalette(itItem->asString(), it->first);
                }
            } else {
                pt->m_d->palettes.push_back(std::make_pair(it->first, it->second.asString()));
                pt->addPalette(it->second.asString(), it->first);
            }
        }
    }

    if ( m.hasKey("dependencies") ){
        MLNode::ArrayType dep = m["dependencies"].asArray();
        for ( auto it = dep.begin(); it != dep.end(); ++it ){
            pt->m_d->dependencies.push_back(it->asString());
        }
    }

    if ( m.hasKey("modules") ){
        MLNode moduleDef = m["modules"];
        if ( moduleDef.type() == MLNode::Array ){
            MLNode::ArrayType mod = m["modules"].asArray();
            for ( auto it = mod.begin(); it != mod.end(); ++it ){
                pt->m_d->modules.push_back(it->asString());
            }
        } else if ( moduleDef.type() == MLNode::String ){
            std::string mod = moduleDef.asString();
            if ( mod == "*" ){
                auto files = Module::scanLvFiles(path);
                for ( auto it = files.begin(); it != files.end(); ++it )
                    pt->m_d->modules.push_back(*it);
            } else {
                pt->m_d->modules.push_back(mod);
            }
        } else {
            THROW_EXCEPTION(lv::Exception, "'modules' key expected to be array or string.", lv::Exception::toCode("~Parse"));
        }

    }

    if ( m.hasKey("libraryModules") ){
        MLNode::ArrayType libmod = m["libraryModules"].asArray();
        for ( auto it = libmod.begin(); it != libmod.end(); ++it ){
            pt->m_d->libraryModules.push_back(it->asString());
        }
    }

    if ( m.hasKey("assets") ){
        MLNode::ArrayType assets = m["assets"].asArray();
        for ( auto it = assets.begin(); it != assets.end(); ++it ){
            pt->m_d->assets.push_back(it->asString());
        }
    }

    return pt;
}

/** Create empty plugin */
Module::Ptr Module::createEmpty(const std::string &name){
    Module::Ptr pt(new Module(name, name, name, ""));
    return pt;
}

/** Name getter */
const std::string &Module::name() const{
    return m_d->name;
}

/** Path getter */
const std::string &Module::path() const{
    return m_d->path;
}

/** File path getter */
const std::string &Module::filePath() const{
    return m_d->filePath;
}

/** Returns the package path */
std::string Module::packagePath() const{
    return Path::parent(package());
}

/** Gets this plugins path relative to the package */
std::string Module::pathFromPackage() const{
    std::string thisPath = path();
    Utf8::replaceAll(thisPath, packagePath(), "");
    return (thisPath.empty()) ? thisPath : (thisPath[0] == '/' || thisPath[0] == '\\' ? thisPath.substr(1) : thisPath);
}

/** Configured package path getter */
const std::string &Module::package() const{
    return m_d->package;
}

/** Dependencies getter */
const std::list<std::string> &Module::dependencies() const{
    return m_d->dependencies;
}

/** Modules getter */
const std::list<std::string> &Module::fileModules() const{
    return m_d->modules;
}

/** Library modules getter */
const std::list<std::string> &Module::libraryModules() const{
    return m_d->libraryModules;
}

const std::list<std::string> &Module::assets() const{
    return m_d->assets;
}

/** Palettes getter */
const std::list<std::pair<std::string, std::string> > &Module::palettes() const{
    return m_d->palettes;
}

/** Assign package graph context */
void Module::assignContext(PackageGraph *graph){
    if ( m_d->context )
        delete m_d->context;

    m_d->context = new Module::Context;
    m_d->context->packageGraph = graph;

    if ( graph ){
        for ( auto it = m_d->palettes.begin(); it != m_d->palettes.end(); ++it ){
            addPalette(it->second, it->first);
        }
    }

}

/** Context getter */
Module::Context *Module::context(){
    return m_d->context;
}

Module::Module(const std::string &path, const std::string &filePath, const std::string &name, const std::string &package)
    : m_d(new ModulePrivate)
{
    m_d->path     = path;
    m_d->filePath = filePath;
    m_d->name     = name;
    m_d->package  = package;
    m_d->context  = nullptr;
}

bool Module::lvFilesExistIn(const std::string &dir){
    auto it = Directory::iterate(dir);
    while ( !it.isEnd() ){
        std::string path = it.path();
        if ( Path::extension(path) == ".lv" )
            return true;
        it.next();
    }
    return false;
}

std::list<std::string> Module::scanLvFiles(const std::string &dir){
    auto it = Directory::iterate(dir);
    std::list<std::string> result;
    while ( !it.isEnd() ){
        std::string path = it.path();
        if ( Path::extension(path) == ".lv" )
            result.push_back(Path::baseName(path));
        it.next();
    }
    return result;
}

std::string Module::findPackageFrom(const std::string& path){
    auto current = path;
    while ( Path::exists(current) ){
        if ( Package::existsIn(current) ){
            return current;
        }
        if ( Path::rootPath(current) == current ){
            return "";
        }
        current = Path::parent(current);
    }
    return "";
}

void Module::addPalette(const std::string &type, const std::string &path){
    if ( context() ){
        Utf8 extension = Path::suffix(path);
        Utf8 name = Path::baseName(path);
        Utf8 plugin = m_d->name;
        Utf8 fullPath = Path::join(m_d->path, path);
        context()->packageGraph->paletteContainer()->addPalette(type, fullPath, name, extension, plugin);
    }
}


}// namespace

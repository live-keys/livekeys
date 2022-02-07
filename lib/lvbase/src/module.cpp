#include "module.h"
#include "modulecontext.h"

#include "lockedfileiosession.h"
#include "live/mlnodetojson.h"
#include "live/exception.h"
#include "live/package.h"
#include "live/packagegraph.h"
#include "live/palettecontainer.h"
#include "live/visuallog.h"
#include <list>
#include <map>

#include <fstream>
#include <istream>
#include <sstream>

#include <QDir>
#include <QFileInfo>


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
    QDir d(QString::fromStdString(path));
    if ( !d.exists() )
         return false;

    QFileInfo finfo(d.path() + "/"  + QString(Module::fileName));
    return finfo.exists();
}

/** Creates module from a given path */
Module::Ptr Module::createFromPath(const std::string &path){
    QString modulePath = QString::fromStdString(path);
    QString moduleDirPath;

    QFileInfo finfo(modulePath);
    if ( finfo.isDir() ){
        modulePath = finfo.filePath() + "/" + QString(Module::fileName);
        moduleDirPath = finfo.filePath();
    } else {
        moduleDirPath = finfo.path();
    }

    std::ifstream instream(modulePath.toStdString(), std::ifstream::in | std::ifstream::binary);
    if ( !instream.is_open() ){
        THROW_EXCEPTION(lv::Exception, std::string("Cannot open file: ") + path, 1);
    }

    instream.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(instream.tellg());
    std::string buffer(size, ' ');
    instream.seekg(0);
    instream.read(&buffer[0], static_cast<std::streamsize>(size));

    MLNode m;
    ml::fromJson(buffer, m);

    return createFromNode(moduleDirPath.toStdString(), modulePath.toStdString(), m);
}

/** Creates plugin from a given MLNode*/
Module::Ptr Module::createFromNode(const std::string &path, const std::string &filePath, const MLNode &m){
    if ( !m.hasKey("name") || !m.hasKey("package") ){
        THROW_EXCEPTION(lv::Exception, "Failed to read plugin file at: " + path + ". Plugin requries 'name' and 'package' info.", Exception::toCode("~Keys"));
    }

    QString package = QString::fromStdString(m["package"].asString());

    QFileInfo finfo(package);
    if ( finfo.isRelative() ){
        package = QDir::cleanPath(QString::fromStdString(path) + "/" + package);
        finfo = QFileInfo(package);
    }

    if ( finfo.isDir() ){
        package = finfo.filePath() + "/" + QString(Package::fileName);
    }


    Module::Ptr pt(new Module(path, filePath, m["name"].asString(), package.toStdString()));


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
        MLNode::ArrayType mod = m["modules"].asArray();
        for ( auto it = mod.begin(); it != mod.end(); ++it ){
            pt->m_d->modules.push_back(it->asString());
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
    return QFileInfo(QString::fromStdString(package())).path().toStdString();
}

/** Gets this plugins path relative to the package */
std::string Module::pathFromPackage() const{
    std::string thisPath = path();
    Utf8::replaceAll(thisPath, packagePath(), "");
    return (thisPath.empty()) ? thisPath : (thisPath[0] == '/' ? thisPath.substr(1) : thisPath);
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

void Module::addPalette(const std::string &type, const std::string &path){
    if ( context() ){
        QFileInfo finfo(QString::fromStdString(path));
        Utf8 extension = finfo.suffix().toStdString();
        Utf8 name = finfo.baseName().toStdString();
        Utf8 plugin = m_d->name;
        Utf8 fullPath = m_d->path + "/" + path;
        context()->packageGraph->paletteContainer()->addPalette(type, fullPath, name, extension, plugin);
    }
}


}// namespace

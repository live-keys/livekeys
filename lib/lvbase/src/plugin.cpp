#include "plugin.h"
#include "plugincontext.h"

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
  \class lv::Plugin
  \brief Manages a Livekeys plugin.
  \ingroup lvbase
 */

namespace lv{

const char* Plugin::fileName = "live.plugin.json";

/// \private
class PluginPrivate{

public:
    std::string name;
    std::string path;
    std::string filePath;
    std::string package;
    std::list<std::pair<std::string, std::string> > palettes;
    std::list<std::string> dependencies;
    std::list<std::string> modules;
    std::list<std::string> libraryModules;
    Plugin::Context* context;
};

/** Default destructor */
Plugin::~Plugin(){
    delete m_d;
}

/** Checks if live.plugin.json exists in the given path */
bool Plugin::existsIn(const std::string &path){
    QDir d(QString::fromStdString(path));
    if ( !d.exists() )
         return false;

    QFileInfo finfo(d.path() + "/"  + QString(Plugin::fileName));
    return finfo.exists();
}

/** Creates plugin from a given path */
Plugin::Ptr Plugin::createFromPath(const std::string &path){
    QString pluginPath = QString::fromStdString(path);
    QString pluginDirPath;

    QFileInfo finfo(pluginPath);
    if ( finfo.isDir() ){
        pluginPath = finfo.filePath() + "/" + QString(Plugin::fileName);
        pluginDirPath = finfo.filePath();
    } else {
        pluginDirPath = finfo.path();
    }

    std::ifstream instream(pluginPath.toStdString(), std::ifstream::in | std::ifstream::binary);
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

    return createFromNode(pluginDirPath.toStdString(), pluginPath.toStdString(), m);
}

/** Creates plugin from a given MLNode*/
Plugin::Ptr Plugin::createFromNode(const std::string &path, const std::string &filePath, const MLNode &m){
    if ( !m.hasKey("name") || !m.hasKey("package") )
        return Plugin::Ptr(nullptr);

    QString package = QString::fromStdString(m["package"].asString());

    QFileInfo finfo(package);
    if ( finfo.isRelative() ){
        package = QDir::cleanPath(QString::fromStdString(path) + "/" + package);
        finfo = QFileInfo(package);
    }

    if ( finfo.isDir() ){
        package = finfo.filePath() + "/" + QString(Package::fileName);
    }


    Plugin::Ptr pt(new Plugin(path, filePath, m["name"].asString(), package.toStdString()));


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

    return pt;
}

/** Create empty plugin */
Plugin::Ptr Plugin::createEmpty(const std::string &name){
    Plugin::Ptr pt(new Plugin(name, name, name, ""));
    return pt;
}

/** Name getter */
const std::string &Plugin::name() const{
    return m_d->name;
}

/** Path getter */
const std::string &Plugin::path() const{
    return m_d->path;
}

/** File path getter */
const std::string &Plugin::filePath() const{
    return m_d->filePath;
}

const std::string &Plugin::package() const{
    return m_d->package;
}

/** Dependencies getter */
const std::list<std::string> &Plugin::dependencies() const{
    return m_d->dependencies;
}

/** Modules getter */
const std::list<std::string> &Plugin::modules() const{
    return m_d->modules;
}

/** Library modules getter */
const std::list<std::string> &Plugin::libraryModules() const{
    return m_d->libraryModules;
}

/** Palettes getter */
const std::list<std::pair<std::string, std::string> > &Plugin::palettes() const{
    return m_d->palettes;
}

/** Assign package graph context */
void Plugin::assignContext(PackageGraph *graph){
    if ( m_d->context )
        delete m_d->context;

    m_d->context = new Plugin::Context;
    m_d->context->packageGraph = graph;

    if ( graph ){
        for ( auto it = m_d->palettes.begin(); it != m_d->palettes.end(); ++it ){
            addPalette(it->second, it->first);
        }
    }

}

/** Context getter */
Plugin::Context *Plugin::context(){
    return m_d->context;
}

Plugin::Plugin(const std::string &path, const std::string &filePath, const std::string &name, const std::string &package)
    : m_d(new PluginPrivate)
{
    m_d->path     = path;
    m_d->filePath = filePath;
    m_d->name     = name;
    m_d->package  = package;
    m_d->context  = nullptr;
}

void Plugin::addPalette(const std::string &type, const std::string &path){
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

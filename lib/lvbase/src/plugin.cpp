#include "plugin.h"
#include "plugincontext.h"

#include "lockedfileiosession.h"
#include "live/mlnodetojson.h"
#include "live/exception.h"
#include "live/package.h"
#include <list>
#include <map>

#include <fstream>
#include <istream>
#include <sstream>

#include <QDir>
#include <QFileInfo>


/**
  \class lv::Plugin
  \brief Manages a LiveCV plugin.
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
    std::map<std::string, std::string> palettes;
    std::list<std::string> dependencies;
    Plugin::Context* context;
};


Plugin::~Plugin(){
    delete m_d;
}

bool Plugin::existsIn(const std::string &path){
    QDir d(QString::fromStdString(path));
    if ( !d.exists() )
         return false;

    QFileInfo finfo(d.path() + "/"  + QString(Plugin::fileName));
    return finfo.exists();
}

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
    size_t size = instream.tellg();
    std::string buffer(size, ' ');
    instream.seekg(0);
    instream.read(&buffer[0], size);

    MLNode m;
    ml::fromJson(buffer, m);

    return createFromNode(pluginDirPath.toStdString(), pluginPath.toStdString(), m);
}

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
            pt->m_d->palettes[it->first] = it->second.asString();
        }
    }

    if ( m.hasKey("dependencies") ){
        MLNode::ArrayType dep = m["dependencies"].asArray();
        for ( auto it = dep.begin(); it != dep.end(); ++it ){
            pt->m_d->dependencies.push_back(it->asString());
        }
    }

    return pt;
}

const std::string &Plugin::name() const{
    return m_d->name;
}

const std::string &Plugin::path() const{
    return m_d->path;
}

const std::string &Plugin::filePath() const{
    return m_d->filePath;
}

const std::string &Plugin::package() const{
    return m_d->package;
}

const std::list<std::string> &Plugin::dependencies() const{
    return m_d->dependencies;
}

const std::map<std::string, std::string> &Plugin::palettes() const{
    return m_d->palettes;
}

void Plugin::assignContext(PackageGraph *graph){
    if ( m_d->context )
        delete m_d->context;

    m_d->context = new Plugin::Context;
    m_d->context->packageGraph = graph;
}

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


}// namespace

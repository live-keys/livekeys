#include "package.h"
#include "lockedfileiosession.h"
#include "live/mlnodetojson.h"
#include "live/exception.h"
#include <list>
#include <map>

#include <fstream>
#include <istream>
#include <sstream>

#include <QDir>
#include <QFileInfo>

/**
  \class lv::Package
  \brief Manages a LiveCV package.
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
    Version     version;
    std::string extension;
    std::map<std::string, Package::Dependency*> dependencies;
    std::map<std::string, Package::Library*>    libraries;
};

Package::~Package(){
    for ( auto it = m_d->dependencies.begin(); it != m_d->dependencies.end(); ++it )
        delete it->second;
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it )
        delete it->second;

    delete m_d;
}

bool Package::existsIn(const std::string &path){
    QDir d(QString::fromStdString(path));
    if ( !d.exists() )
         return false;

    QFileInfo finfo(d.path() + "/"  + QString::fromStdString(Package::fileName));
    return finfo.exists();
}

Package::Ptr Package::createFromPath(const std::string &path){
    QString packagePath = QString::fromStdString(path);
    QString packageDirPath;

    QFileInfo finfo(packagePath);
    if ( finfo.isDir() ){
        packagePath = finfo.filePath() + "/" + QString::fromStdString(Package::fileName);
        packageDirPath = finfo.filePath();
    } else {
        packageDirPath = finfo.path();
    }

    std::ifstream instream(packagePath.toStdString(), std::ifstream::in | std::ifstream::binary);
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

    return createFromNode(packageDirPath.toStdString(), packagePath.toStdString(), m);
}

Package::Ptr Package::createFromNode(const std::string& path, const std::string &filePath, const MLNode &m){
    if ( !m.hasKey("name") || !m.hasKey("version") )
        return Package::Ptr(nullptr);


    Package::Ptr pt(new Package(path, filePath, m["name"].asString(), Version(m["version"].asString())));

    if (m.hasKey("extension") ){
        pt->m_d->extension = m["extension"].asString();
        QFileInfo finfo(QString::fromStdString(path + "/" + pt->m_d->extension));
        if ( !finfo.exists() ){
            THROW_EXCEPTION(lv::Exception, std::string("Extension doesn\'t exist at path: ") + pt->m_d->extension, 2);
        }
    }

    if ( m.hasKey("dependencies") ){
        MLNode::ObjectType dep = m["dependencies"].asObject();
        for ( auto it = dep.begin(); it != dep.end(); ++it ){
            Package::Dependency* dep = new Package::Dependency(it->first, Version(it->second.asString()));
            pt->m_d->dependencies[dep->name] = dep;
        }
    }

    if ( m.hasKey("libraries") ){
        MLNode::ObjectType libs = m["libraries"].asObject();
        for ( auto it = libs.begin(); it != libs.end(); ++it ){
            MLNode libValue = it->second;

            Package::Library* lib = new Package::Library(it->first, Version(libValue["version"].asString()));
            if ( libValue.hasKey("flags") ){
                for ( auto it = libValue["flags"].asArray().begin(); it != libValue["flags"].asArray().end(); ++it ){
                    lib->flags.push_back(it->asString());
                }
            }
        }
    }

    return pt;
}

const std::string &Package::name() const{
    return m_d->name;
}

const std::string &Package::path() const{
    return m_d->path;
}

const std::string &Package::filePath() const{
    return m_d->filePath;
}

const Version &Package::version() const{
    return m_d->version;
}

const std::string &Package::extension() const{
    return m_d->extension;
}

std::string Package::extensionAbsolutePath() const{
    return m_d->path + "/" + m_d->extension;
}

bool Package::hasExtension() const{
    return !m_d->extension.empty();
}

const std::map<std::string, Package::Dependency *> Package::dependencies() const{
    return m_d->dependencies;
}

const std::map<std::string, Package::Library *> Package::libraries() const{
    return m_d->libraries;
}

Package::Package(const std::string &path, const std::string& filePath, const std::string &name, const Version &version)
    : m_d(new PackagePrivate)
{
    m_d->path = path;
    m_d->filePath = filePath;
    m_d->name = name;
    m_d->version = version;
}

} // namespace

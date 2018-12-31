#include "package.h"
#include "packagecontext.h"

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
    std::map<std::string, Package::Reference*> dependencies;
    std::map<std::string, Package::Library*>   libraries;

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
            Package::Reference* dep = new Package::Reference(it->first, Version(it->second.asString()));
            pt->m_d->dependencies[dep->name] = dep;
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

/** \brief Returns the package version */
const Version &Package::version() const{
    return m_d->version;
}

/** \brief Returns the package extension */
const std::string &Package::extension() const{
    return m_d->extension;
}

/** \brief Returns the absolute path concatenation of the path and extension */
std::string Package::extensionAbsolutePath() const{
    return m_d->path + "/" + m_d->extension;
}

/** \brief Returns an indicator that the extension exists */
bool Package::hasExtension() const{
    return !m_d->extension.empty();
}

/** \brief Returns a map of dependencies with string keys */
const std::map<std::string, Package::Reference *>& Package::dependencies() const{
    return m_d->dependencies;
}


/** \brief Returns a map of libraries with string keys */
const std::map<std::string, Package::Library *>& Package::libraries() const{
    return m_d->libraries;
}

/** \brief Assigns a new context to this package. */
void Package::assignContext(PackageGraph *graph){
    if ( m_d->context ){
        if ( m_d->context->packageGraph == graph )
            return;
        delete m_d->context;
    }

    m_d->context = new Package::Context;
    m_d->context->packageGraph = graph;
}

PackageGraph *Package::contextOwner(){
    if ( m_d->context )
        return m_d->context->packageGraph;
    return nullptr;
}

/** \brief Returns the current context if any has been assigned, nullptr otherwise. */
Package::Context *Package::context(){
    return m_d->context;
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

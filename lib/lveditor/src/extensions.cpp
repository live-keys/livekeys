#include "extensions.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/applicationcontext.h"

#include <QQmlComponent>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDebug>

/**
 * \class lv::Extensions
 * \brief Object used to store all the extensions of LiveKeys
 * \ingroup lvview
 */

namespace lv{

/**
 * \brief Default constructor
 */
Extensions::Extensions(ViewEngine *engine, const QString &settingsPath, QObject *parent)
    : QObject(parent)
    , m_globals(new QQmlPropertyMap(this))
    , m_engine(engine)
{
    m_path = QDir::cleanPath(settingsPath + "/workspace.json");
}

/** Default destructor */
Extensions::~Extensions(){
    delete m_globals;
}

/** Loads all the extensions available */
void Extensions::loadExtensions(){
    QFile file(m_path);

    MLNode extensionConfiguration;

    if ( !file.exists() ){
        const MLNode& startupcfg = ApplicationContext::instance().startupConfiguration();
        if ( startupcfg.hasKey("workspace") )
            extensionConfiguration = startupcfg["workspace"]["extensions"];

        if ( file.open(QIODevice::WriteOnly) ){
            std::string r;
            MLNode n(MLNode::Object);
            n["extensions"] = extensionConfiguration;

            ml::toJson(n, r);

            vlog("extensions").v() << "Saving extensions file to: " << m_path;

            file.write(r.c_str(), static_cast<int>(r.length()));
            file.close();
        }
    } else if ( file.open(QIODevice::ReadOnly) ){
        MLNode n;
        QByteArray fileContents = file.readAll();
        ml::fromJson(fileContents.data(), n);

        if ( n.hasKey("extensions") ){
            extensionConfiguration = n["extensions"];
        }
    }

    MLNode::ArrayType extensionArray = extensionConfiguration.asArray();

    for ( auto it = extensionArray.begin(); it != extensionArray.end(); ++it ){
        MLNode currentExt = *it;

        bool isEnabled = currentExt["enabled"].asBool();
        if ( isEnabled ){
            std::string packageName = currentExt["package"].asString();
            std::string packagePath = packageName;
            QFileInfo fpath(QString::fromStdString(packagePath));
            if ( fpath.isRelative() ){
                packagePath = ApplicationContext::instance().pluginPath() + "/" + packageName;
            }
            std::string component = currentExt["component"].asString();

            if ( Package::existsIn(packagePath) ){
                WorkspaceExtension* le = loadPackageExtension(packagePath, component);
                if ( le ){ // if package has extension
                    vlog("extensions").v() << "Loaded extension from package: " << packagePath;
                }
            }
        }
    }
}

/** Loads package extension from a given path */
WorkspaceExtension* Extensions::loadPackageExtension(const std::string &path, const std::string& component){
    Package::Ptr p = Package::createFromPath(path);
    if ( !p )
        return nullptr;
    return loadPackageExtension(p, component);
}

/** Loads package extension from a given package */
WorkspaceExtension *Extensions::loadPackageExtension(const Package::Ptr &package, const std::string& componentName){
    std::string path = package->path() + "/" + componentName;
    vlog("extensions").v() << "Loading extension: " << path;

    QQmlComponent component(m_engine->engine(), QUrl::fromLocalFile(QString::fromStdString(path)), this);
    if ( component.isError() ){
        THROW_EXCEPTION(lv::Exception, "Failed to load component: " + component.errorString().toStdString(), 4);
    }

    WorkspaceExtension* le = qobject_cast<WorkspaceExtension*>(component.create());
    if ( !le ){
        THROW_EXCEPTION(lv::Exception, "Extension failed to create or cast to LiveExtension type in: " + path, 3);
    }

    le->setIdentifiers(package->name(), path);
    le->setParent(this);
    m_extensions.insert(le->name(), le);
    m_globals->insert(QString::fromStdString(le->name()), QVariant::fromValue(le->globals()));

    return le;
}

} // namespace

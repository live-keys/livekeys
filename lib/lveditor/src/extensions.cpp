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
    m_path = QDir::cleanPath(settingsPath + "/extensions.json");
}

/** Default destructor */
Extensions::~Extensions(){
    delete m_globals;
}

/** Loads all the extensions available */
void Extensions::loadExtensions(){
    QFile file(m_path);
    if ( file.exists() && file.open(QIODevice::ReadOnly) ){
        vlog("extensions").v() << "Reading extensions file: " << m_path;

        MLNode n;
        QByteArray fileContents = file.readAll();
        ml::fromJson(fileContents.data(), n);
        if ( n.type() == MLNode::Object ){
            for ( auto it = n.begin(); it != n.end(); ++it ){
                QFileInfo fpath(QString::fromStdString(it.key()));

                std::string packagePath;
                if ( fpath.isRelative() ){
                    packagePath = ApplicationContext::instance().pluginPath() + "/" + it.key();
                } else {
                    packagePath = it.key();
                }

                loadPackageExtension(packagePath);

                vlog("extensions").v() << "Loaded extension:" << packagePath;
            }
        }
    } else {
        vlog("extensions").v() << "Acquiring installed extensions...";

        QDirIterator it(QString::fromStdString(ApplicationContext::instance().pluginPath()));
        while ( it.hasNext() ){
            QString path = it.next();
            if ( it.fileName() == "." || it.fileName() == ".." )
                continue;

            if ( Package::existsIn(path.toStdString()) ){
                WorkspaceExtension* le = loadPackageExtension(path.toStdString());
                if ( le ){ // if package has extension
                    vlog("extensions").v() << "Loaded extension from package: " << path;
                }
            }
        }

        MLNode n(MLNode::Object);
        for ( auto it = begin(); it != end(); ++it ){
            WorkspaceExtension* le = it.value();
            n[le->name()] = true;
        }
        std::string data;
        ml::toJson(n, data);

        vlog("extensions").v() << "Saving extensions file to: " << m_path;

        QFile file(m_path);
        if ( file.open(QIODevice::WriteOnly) ){
            file.write(data.c_str());
            file.close();
        }
    }
}
/** Loads package extension from a given path */
WorkspaceExtension* Extensions::loadPackageExtension(const std::string &path){
    Package::Ptr p = Package::createFromPath(path);
    if ( p->hasExtension() ){
        return loadPackageExtension(p);
    }
    return nullptr;
}

/** Loads package extension from a given package */
WorkspaceExtension *Extensions::loadPackageExtension(const Package::Ptr &package){
    std::string path = package->extensionAbsolutePath();
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

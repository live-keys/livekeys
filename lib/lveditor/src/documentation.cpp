#include "documentation.h"
#include "live/visuallogqt.h"
#include "live/packagegraph.h"
#include "live/applicationcontext.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "documentationloader.h"

namespace lv{

Documentation::Documentation(const PackageGraph *pg, QObject *parent)
    : QObject(parent)
    , m_packageGraph(pg)
    , m_defaultLoader(nullptr)
{
}

Documentation::~Documentation(){
}

QJSValue Documentation::load(const QString &language, const QString &path, const QString &detail){
    Package::ConstPtr package(nullptr);
    if ( !path.isEmpty() ){
        std::string packageNameStr = path.toStdString();
        package = m_packageGraph->findLoadedPackage(packageNameStr);
        if ( !package )
            package = m_packageGraph->findPackage(packageNameStr);
    }

    ViewEngine* engine = lv::ViewContext::instance().engine();

    DocumentationLoader* documentationLoader = nullptr;

    if ( package && !package->documentation().empty() ){
        QString loaderPath = QString::fromStdString(ApplicationContext::instance().pluginPath() + "/" + package->documentation());
        documentationLoader = createLoader(engine, loaderPath);
    } else {
        if ( !m_defaultLoader ){
            m_defaultLoader = createLoader(
                engine, QString::fromStdString(ApplicationContext::instance().pluginPath() + "/editor/loaddocs.qml")
            );
        }
        documentationLoader = m_defaultLoader;
    }

    if ( documentationLoader ){
        QJSValue loadFn = documentationLoader->load();
        if ( !loadFn.isCallable() ){
            lv::Exception e = CREATE_EXCEPTION(Exception, "Property `load` of DocumentationLoader is not a function.", Exception::toCode("~Function"));
            engine->throwError(&e, this);
            return QJSValue();
        }

        QJSValue loadFnResult = loadFn.call(QJSValueList() << language << path << detail);
        if ( loadFnResult.isError() ){
            engine->throwError(loadFnResult, this);
            return QJSValue();
        }

        return loadFnResult;
    }


    return QJSValue();
}

QJSValue Documentation::load(const QString &path){
    int languageIndex = path.indexOf("/");
    if ( languageIndex < 0 )
        return load(path, "", "");

    QString language = path.mid(0, languageIndex);
    QString pathTail = path.mid(languageIndex + 1);

    int index = pathTail.indexOf('.');
    int hashIndex = pathTail.indexOf('#');
    index = (index > 0 && (index < hashIndex || hashIndex < 0)) ? index : hashIndex;

    if ( index < 0 )
        return load(language, pathTail, pathTail);

    QString ns = pathTail.mid(0, index);

    return load(language, ns, pathTail);
}

DocumentationLoader *Documentation::createLoader(ViewEngine* engine, const QString &loaderPath){
    QQmlComponent component(engine->engine(), loaderPath);

    QList<QQmlError> errors = component.errors();
    if ( !errors.isEmpty() ){
        engine->throwError(engine->toJSError(errors.first()), this);
        return nullptr;
    }

    QObject* result = component.create();

    errors = component.errors();
    if ( !errors.isEmpty() ){
        engine->throwError(engine->toJSError(errors.first()), this);
        return nullptr;
    }

    if ( !result ){
        lv::Exception e = CREATE_EXCEPTION(Exception, "Null default documentation loader.", Exception::toCode("~Component"));
        engine->throwError(&e, this);
        return nullptr;
    }

    DocumentationLoader* resultType = static_cast<DocumentationLoader*>(result);
    return resultType;
}

}// namespace

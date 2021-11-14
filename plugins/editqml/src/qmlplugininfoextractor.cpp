#include "qmlplugininfoextractor.h"

#include "live/project.h"
#include "live/applicationcontext.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

#include "live/projectqmlscope.h"
#include "live/projectqmlextension.h"
#include "live/qmllanguagescanner.h"

#include <QQmlContext>

namespace lv{

QmlPluginInfoExtractor::QmlPluginInfoExtractor(QObject *parent)
    : QObject(parent)
    , m_languageScanner(nullptr)
    , m_engine(nullptr)
{
    ViewEngine* viewEngine = lv::ViewContext::instance().engine();
    m_engine = viewEngine->engine();

    m_languageScanner = new QmlLanguageScanner(viewEngine->fileIO(), m_engine->importPathList());
    m_languageScanner->enableFork(false);
    m_languageScanner->onMessage([this](int level, const QString& data){
        emit message(level, data);
    });
}

void QmlPluginInfoExtractor::setImportUri(const QString &importUri){
    if (!m_importUri.isEmpty() ){
        Exception e = CREATE_EXCEPTION(Exception, "QmlPluginInfoExtractor: importUri can only be set once.", Exception::toCode("~SingleSet"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    m_importUri = importUri;
    emit importUriChanged();

    if ( !ProjectQmlExtension::pluginTypesEnabled() ){
        Exception e = CREATE_EXCEPTION(Exception, "QmlPluginInfoExtractor: Plugin types was not enabled.", Exception::toCode("~QmlPluginTypes"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    QmlLibraryInfo::Ptr lib = ProjectQmlScope::findQmlLibraryInImports(m_engine->importPathList(), importUri, 1, 0);

    if ( !lib ){
        Exception e = CREATE_EXCEPTION(Exception, "QmlPluginInfoExtractor: importUri not found: " + importUri.toStdString(), Exception::toCode("~Found"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    m_languageScanner->queueLibrary(lib);
    m_languageScanner->processQueue();


    QByteArray stream;

    if ( QmlLibraryInfo::Done == ProjectQmlExtension::loadPluginInfoInternal(m_languageScanner, lib, m_engine, &stream) ){
        emit pluginInfoReady(QString::fromUtf8(stream));
    } else {
        Exception e = CREATE_EXCEPTION(Exception, "QmlPluginInfoExtractor: Failed to find imports for : " + importUri.toStdString(), Exception::toCode("~Imports"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
}

void QmlPluginInfoExtractor::printResult(const QString &data){
    QByteArray utf8 = data.toUtf8();
    printf("%s", utf8.data());
}

}// namespace

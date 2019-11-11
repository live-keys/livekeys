#include "qmlplugininfoextractor.h"

#include "live/project.h"
#include "live/applicationcontext.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallog.h"

#include "live/projectqmlextension.h"
#include "live/plugininfoextractor.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

QmlPluginInfoExtractor::QmlPluginInfoExtractor(QObject *parent)
    : QObject(parent)
    , m_qmlExtension(nullptr)
    , m_extractor(nullptr)
{
    Settings* settings = lv::ViewContext::instance().settings();
    ViewEngine* viewEngine = lv::ViewContext::instance().engine();
    QQmlEngine* engine = viewEngine->engine();
    QObject* project = engine->rootContext()->contextProperty("project").value<QObject*>();

    m_qmlExtension = new lv::ProjectQmlExtension();
    m_qmlExtension->setParams(settings, static_cast<lv::Project*>(project), viewEngine);

}

void QmlPluginInfoExtractor::setImportUri(const QString &importUri){
    if (!m_importUri.isEmpty() ){
        Exception e = CREATE_EXCEPTION(Exception, "QmlPluginInfoExtractor: importUri can only be set once.", Exception::toCode("~SingleSet"));
        ViewContext::instance().engine()->throwError(&e, this);
    }
    m_importUri = importUri;
    emit importUriChanged();

    m_extractor = m_qmlExtension->getPluginInfoExtractor(m_importUri);
    if ( m_extractor ){
        connect(m_extractor, &PluginInfoExtractor::resultReady, this, &QmlPluginInfoExtractor::__pluginInfoReady);
    }
}

void QmlPluginInfoExtractor::__pluginInfoReady(){
    emit pluginInfoReady(QString::fromUtf8(m_extractor->result()));
}

}// namespace

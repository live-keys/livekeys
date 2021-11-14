#include "componentsource.h"
#include <QQmlContext>
#include <QQuickItem>
#include <QQmlEngine>
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/documentqmlinfo.h"
#include "live/lockedfileiosession.h"
#include "live/project.h"
#include "live/mlnode.h"

namespace lv{

ComponentSource::ComponentSource(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
    , m_project(nullptr)
{
    m_project = qobject_cast<lv::Project*>(
        lv::ViewContext::instance().engine()->engine()->rootContext()->contextProperty("project").value<QObject*>()
    );
    if ( !m_project ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Failed to load 'project' property from context", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }
}

ComponentSource::~ComponentSource(){
}

void ComponentSource::componentComplete(){
    m_componentComplete = true;
    extractSource();
}

void ComponentSource::extractSource(){
    if ( !m_sourceCode.isEmpty() )
        return;

    QQmlContext* thisContext = qmlContext(this);
    if ( !thisContext ){
        lv::Exception lve = CREATE_EXCEPTION(lv::Exception, "Failed to find ComponentSource context.", 0);
        lv::ViewContext::instance().engine()->throwError(&lve, this);
        return;
    }

    QString thisPath = thisContext->baseUrl().toLocalFile();
    QString thisId = thisContext->nameForObject(this);

    if ( thisId.isEmpty() ){
        lv::Exception lve = CREATE_EXCEPTION(lv::Exception, "Each ComponentSource requires an id to be set.", 0);
        lv::ViewContext::instance().engine()->throwError(&lve, this);
        return;
    }

    DocumentQmlInfo::Ptr dqi = DocumentQmlInfo::create(thisPath);
    QString code = QString::fromStdString(lv::ViewContext::instance().engine()->fileIO()->readFromFile(thisPath.toStdString()));
    dqi->parse(code);
    dqi->createRanges();

    DocumentQmlInfo::ValueReference rootVr = dqi->rootObject();
    int rootTypeBegin, rootTypeEnd;
    dqi->extractTypeNameRange(rootVr, rootTypeBegin, rootTypeEnd);

    DocumentQmlInfo::ValueReference vr = dqi->valueForId(thisId);

    int begin, end;
    dqi->extractRange(vr, begin, end);
    if ( begin > -1 && end > 0){

        QString sourceData = code.mid(begin, end - begin);

        m_sourceCode = sourceData;

        if ( rootTypeBegin > -1 ){
            m_importSourceCode = code.mid(0, rootTypeBegin);
        }
    }
}

void ComponentSource::setSource(QQmlComponent *source){
    if ( m_componentComplete ){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "ComponentSource: Cannot set source after component is completed.", 0
        );
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }

    m_source = source;
    emit sourceChanged();
}



}// namespace

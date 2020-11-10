#include "worker.h"
#include "live/workerthread.h"
#include "live/qmlact.h"
#include "live/qmlstreamfilter.h"
#include "live/applicationcontext.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/documentqmlinfo.h"
#include "live/lockedfileiosession.h"
#include "live/project.h"
#include "live/mlnode.h"

#include <QQmlContext>
#include <QQuickItem>
#include <QQmlEngine>

namespace lv{

Worker::Worker(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
    , m_filterWorker(nullptr)
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

Worker::~Worker(){
    delete m_filterWorker;
}

QQmlListProperty<QObject> Worker::acts(){
    return QQmlListProperty<QObject>(this, this,
             &Worker::appendAct,
             &Worker::actCount,
             &Worker::act,
             &Worker::clearActs);
}

void Worker::appendAct(QObject * filter){
    if ( !m_filterWorker ){
        extractSource();
    }

    lv::QmlAct* f = dynamic_cast<lv::QmlAct*>(filter);
    if ( f ){
        //HERE
//        f->setWorkerThread(m_filterWorker);
        m_filterWorker->acts().append(f);
        return;
    }

    lv::QmlStreamFilter* sf = qobject_cast<QmlStreamFilter*>(filter);
    if ( sf ){
        sf->setWorkerThread(m_filterWorker);
        m_filterWorker->acts().append(sf);
        return;
    }

    lv::Exception lve = CREATE_EXCEPTION(
        lv::Exception, std::string("Object not of filter type: ") + filter->metaObject()->className(), 2
    );
    lv::ViewContext::instance().engine()->throwError(&lve, this);
}

int Worker::actCount() const{
    return m_filterWorker ? m_filterWorker->acts().size() : 0;
}

QObject *Worker::act(int index) const{
    return m_filterWorker ? dynamic_cast<QObject*>(m_filterWorker->acts().at(index)) : nullptr;
}

void Worker::clearActs(){
    if ( m_filterWorker )
        m_filterWorker->acts().clear();
}

void Worker::componentComplete(){
    m_componentComplete = true;
}

void Worker::extractSource(){
    if ( !m_actSource.isEmpty() )
        return;

    QQmlContext* thisContext = qmlContext(this);
    if ( !thisContext ){
        lv::Exception lve = CREATE_EXCEPTION(lv::Exception, "Failed to find Worker context.", 0);
        lv::ViewContext::instance().engine()->throwError(&lve, this);
        return;
    }

    QString thisPath = thisContext->baseUrl().toLocalFile();
    QString thisId = thisContext->nameForObject(this);

    if ( thisId.isEmpty() ){
        lv::Exception lve = CREATE_EXCEPTION(lv::Exception, "Each Worker requires an id to be set.", 0);
        lv::ViewContext::instance().engine()->throwError(&lve, this);
        return;
    }

    DocumentQmlInfo::Ptr dqi = DocumentQmlInfo::create(thisPath);
    QString code = QString::fromStdString(m_project->lockedFileIO()->readFromFile(thisPath.toStdString()));
    dqi->parse(code);
    dqi->createRanges();

    DocumentQmlInfo::ValueReference rootVr = dqi->rootObject();
    int rootTypeBegin, rootTypeEnd;
    dqi->extractTypeNameRange(rootVr, rootTypeBegin, rootTypeEnd);

    DocumentQmlInfo::ValueReference vr = dqi->valueForId(thisId);

    int begin, end;
    dqi->extractRange(vr, begin, end);
    if ( begin > -1 && end > 0){

        QString sourceData = "Worker" + code.mid(begin, end - begin);

        lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create("Worker.qml");
        if ( !docinfo->parse(sourceData) )
            return;

        lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
        lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

        for ( auto it = root->children.begin(); it != root->children.end(); ++it ){
            lv::DocumentQmlValueObjects::RangeObject* actChild = *it;
            for ( auto pit = actChild->properties.begin(); pit != actChild->properties.end(); ++pit ){
                lv::DocumentQmlValueObjects::RangeProperty* p = *pit;
                QString propertyName = sourceData.mid(p->begin, p->propertyEnd - p->begin);
                if ( propertyName == "run" ){
                    QString propertyContent = sourceData.mid(p->valueBegin, p->end - p->valueBegin);
                    m_actSource.append(propertyContent);
                }
            }
        }
    }

    m_filterWorker = new WorkerThread(m_actSource);
    m_filterWorker->start();
}

void Worker::appendAct(QQmlListProperty<QObject> *list, QObject *o){
    reinterpret_cast<Worker*>(list->data)->appendAct(o);
}

int Worker::actCount(QQmlListProperty<QObject> *list){
    return reinterpret_cast<Worker*>(list->data)->actCount();
}

QObject *Worker::act(QQmlListProperty<QObject> *list, int i){
    return reinterpret_cast<Worker*>(list->data)->act(i);
}

void Worker::clearActs(QQmlListProperty<QObject> *list){
    reinterpret_cast<Worker*>(list->data)->clearActs();
}

}// namespace

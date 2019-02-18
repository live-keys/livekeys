#include "worker.h"
#include "live/workerthread.h"
#include "live/act.h"
#include "live/applicationcontext.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/exception.h"

namespace lv{

Worker::Worker(QObject *parent)
    : QObject(parent)
    , m_filterWorker(new WorkerThread)
{
    m_filterWorker->start();
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
    lv::Act* f = dynamic_cast<lv::Act*>(filter);
    if ( f ){
        f->setWorkerThread(m_filterWorker);
        m_acts.append(f);
    } else {
        lv::Exception lve = CREATE_EXCEPTION(
            lv::Exception, std::string("Object not of filter type: ") + filter->metaObject()->className(), 2
        );
        lv::ViewContext::instance().engine()->throwError(&lve, this);
        return;
    }
}

int Worker::actCount() const{
    return m_acts.size();
}

QObject *Worker::act(int index) const{
    return dynamic_cast<QObject*>(m_acts.at(index));
}

void Worker::clearActs(){
    m_acts.clear();
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

#include "thread.h"
#include "live/filterworker.h"
#include "live/filter.h"
#include "live/applicationcontext.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/exception.h"

namespace lv{

Thread::Thread(QObject *parent)
    : QObject(parent)
    , m_filterWorker(new FilterWorker)
{
    m_filterWorker->start();
}

Thread::~Thread(){
    delete m_filterWorker;
}

QQmlListProperty<QObject> Thread::filters(){
    return QQmlListProperty<QObject>(this, this,
             &Thread::appendFilter,
             &Thread::filterCount,
             &Thread::filter,
             &Thread::clearFilters);
}

void Thread::appendFilter(QObject * filter){
    lv::Filter* f = dynamic_cast<lv::Filter*>(filter);
    if ( f ){
        f->setWorkerThread(m_filterWorker);
        m_filters.append(f);
    } else {
        lv::Exception lve = CREATE_EXCEPTION(
            lv::Exception, std::string("Object not of filter type: ") + filter->metaObject()->className(), 2
        );
        lv::ViewContext::instance().engine()->throwError(&lve, this);
        return;
    }
}

int Thread::filterCount() const{
    return m_filters.size();
}

QObject *Thread::filter(int index) const{
    return dynamic_cast<QObject*>(m_filters.at(index));
}

void Thread::clearFilters(){
    m_filters.clear();
}

void Thread::appendFilter(QQmlListProperty<QObject> *list, QObject *o){
    reinterpret_cast<Thread*>(list->data)->appendFilter(o);
}

int Thread::filterCount(QQmlListProperty<QObject> *list){
    return reinterpret_cast<Thread*>(list->data)->filterCount();
}

QObject *Thread::filter(QQmlListProperty<QObject> *list, int i){
    return reinterpret_cast<Thread*>(list->data)->filter(i);
}

void Thread::clearFilters(QQmlListProperty<QObject> *list){
    reinterpret_cast<Thread*>(list->data)->clearFilters();
}

}// namespace

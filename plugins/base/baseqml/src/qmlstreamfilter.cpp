#include "qmlstreamfilter.h"

#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/applicationcontext.h"
#include "live/workerthread.h"

namespace lv{

QmlStreamFilter::QmlStreamFilter(QObject* parent)
    : QObject(parent)
    , m_pull(nullptr)
    , m_result(new QmlWritableStream(this))
{
}

QmlStreamFilter::~QmlStreamFilter(){
}

void QmlStreamFilter::streamHandler(QObject *that, const QJSValue &val){
    QmlStreamFilter* sf = static_cast<QmlStreamFilter*>(that);
    sf->setCurrent(val);
}

QQmlListProperty<QObject> QmlStreamFilter::childObjects(){
    return QQmlListProperty<QObject>(this, this,
             &QmlStreamFilter::appendChildObject,
             &QmlStreamFilter::childObjectCount,
             &QmlStreamFilter::childObject,
             &QmlStreamFilter::clearChildObjects);
}

void QmlStreamFilter::appendChildObject(QObject *obj){
    m_childObjects.append(obj);
}

int QmlStreamFilter::childObjectCount() const{
    return m_childObjects.size();
}

QObject *QmlStreamFilter::childObject(int index) const{
    return m_childObjects.at(index);
}

void QmlStreamFilter::clearChildObjects(){
    m_childObjects.clear();
}

void QmlStreamFilter::appendChildObject(QQmlListProperty<QObject> *list, QObject *o){
    reinterpret_cast<QmlStreamFilter*>(list->data)->appendChildObject(o);
}

int QmlStreamFilter::childObjectCount(QQmlListProperty<QObject> *list){
    return reinterpret_cast<QmlStreamFilter*>(list->data)->childObjectCount();
}

QObject *QmlStreamFilter::childObject(QQmlListProperty<QObject> *list, int index){
    return reinterpret_cast<QmlStreamFilter*>(list->data)->childObject(index);
}

void QmlStreamFilter::clearChildObjects(QQmlListProperty<QObject> *list){
    reinterpret_cast<QmlStreamFilter*>(list->data)->clearChildObjects();
}

void QmlStreamFilter::setPull(QmlStream *pull){
    if (m_pull == pull)
        return;

    if ( m_pull )
        m_pull->forward(nullptr, nullptr);

    m_pull = pull;
    m_pull->forward(this, &QmlStreamFilter::streamHandler);

    emit pullChanged();
}

void QmlStreamFilter::setCurrent(const QJSValue &val){
    m_current = val;
    emit currentChanged();
}

}// namespace

#include "qtransformimage.h"

#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/qmlact.h"

#include <QQmlEngine>
#include <QQmlProperty>

QTransformImage::QTransformImage(QObject *parent)
    : QObject(parent)
    , m_result(QJSValue(QJSValue::NullValue))
{
}

void QTransformImage::appendObjectToList(QQmlListProperty<QObject> *list, QObject *object){
    auto that = reinterpret_cast<QTransformImage*>(list->data);
    lv::QmlAct* act = qobject_cast<lv::QmlAct*>(object);
    if ( act ){
        connect(act, &lv::QmlAct::resultChanged, that, &QTransformImage::__childResultChanged);
    }
    connect(object, &QObject::destroyed, that, &QTransformImage::__childDestroyed);
    that->m_children.append(object);
}

int QTransformImage::objectCount(QQmlListProperty<QObject> *list){
    auto that = reinterpret_cast<QTransformImage*>(list->data);
    return that->m_children.size();
}

QObject *QTransformImage::objectAt(QQmlListProperty<QObject> *list, int index){
    auto that = reinterpret_cast<QTransformImage*>(list->data);
    return that->m_children.at(index);
}

void QTransformImage::clearObjects(QQmlListProperty<QObject> *list){
    auto that = reinterpret_cast<QTransformImage*>(list->data);
    QList<QObject*>& children = that->m_children;
    for ( QObject* ob: children ){
        disconnect(ob, &QObject::destroyed, that, &QTransformImage::__childDestroyed);

        lv::QmlAct* act = qobject_cast<lv::QmlAct*>(ob);
        if ( act ){
            disconnect(act, &lv::QmlAct::resultChanged, that, &QTransformImage::__childResultChanged);
        }
        if ( QQmlEngine::objectOwnership(ob) == QQmlEngine::CppOwnership ){
            ob->deleteLater();
        }
    }
    that->m_children.clear();

    that->run();
}

void QTransformImage::run(){
    lv::ViewEngine* engine = lv::ViewEngine::grab(this);

    if ( !m_children.length() ){
        m_result = engine->engine()->newQObject(m_input);
        emit resultChanged();
        return;
    }

    QQmlProperty pp(m_children[0], "input");
    pp.write(QVariant::fromValue(m_input));

    for ( int i = 1; i < m_children.size(); ++i ){
        QQmlProperty ppresult(m_children[i - 1], "result");
        QQmlProperty ppin(m_children[i], "input");

        QJSValue val = ppresult.read().value<QJSValue>();
        ppin.write(QVariant::fromValue(val.toQObject()));
    }

    QQmlProperty pplast(m_children.last(), "result");
    m_result = pplast.read().value<QJSValue>();
    emit resultChanged();
}

void QTransformImage::__childDestroyed(){
    m_children.removeOne(sender());
    run();
}

void QTransformImage::__childResultChanged(){
    //TODO: Optimize: start from that child onward only
    lv::ViewEngine* engine = lv::ViewEngine::grab(this);

    if ( !m_children.length() ){
        m_result = engine->engine()->newQObject(m_input);
        emit resultChanged();
        return;
    }

    QQmlProperty pp(m_children[0], "input");
    pp.write(QVariant::fromValue(m_input));

    for ( int i = 1; i < m_children.size(); ++i ){
        QQmlProperty ppresult(m_children[i - 1], "result");
        QQmlProperty ppin(m_children[i], "input");

        QJSValue val = ppresult.read().value<QJSValue>();
        ppin.write(QVariant::fromValue(val.toQObject()));
    }

    QQmlProperty pplast(m_children.last(), "result");
    m_result = pplast.read().value<QJSValue>();
    emit resultChanged();
}

QQmlListProperty<QObject> QTransformImage::items(){
    return QQmlListProperty<QObject>(
        this,
        this,
        &QTransformImage::appendObjectToList,
        &QTransformImage::objectCount,
        &QTransformImage::objectAt,
        &QTransformImage::clearObjects
    );
}

QJSValue QTransformImage::result() const{
    return m_result;
}


void QTransformImage::setInput(QObject *input){
    if (m_input == input)
        return;

    m_input = input;
    emit inputChanged();

    run();
}

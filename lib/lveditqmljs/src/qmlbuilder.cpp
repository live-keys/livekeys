#include "qmlbuilder.h"

#include <QQmlContext>
#include <QQmlEngine>

#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/visuallogqt.h"
#include "live/hookcontainer.h"

namespace lv{

QmlBuilder::QmlBuilder(QQuickItem *parent)
    : QQuickItem(parent)
{
}

QmlBuilder::~QmlBuilder(){
}

void QmlBuilder::setSource(const QString &source){
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged();
}

void QmlBuilder::__updateImplicitWidth(){
    QQuickItem* item = qobject_cast<QQuickItem*>(m_object);
    if ( item ){
        setImplicitWidth(item->width());
    }
}

void QmlBuilder::__updateImplicitHeight(){
    QQuickItem* item = qobject_cast<QQuickItem*>(m_object);
    if ( item ){
        setImplicitHeight(item->height());
    }
}

void QmlBuilder::componentComplete(){
    QQuickItem::componentComplete();

    QQmlContext* ctx = qmlContext(this);

    QString id = ctx->nameForObject(this);
    QString url = ctx->baseUrl().toString();

    HookContainer* hk = qobject_cast<HookContainer*>(ctx->contextProperty("hooks").value<QObject*>());
    hk->insertKey(m_source, id, this);

    QQmlEngine* engine = qmlEngine(this);

    QQmlComponent component(engine);
    component.loadUrl(QUrl::fromLocalFile(m_source));

    QList<QQmlError> errors = component.errors();
    if ( errors.size() ){
        qDebug() << "HAVE ERRORS";
//        emit runError(m_viewEngine->toJSErrors(errors));
        return;
    }

    m_object = component.create(ctx);
    errors = component.errors();
    if ( errors.size() ){
        qDebug() << "HAVE ERRORS";
//        emit runError(m_viewEngine->toJSErrors(errors));
        return;
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(m_object);
    if ( m_object )
        m_object->setParent(this);
    if ( item ){
        item->setParentItem(this);
        setImplicitHeight(item->height());
        setImplicitWidth(item->width());
        connect(item, &QQuickItem::widthChanged, this, &QmlBuilder::__updateImplicitWidth);
        connect(item, &QQuickItem::heightChanged, this, &QmlBuilder::__updateImplicitHeight);
    }

    emit objectChanged();
}

void QmlBuilder::setSourceComponent(QQmlComponent *sourceComponent){
    if (m_sourceComponent == sourceComponent)
        return;

    m_sourceComponent = sourceComponent;
    emit sourceComponentChanged();
}

}// namespace

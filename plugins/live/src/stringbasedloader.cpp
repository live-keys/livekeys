#include "stringbasedloader.h"

#include "live/exception.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

StringBasedLoader::StringBasedLoader(QQuickItem *parent)
    : QQuickItem(parent)
    , m_sourceComponent(0)
    , m_item(0)
{
}

StringBasedLoader::~StringBasedLoader(){
    if ( m_item ){
        delete m_item;
        m_item = 0;
    }
}

void StringBasedLoader::setSource(const QString &source){
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged();

    createObject();
}

void StringBasedLoader::createObject(){
    if ( m_source.isEmpty() )
        return;

    if ( m_item )
        delete m_item;
    if ( m_sourceComponent )
        delete m_sourceComponent;

    QQmlContext* ctx = qmlContext(this);

    m_sourceComponent = new QQmlComponent(qmlEngine(this), this);
    m_sourceComponent->setData(m_source.toUtf8(), ctx->baseUrl());
    if ( m_sourceComponent->isError() ){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "Failed to compile component: " + m_sourceComponent->errorString().toStdString(), 0
        );
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }

    m_item = m_sourceComponent->create(ctx);
    if ( !m_item ){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "Failed to create component object: " + ctx->baseUrl().toLocalFile().toStdString(), 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return;
    }

    m_item->setParent(this);
    QQuickItem* asitem = qobject_cast<QQuickItem*>(m_item);
    if ( asitem ){
        asitem->setParentItem(this);
    }
    qmlEngine(this)->setObjectOwnership(m_item, QQmlEngine::CppOwnership);

    emit itemChanged();
}

}// namespace

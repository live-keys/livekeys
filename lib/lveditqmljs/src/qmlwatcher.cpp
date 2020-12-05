#include "qmlwatcher.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQmlEngine>

#include <QThread>

#include "live/hookcontainer.h"
#include "live/project.h"
#include "live/projectdocument.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"
#include "live/componentdeclaration.h"

namespace lv{

QmlWatcher::QmlWatcher(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
{
}

QmlWatcher::~QmlWatcher(){
}

void QmlWatcher::componentComplete(){
    m_componentComplete = true;

    ViewEngine* ve = ViewEngine::grab(this);

    QQmlContext* ctx = qmlContext(this);
    m_id = ctx->nameForObject(parent());

    if ( m_id.isEmpty() )
        return;

    ComponentDeclaration cd = ve->rootDeclaration(parent());
    m_file = cd.url().toLocalFile();

    HookContainer* hk = qobject_cast<HookContainer*>(ctx->contextProperty("hooks").value<QObject*>());
    hk->insertKey(m_file, m_id, this);

    emit ready();
}

}// namespace

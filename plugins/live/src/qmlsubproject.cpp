#include "qmlsubproject.h"
#include "live/project.h"
#include "live/runnable.h"
#include "live/runnablecontainer.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

QmlSubproject::QmlSubproject(QQuickItem *parent)
    : QQuickItem(parent)
    , m_runnable(nullptr)
    , m_sourceComponent(nullptr)
    , m_label("")
    , m_project(nullptr)
    , m_sourceMonitor(false)
{
}

QmlSubproject::~QmlSubproject(){
    if ( m_runnable ){
        m_project->runnables()->closeRunnable(m_runnable);
    }
}

void QmlSubproject::_runnableDestroyed(){
    m_runnable = nullptr;
}

void QmlSubproject::componentComplete(){
    QQuickItem::componentComplete();

    ViewEngine* engine = lv::ViewContext::instance().engine();

    m_project = qobject_cast<Project*>(
        engine->engine()->rootContext()->contextProperty("project").value<QObject*>()
    );

    if ( m_sourceComponent ){
        m_runnable = new Runnable(engine, m_sourceComponent, m_project->runnables(), m_label);
        m_runnable->setRunSpace(this);

        if ( m_label.isEmpty() ){
            QString label;
            label.sprintf("%8p", (void*)m_runnable);
            setLabel("S: " + label);
            m_runnable->setName("S: " + label);
        }

        m_project->runnables()->addRunnable(m_runnable);

        connect(m_runnable, &Runnable::objectReady, this, &QmlSubproject::rootChanged);
        connect(m_runnable, &Runnable::destroyed, this, &QmlSubproject::_runnableDestroyed);

        m_runnable->run();

    } else if ( !m_source.isEmpty() ){
        QSet<QString> activations;
        if ( sourceMonitor() )
            activations.insert(m_source);

        m_runnable = new Runnable(engine, m_source, m_project->runnables(), m_label, activations);
        m_runnable->setRunSpace(this);

        if ( m_label.isEmpty() ){
            QString label;
            label.sprintf("%8p", (void*)m_runnable);
            setLabel("S: " + label);
            m_runnable->setName("S: " + label);
        }

        m_project->runnables()->addRunnable(m_runnable);

        connect(m_runnable, &Runnable::objectReady, this, &QmlSubproject::rootChanged);
        connect(m_runnable, &Runnable::destroyed, this, &QmlSubproject::_runnableDestroyed);

        m_runnable->run();
    }
}


QObject* QmlSubproject::root() const{
    return m_runnable ? m_runnable->appRoot() : nullptr;
}

}// namespace

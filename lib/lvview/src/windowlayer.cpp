#include "windowlayer.h"
#include "live/visuallog.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include <QFile>
#include <QUrl>
#include <QQuickWindow>
#include <QQuickItem>

#include <QDebug>

namespace lv{

WindowLayer::WindowLayer(QObject *parent)
    : Layer(parent)
{
    setHasView(true);
}

WindowLayer::~WindowLayer(){
}

void WindowLayer::loadView(ViewEngine* engine, QObject *parent){

    QString path = ":/window.qml";

    QFile f(path);
    if ( !f.open(QFile::ReadOnly) )
        THROW_EXCEPTION(Exception, "Failed to read layer view file:" + path.toStdString(), Exception::toCode("~File"));

    QByteArray contentBytes = f.readAll();

    //TODO: loadAsync, connect engine to loading slot, disconnect after
    QObject* layerObj = engine->createObject(contentBytes, parent, QUrl(path));
    if ( !layerObj && engine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(engine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    QQuickWindow* w = qobject_cast<QQuickWindow*>(layerObj);

    m_nextViewParent = w->contentItem();

    //TODO: Extract properties, i.e. dialogs, etc

    emit viewReady(this, m_nextViewParent);
}

QObject *WindowLayer::nextViewParent(){
    return m_nextViewParent;
}

}// namespace

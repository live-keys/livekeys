#include "workspacelayer.h"
#include "live/visuallog.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include <QFile>
#include <QUrl>

#include <QQuickItem>

#include <QDebug>

namespace lv{

WorkspaceLayer::WorkspaceLayer(QObject *parent)
    : Layer(parent)
{
    setHasView(true);
}

WorkspaceLayer::~WorkspaceLayer(){
}

void WorkspaceLayer::loadView(ViewEngine *engine, QObject *parent){

    QString path = ":/workspace.qml";

    QFile f(path);
    if ( !f.open(QFile::ReadOnly) )
        THROW_EXCEPTION(Exception, "Failed to read layer view file:" + path.toStdString(), Exception::toCode("~File"));

    QByteArray contentBytes = f.readAll();

    QObject* layerObj = engine->createObject(contentBytes, parent, QUrl(path));
    if ( !layerObj && engine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(engine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    m_nextViewParent = layerObj;

    emit viewReady(this, m_nextViewParent);
}

QObject *WorkspaceLayer::nextViewParent(){
    return m_nextViewParent;
}

}// namespace

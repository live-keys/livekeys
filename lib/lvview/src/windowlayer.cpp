#include "windowlayer.h"
#include "live/visuallog.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include "qmlclipboard.h"

#include <QFile>
#include <QUrl>
#include <QQuickWindow>
#include <QQuickItem>

namespace lv{

WindowLayer::WindowLayer(QObject *parent)
    : Layer(parent)
    , m_window(nullptr)
    , m_nextViewParent(nullptr)
    , m_handle(nullptr)
    , m_dialogs(nullptr)
    , m_clipboard(nullptr)
{
    setHasView(true);
}

WindowLayer::~WindowLayer(){
    delete m_clipboard;
}

void WindowLayer::loadView(ViewEngine* engine, QObject *parent){
    QString path = ":/window.qml";

    QFile f(path);
    if ( !f.open(QFile::ReadOnly) )
        THROW_EXCEPTION(Exception, "Failed to read layer view file:" + path.toStdString(), Exception::toCode("~File"));

    QByteArray contentBytes = f.readAll();

    //TODO: loadAsync, connect engine to loading slot, disconnect after
    QObject* layerObj = engine->createObject(contentBytes, parent, QUrl("qrc:/window.qml"));
    if ( !layerObj && engine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(engine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    m_window = qobject_cast<QQuickWindow*>(layerObj);

    connect(m_window, &QQuickWindow::activeChanged, this, &WindowLayer::windowActiveChanged);

    m_nextViewParent = m_window->property("container").value<QObject*>();

    m_dialogs = m_window->property("dialogs").value<QObject*>();
    m_handle  = m_window->property("handle").value<QObject*>();

    emit dialogsChanged();
    emit handleChanged();

    emit viewReady(this, m_nextViewParent);
}

QObject *WindowLayer::nextViewParent(){
    return m_nextViewParent;
}

void WindowLayer::windowActiveChanged(){
    emit isActiveChanged(m_window->isActive());
}

QmlClipboard *WindowLayer::clipboard(){
    if ( !m_clipboard ){
        m_clipboard = new QmlClipboard(this);
    }
    return m_clipboard;
}

}// namespace

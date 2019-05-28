#include "workspacelayer.h"
#include "live/visuallog.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/settings.h"
#include "live/liveextension.h"

#include <QFile>
#include <QUrl>

#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlPropertyMap>
#include <QJSValueIterator>
#include <QJSValue>

namespace lv{

WorkspaceLayer::WorkspaceLayer(QObject *parent)
    : Layer(parent)
    , m_project(nullptr)
    , m_panes(nullptr)
    , m_commands(new Commands)
    , m_keymap(nullptr)
    , m_extensions(nullptr)
{
    Settings* settings = lv::ViewContext::instance().settings();

    m_keymap = new KeyMap(settings->path());
    settings->addConfigFile("keymap", m_keymap);
    m_commands->setModel(new CommandsModel(m_commands, m_keymap));

    m_extensions = new Extensions(lv::ViewContext::instance().engine(), settings->path());
    settings->addConfigFile("extensions", m_extensions);

    setHasView(true);
}

WorkspaceLayer::~WorkspaceLayer(){
}

void WorkspaceLayer::loadView(ViewEngine *engine, QObject *parent){

    m_extensions->loadExtensions();

    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        LiveExtension* le = it.value();
        m_commands->add(le, le->commands());
        m_keymap->store(le->keyBindings());
    }

    QString path = ":/workspace.qml";

    QFile f(path);
    if ( !f.open(QFile::ReadOnly) )
        THROW_EXCEPTION(Exception, "Failed to read layer view file:" + path.toStdString(), Exception::toCode("~File"));

    QByteArray contentBytes = f.readAll();

    QObject* layerObj = engine->createObject(contentBytes, parent, QUrl("qrc:/workspace.qml"));
    if ( !layerObj && engine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(engine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    m_project = layerObj->property("projectEnvironment").value<QObject*>();
    m_panes   = layerObj->property("panes").value<QObject*>();
    m_nextViewParent = layerObj->property("runSpace").value<QObject*>();

    m_keymap->store(0, Qt::Key_O,           lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.project.openFile");
    m_keymap->store(0, Qt::Key_Backslash,   lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.project.toggleVisibility");
    m_keymap->store(0, Qt::Key_T,           lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleMaximizedRuntime");
    m_keymap->store(0, Qt::Key_K,           lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleNavigation");
    m_keymap->store(0, Qt::Key_L,           lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleLog");

    emit viewReady(this, m_nextViewParent);
}

QObject *WorkspaceLayer::nextViewParent(){
    return m_nextViewParent;
}

QJSValue WorkspaceLayer::interceptMenu(QJSValue context){
    QJSValueList interceptorArgs;
    interceptorArgs << context;

    QJSValueList result;

    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        LiveExtension* le = it.value();
        if ( le->hasMenuInterceptor() ){
            QJSValue v = le->callMenuInterceptor(interceptorArgs);
            if ( v.isArray() ){
                QJSValueIterator it(v);
                while ( it.hasNext() ){
                    it.next();
                    if ( it.name() != "length" ){
                        result << it.value();
                    }
                }
            }
        }
    }

    QJSValue concat = lv::ViewContext::instance().engine()->engine()->newArray(result.length());
    int index = 0;
    for ( auto it = result.begin(); it != result.end(); ++it ){
        concat.setProperty(index, *it);
        ++index;
    }

    return concat;
}

}// namespace

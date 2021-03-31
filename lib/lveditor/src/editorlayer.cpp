#include "editorlayer.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/keymap.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlPropertyMap>

namespace lv{

EditorLayer::EditorLayer(QObject *parent)
    : Layer(parent)
    , m_environment(nullptr)
{
    QQmlEngine* engine = lv::ViewContext::instance().engine()->engine();
    QObject* livekeys = engine->rootContext()->contextProperty("lk").value<QObject*>();
    QObject* workspace = livekeys->property("layers").value<QQmlPropertyMap*>()->property("workspace").value<QObject*>();

    if ( !workspace ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "Editor layer requires workspace layer.", Exception::toCode("~layer"));
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    lv::KeyMap* keymap = static_cast<lv::KeyMap*>(workspace->property("keymap").value<QObject*>());
    if ( keymap ){
//        keymap->store(0, Qt::Key_BraceLeft,   lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Shift, "window.workspace.addHorizontalEditorView");
//        keymap->store(0, Qt::Key_BracketLeft, lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Shift, "window.workspace.addHorizontalEditorView");
//        keymap->store(0, Qt::Key_BraceLeft,   lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Alt,   "window.workspace.addHorizontalFragmentEditorView");
//        keymap->store(0, Qt::Key_BracketLeft, lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Alt,   "window.workspace.addHorizontalFragmentEditorView");
//        keymap->store(0, Qt::Key_BraceRight,  lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Shift, "window.workspace.removeHorizontalEditorView");
//        keymap->store(0, Qt::Key_BracketRight,lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Shift, "window.workspace.removeHorizontalEditorView");

        keymap->store(0, Qt::Key_S,           lv::KeyMap::CONTROL_OR_COMMAND, "editor.saveFile");
        keymap->store(0, Qt::Key_S,           lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Alt, "editor.saveFileAs");
        keymap->store(0, Qt::Key_W,           lv::KeyMap::CONTROL_OR_COMMAND, "editor.closeFile");
        keymap->store(0, Qt::Key_E,           lv::KeyMap::CONTROL_OR_COMMAND, "editor.toggleSize");
        keymap->store(0, Qt::Key_Slash,       lv::KeyMap::CONTROL_OR_COMMAND, "editor.toggleComment");
        keymap->store(lv::KeyMap::Linux,    Qt::Key_Space, lv::KeyMap::Control, "editor.assistCompletion");
        keymap->store(lv::KeyMap::Windows,  Qt::Key_Space, lv::KeyMap::Control, "editor.assistCompletion");
        keymap->store(lv::KeyMap::Mac,      Qt::Key_Space, lv::KeyMap::Alt,     "editor.assistCompletion");
    }
}

EditorLayer::~EditorLayer(){
}

void EditorLayer::setEnvironment(QObject *environment){
    if ( m_environment == environment )
        return;

    m_environment = environment;
    emit environmentChanged();
}

}// namespace

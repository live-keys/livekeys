#include "languagelvextension.h"
#include "languagelvhandler.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/settings.h"
#include "live/viewengine.h"

#include "live/editorsettings.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

LanguageLvExtension::LanguageLvExtension(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
    , m_scanMonitor(nullptr)
{
}

LanguageLvExtension::~LanguageLvExtension(){
    for ( auto it = m_codeHandlers.begin(); it != m_codeHandlers.end(); ++it ){
        LanguageLvHandler* lh = *it;
        lh->resetExtensionObject();
    }
    m_codeHandlers.clear();
}

void LanguageLvExtension::componentComplete(){
    if ( !m_scanMonitor ){
        QQmlContext* ctx = qmlEngine(this)->rootContext();
        QObject* lg = ctx->contextProperty("lk").value<QObject*>();
        if ( !lg ){
            qWarning("Failed to find live global object.");
            return;
        }

        ViewEngine* engine = static_cast<ViewEngine*>(lg->property("engine").value<QObject*>());
        if ( !engine ){ qWarning("Failed to find engine object."); return; }

        Settings* settings = static_cast<Settings*>(lg->property("settings").value<QObject*>());
        if ( !settings ){ qWarning("Failed to find settings object."); return; }

        Project* project = static_cast<Project*>(ctx->contextProperty("project").value<QObject*>());
        if ( !project ){ qWarning("Failed to find project object."); return; }

        m_project = project;
        m_engine = engine;

        m_scanMonitor = new ProjectLvMonitor(this, m_project, m_engine);

        lv::EditorSettings* editorSettings = qobject_cast<lv::EditorSettings*>(settings->file("editor"));

        m_settings = new EditLvSettings(editorSettings);

        editorSettings->write("lv", m_settings->toJson());
        editorSettings->syncWithFile();
    }
}

void LanguageLvExtension::addLanguageHandler(LanguageLvHandler *handler){
    m_codeHandlers.append(handler);
}

void LanguageLvExtension::removeLanguageHandler(LanguageLvHandler *handler){
    m_codeHandlers.removeAll(handler);
}

QObject *LanguageLvExtension::createHandler(ProjectDocument *document, DocumentHandler *handler){
    return new LanguageLvHandler(m_engine, m_project, m_settings, this, document, handler);
}

}// namespace

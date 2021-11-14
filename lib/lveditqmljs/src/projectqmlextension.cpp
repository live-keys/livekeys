/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "projectqmlextension.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/settings.h"
#include "live/viewengine.h"
#include "live/languageqmlhandler.h"
#include "live/editorsettings.h"
#include "live/editorlayer.h"

#include "qmlbuilder.h"
#include "qmlwatcher.h"
#include "qmljssettings.h"
#include "qmlprojectmonitor_p.h"
#include "qmladdcontainer.h"
#include "qmleditfragment.h"
#include "qmleditfragmentcontainer.h"
#include "documentqmlchannels.h"
#include "qmlbindingchannelsdispatcher.h"
#include "qmltokenizer_p.h"
#include "qmlsyntax_p.h"
#include "qmlmetainfo_p.h"
#include "qmlmetatypeinfo_p.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlPropertyMap>

namespace lv{

static QObject* qmlTokenizerProvider(QQmlEngine* engine, QJSEngine*){
    return new QmlTokenizer(engine);
}

static QObject* qmlMetaInfoProvider(QQmlEngine* engine, QJSEngine*){
    return new QmlMetaInfo(engine);
}

static QObject* qmlSyntaxProvider(QQmlEngine* engine, QJSEngine*){
    return new QmlSyntax(engine);
}


/**
 * \class lv::ProjectQmlExtension
 * \ingroup lveditqmljs
 * \brief Main object used throughout qml extension plugin.
 *
 * To capture the object, you can use the workspace layer:
 *
 * \code
 * var projectQmlExtension = lk.layers.workspace.extensions["editqml"].globals
 * \endcode
 */


/**
 * \brief ProjectQmlExtension constructor with a \p parent parameter
 */
ProjectQmlExtension::ProjectQmlExtension(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
    , m_scanMonitor(nullptr)
    , m_paletteContainer(nullptr)
    , m_channelDispatcher(nullptr)
{
}

/**
 * @brief ProjectQmlExtension destructor
 */
ProjectQmlExtension::~ProjectQmlExtension(){
    for ( auto it = m_languageHandlers.begin(); it != m_languageHandlers.end(); ++it ){
        LanguageQmlHandler* cqh = *it;
        cqh->resetProjectQmlExtension();
    }
    m_languageHandlers.clear();
}

/**
 * @brief Override of QQmlParserStatus::classBegin
 */
void ProjectQmlExtension::classBegin(){
}

/// \brief Returns the lv::PaletteContainer associated with this object.
PaletteLoader *ProjectQmlExtension::paletteContainer(){
    if( !m_paletteContainer ){
        QObject* lk = m_engine->engine()->rootContext()->contextProperty("lk").value<QObject*>();
        if ( !lk ){
            return nullptr;
        }
        QObject* editorLayerOb = lk->property("layers").value<QQmlPropertyMap*>()->property("editor").value<QObject*>();
        EditorLayer* editorLayer = static_cast<EditorLayer*>(editorLayerOb);
        if ( !editorLayer ){
            return nullptr;
        }
        m_paletteContainer = editorLayer->paletteLoader();
    }
    return m_paletteContainer;
}

/**
 * @brief Override of QQmlParserStatus::componentComplete
 */
void ProjectQmlExtension::componentComplete(){
    if ( !m_scanMonitor ){
        QQmlEngine* qmlengine = qmlEngine(this);
        QQmlContext* ctx = qmlengine->rootContext();
        QObject* lk = ctx->contextProperty("lk").value<QObject*>();
        if ( !lk ){
            qWarning("Failed to find live global object.");
            return;
        }

        ViewEngine* engine = ViewEngine::grabFromQmlEngine(qmlengine);
        if ( !engine ){ qWarning("Failed to find engine object."); return; }

        Settings* settings = static_cast<Settings*>(lk->property("settings").value<QObject*>());
        if ( !settings ){ qWarning("Failed to find settings object."); return; }


        QObject* workspaceLayerOb = lk->property("layers").value<QQmlPropertyMap*>()->property("workspace").value<QObject*>();
        Project* project = workspaceLayerOb ? qobject_cast<lv::Project*>(workspaceLayerOb->property("project").value<QObject*>()) : nullptr;
        if ( !project ){ qWarning("Failed to find project object."); return; }

        Workspace* workspace = Workspace::getFromContext(ctx);
        if ( !workspace ){ qWarning("Failed to find workspace object."); return; }

        m_project = project;
        m_engine = engine;

        m_scanMonitor = new QmlProjectMonitor(this, m_project, m_engine, workspace);

        lv::EditorSettings* editorSettings = qobject_cast<lv::EditorSettings*>(settings->file("editor"));
        m_settings = new QmlJsSettings(editorSettings);
        editorSettings->syncWithFile();

        m_channelDispatcher = new QmlBindingChannelsDispatcher(m_project, this);
    }
}

/**
 * \brief Adds a codeHandler to this object.
 *
 * CodeHandlers are updated each time the engine recompiles the code.
 */
void ProjectQmlExtension::addLanguageQmlHandler(LanguageQmlHandler *handler){
    m_languageHandlers.append(handler);
}

/**
 * \brief Removes the \p handler from this object.
 *
 * Note that this does not destroy the object.
 */
void ProjectQmlExtension::removeLanguageQmlHandler(LanguageQmlHandler *handler){
    m_languageHandlers.removeAll(handler);
}

/**
 * \brief Registers the ProjectQmlExtension qml types to the \p uri.
 */
void ProjectQmlExtension::registerTypes(const char *uri){
    qmlRegisterType<lv::ProjectQmlExtension>(uri, 1, 0, "ProjectQmlExtension");
    qmlRegisterType<lv::QmlWatcher>(         uri, 1, 0, "Watcher");
    qmlRegisterType<lv::QmlBuilder>(         uri, 1, 0, "Builder");

    qmlRegisterUncreatableType<lv::QmlEditFragment>(
        uri, 1, 0, "QmlEditFragment", "QmlEditFragment can be created through the Editor.code.language.");
    qmlRegisterUncreatableType<lv::LanguageQmlHandler>(
        uri, 1, 0, "LanguageQmlHandler", "LanguageQmlHandler can only be accessed through the Editor.code");
    qmlRegisterUncreatableType<lv::QmlEditFragmentContainer>(
        uri, 1, 0, "QmlEditFragmentContainer", "QmlEditFragmentContainer can only be accessed through the Editor.code.language.editContainer");
    qmlRegisterUncreatableType<lv::DocumentQmlChannels>(
        uri, 1, 0, "DocumentQmlChannels", "DocumentQmlChannels can only be accessed through the Editor.code.language.bindingChannels");
    qmlRegisterUncreatableType<lv::QmlAddContainer>(
        uri, 1, 0, "QmlAddContainer", "QmlAddContainer can only be accessed through the qmledit extension.");
    qmlRegisterUncreatableType<lv::QmlSuggestionModel>(
        uri, 1, 0, "QmlSuggestionModel", "QmlSuggestionModel can only be accessed through the qmledit extension.");
    qmlRegisterUncreatableType<lv::QmlMetaTypeInfo>(
        uri, 1, 0, "QmlMetaTypeInfo", "QmlMetaTypeInfo can only be accessed through the qmledit extension.");

    qmlRegisterSingletonType<lv::QmlTokenizer>(uri, 1, 0, "Tokenizer", &qmlTokenizerProvider);
    qmlRegisterSingletonType<lv::QmlSyntax>(   uri, 1, 0, "Syntax", &qmlSyntaxProvider);
    qmlRegisterSingletonType<lv::QmlMetaInfo>( uri, 1, 0, "MetaInfo", &qmlMetaInfoProvider);
}

/**
 * \brief Assign initialization params for this object
 */
void ProjectQmlExtension::setParams(Settings *settings, Project *project, ViewEngine *engine, Workspace *workspace){

}

bool ProjectQmlExtension::pluginTypesEnabled(){
    return PluginTypesFacade::pluginTypesEnabled();
}

QmlLibraryInfo::ScanStatus ProjectQmlExtension::loadPluginInfoInternal(
        QmlLanguageScanner *scanner,
        QmlLibraryInfo::Ptr lib,
        QQmlEngine *engine,
        QByteArray *stream)
{
    return PluginTypesFacade::loadPluginInfo(scanner, lib, engine, stream);
}


/**
 * \brief Creates a LanguageQmlHandler for the given \p document
 */
QObject *ProjectQmlExtension::createHandler(ProjectDocument* document, CodeHandler *handler){
    return new LanguageQmlHandler(m_engine, m_project, m_settings, this, document, handler);
}

}// namespace

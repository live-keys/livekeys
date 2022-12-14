#include "editjsonobject.h"
#include "qmlscriptlanguagehandler.h"
#include "live/mlnodetoqml.h"

#include "live/visuallogqt.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

EditJsonObject::EditJsonObject(QQmlEngine* parent)
    : QObject(parent)
    , m_engine(nullptr)
    , m_editorSettings(nullptr)
{
    const TSLanguage* lang = tree_sitter_json();

    m_language = new QmlLanguageObject(lang, this);

    QQmlContext* ctx = parent->rootContext();
    QObject* lg = ctx->contextProperty("lk").value<QObject*>();
    if ( !lg ){
        qWarning("Failed to find live global object.");
        return;
    }

    m_engine = ViewEngine::grabFromQmlEngine(parent);
    if ( !m_engine ){
        qWarning("Failed to find engine object.");
        return;
    }

    Settings* settings = static_cast<Settings*>(lg->property("settings").value<QObject*>());
    if ( !settings ){
        qWarning("Failed to find settings object.");
        return;
    }

    Project* project = Project::grabFromLayer(m_engine);
    if ( !project ){
        qWarning("Failed to find project object.");
        return;
    }

    m_editorSettings = qobject_cast<lv::EditorSettings*>(settings->file("editor"));
}

QObject* EditJsonObject::createHandler(
        ProjectDocument *document,
        CodeHandler *handler,
        const QString &settingsCategory,
        const QJSValue &initialSettings)
{
    if ( !m_editorSettings || !m_engine )
        return nullptr;

    MLNode n;
    ml::fromQml(initialSettings, n);

    QmlScriptLanguageHandler* languageHandler = new QmlScriptLanguageHandler(
        m_editorSettings,
        settingsCategory,
        n,
        m_engine,
        document,
        handler
    );

    return languageHandler;
}

}// namespace

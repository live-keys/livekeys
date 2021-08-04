#ifndef LVQMLSCRIPTLANGUAGEHANDLER_H
#define LVQMLSCRIPTLANGUAGEHANDLER_H

#include <QObject>
#include <QTimer>
#include <QTextCursor>
#include <QQmlListProperty>

#include "qmllanguageobject.h"

#include "live/lockedfileiosession.h"
#include "live/viewengine.h"
#include "live/settings.h"
#include "live/project.h"
#include "live/codecompletionmodel.h"

namespace lv{

class EditorSettings;
class QueryHighlighter;
class QmlScriptLanguageHandler : public QObject{

    Q_OBJECT
    Q_DISABLE_COPY(QmlScriptLanguageHandler)

public:
    QmlScriptLanguageHandler(
            EditorSettings* settings,
            const QString& settingsCategory,
            const MLNode& initialSettings,
            ViewEngine *engine,
            ProjectDocument *document,
            CodeHandler *handler);
    ~QmlScriptLanguageHandler() override;

    void assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange
    );

public slots:
    QList<int> languageFeatures() const;
    void suggestCompletion(int cursorPosition);

    void setLanguage(QmlLanguageObject* language);
    void createHighlighter(const QString& capturePattern, const QJSValue& highlights);

    void __documentFormatUpdate(int position, int length);
    void __cursorWritePositionChanged(QTextCursor cursor);

    void __settingsRefresh();

signals:
    void settingsChanged(QJSValue value);

private:
    QueryHighlighter*      m_highlighter;
    ViewEngine*            m_engine;
    ProjectDocument*       m_document;
    QString                m_settingsCategory;
    EditorSettings*        m_settings;
    QList<int>             m_languageFeatures;
    QmlLanguageObject*     m_language;
};

}// namespace

#endif // LVQMLSCRIPTLANGUAGEHANDLER_H

#ifndef LVLANGUAGEHANDLER_H
#define LVLANGUAGEHANDLER_H

#include <QObject>
#include <QTimer>
#include <QTextCursor>
#include <QQmlListProperty>

#include "live/lockedfileiosession.h"
#include "live/viewengine.h"
#include "live/settings.h"
#include "live/project.h"
#include "live/codecompletionmodel.h"

#include "editlvsettings.h"
#include "documenttree.h"
#include "live/elements/cursorcontext.h"

namespace lv{

class LanguageLvHighlighter;
class LanguageLvExtension;
class EditLvSettings;

class LanguageLvHandlerPrivate;
class LanguageLvHandler : public QObject{

    Q_OBJECT
    Q_DISABLE_COPY(LanguageLvHandler)

    friend class LanguageLvExtension;

public:
    explicit LanguageLvHandler(
        ViewEngine* engine,
        Project* project,
        EditLvSettings *settings,
        LanguageLvExtension* projectHandler,
        ProjectDocument *document,
        CodeHandler* handler = nullptr
    );
    ~LanguageLvHandler() override;

    void resetExtensionObject();

    EditLvSettings* settings();

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

    void __documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void __documentFormatUpdate(int position, int length);
    void __cursorWritePositionChanged(QTextCursor cursor);
    void __updateScope();
    void __aboutToDelete();

private:
//    void suggestionsForImport(const el::CursorContext& ctx, QList<CodeCompletionSuggestion>& suggestions);
//    void suggestionsForListener(const el::CursorContext& ctx, int position, QList<CodeCompletionSuggestion>& suggestions);
//    void suggestionsForLeftOfDeclaration(const el::CursorContext& ctx, int position, QList<CodeCompletionSuggestion>& suggestions);
//    void suggestionsForRightOfDeclaration(const el::CursorContext& ctx, int position, QList<CodeCompletionSuggestion>& suggestions);
//    void defaultSuggestions(const el::CursorContext& ctx, int position, QList<CodeCompletionSuggestion>& suggestions);

    LanguageLvExtension* m_extension;

    QScopedPointer<LanguageLvHandlerPrivate> d_ptr;

    LanguageLvHighlighter* m_highlighter;
    EditLvSettings*        m_settings;
    QQmlEngine*            m_engine;
    ProjectDocument*       m_document;
    el::DocumentTree*      m_documentTree;

    bool   m_newScope;
    QTimer m_scopeTimer;

    Q_DECLARE_PRIVATE(LanguageLvHandler)
};

inline EditLvSettings *LanguageLvHandler::settings(){
    return m_settings;
}

}// namespace

#endif // LVLANGUAGEHANDLER_H

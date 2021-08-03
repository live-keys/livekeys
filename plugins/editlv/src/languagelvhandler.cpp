#include "languagelvhandler.h"
#include "languagelvhighlighter.h"

#include "live/codehandler.h"
#include "live/projectdocument.h"

#include "languagelvextension.h"
#include "editlvsettings.h"
#include "live/elements/cursorcontext.h"
#include "live/elements/parseddocument.h"
#include <QQmlEngine>
#include "live/projectfile.h"

namespace lv{

class LanguageLvHandlerPrivate{
public:
    LanguageLvExtension* languageExtension;
};

LanguageLvHandler::LanguageLvHandler(
        ViewEngine *engine,
        Project*,
        EditLvSettings *settings,
        LanguageLvExtension *languageExtension,
        ProjectDocument *document,
        CodeHandler *handler)
    : QObject(handler)
    , d_ptr(new LanguageLvHandlerPrivate)
    , m_settings(settings)
    , m_engine(engine->engine())
    , m_document(document)
    , m_documentTree(new el::DocumentTree(document))
    , m_newScope(false)

{
    Q_D(LanguageLvHandler);
    d->languageExtension = languageExtension;

    m_highlighter = new LanguageLvHighlighter(settings, handler, m_documentTree, document ? document->textDocument() : nullptr);

    m_scopeTimer.setInterval(1000);
    m_scopeTimer.setSingleShot(true);

    connect(&m_scopeTimer, &QTimer::timeout, this, &LanguageLvHandler::__updateScope);

    CodeHandler* dh = static_cast<CodeHandler*>(parent());
    connect(dh, &CodeHandler::aboutToDeleteHandler, this, &LanguageLvHandler::__aboutToDelete);

    d->languageExtension->addLanguageHandler(this);

    m_highlighter->setTarget(m_document->textDocument());

    if ( m_document ){
        connect(m_document->textDocument(), &QTextDocument::contentsChange,
                this, &LanguageLvHandler::__documentContentsChanged);
        connect(m_document, &ProjectDocument::formatChanged, this, &LanguageLvHandler::__documentFormatUpdate);
        connect(
            m_document->textDocument(), &QTextDocument::cursorPositionChanged,
            this, &LanguageLvHandler::__cursorWritePositionChanged
        );

    }

//    if ( d->languageExtension->scanMonitor()->hasProjectScope() && document != nullptr ){
//        d->languageExtension->scanMonitor()->scanNewDocumentScope(document->file()->path(), document->content(), this);
//        d->languageExtension->scanner()->queueProjectScan();
//    }
}

LanguageLvHandler::~LanguageLvHandler(){
}

void LanguageLvHandler::resetExtensionObject(){
    m_extension = nullptr;
}

void LanguageLvHandler::assistCompletion(
    const QTextCursor &cursor,
    const QChar &insertion,
    bool manuallyTriggered,
    CodeCompletionModel *model,
    QTextCursor &cursorChange)
{
    if ( insertion != QChar() && !manuallyTriggered ){
        if ( insertion == '{' ){
            cursorChange = cursor;
            cursorChange.beginEditBlock();
            cursorChange.insertText("}");
            cursorChange.endEditBlock();
            cursorChange.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
            return;
        } else if (insertion == '}'){
            cursorChange = cursor;
            if (cursor.positionInBlock() >= 5)
            {
                QTextBlock block = m_document->textDocument()->findBlock(cursor.position());
                QString text = block.text();

                if (text.mid(cursor.positionInBlock()-5,4) == "    ")
                {
                    cursorChange.beginEditBlock();
                    for (int i = 0; i < 5; i++)
                        cursorChange.deletePreviousChar();
                    cursorChange.insertText("}");
                    cursorChange.endEditBlock();
                }
            }
            return;
        } else if ( insertion.category() == QChar::Separator_Line || insertion.category() == QChar::Separator_Paragraph){
            cursorChange = cursor;
            cursorChange.movePosition(QTextCursor::Up);
            QString text = cursorChange.block().text();
            QString indent = "";
            for ( QString::const_iterator it = text.begin(); it != text.end(); ++it ){
                if ( !it->isSpace() )
                    break;
                indent += *it;
            }
            bool isOpenBrace = false;
            for ( QString::const_reverse_iterator it = text.rbegin(); it != text.rend(); ++it ){
                if ( *it == QChar('{') )
                    isOpenBrace = true;
                if ( !it->isSpace() )
                    break;
            }
            cursorChange.movePosition(QTextCursor::Down);
            cursorChange.movePosition(QTextCursor::StartOfLine);

            if ( isOpenBrace ){
                text = cursorChange.block().text();
                if ( text.trimmed().startsWith("}") ){
                    cursorChange.beginEditBlock();
                    cursorChange.insertText(indent + "    \n" + indent);
                    cursorChange.endEditBlock();
                    cursorChange.movePosition(QTextCursor::Up);
                    cursorChange.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 4);
                    return;
                }
                indent += "    ";
            }

            cursorChange.beginEditBlock();
            cursorChange.insertText(indent);
            cursorChange.endEditBlock();
            return;
        } else if ( insertion == '\"' ) {
            cursorChange = cursor;
            cursorChange.movePosition(QTextCursor::Left);
            el::CursorContext ctx = el::ParsedDocument::findCursorContext(m_documentTree->ast(), cursor.position());
            if ( !(ctx.context() & el::CursorContext::InStringLiteral) ){
                cursorChange.beginEditBlock();
                cursorChange.insertText("\"");
                cursorChange.endEditBlock();
            } else {
                cursorChange.movePosition(QTextCursor::Right);
            }
            return;
        } else if ( insertion == '\'' ) {
            cursorChange = cursor;
            cursorChange.movePosition(QTextCursor::Left);
            el::CursorContext ctx = el::ParsedDocument::findCursorContext(m_documentTree->ast(), cursor.position());
            if ( !(ctx.context() & el::CursorContext::InStringLiteral) ){
                cursorChange.beginEditBlock();
                cursorChange.insertText("\'");
                cursorChange.endEditBlock();
            } else {
                cursorChange.movePosition(QTextCursor::Right);
            }
            return;

        } else if ( insertion != QChar('.') && !insertion.isLetterOrNumber() ){
            model->disable();
            return;
        }
    } else if ( !manuallyTriggered ){
        model->disable();
        return;
    }

    if (!m_documentTree->ast())
        return;

    auto ctx = el::ParsedDocument::findCursorContext(m_documentTree->ast(), 2*cursor.position());
    QString filter;
    if (ctx.expressionPath().size() > 0){
        auto last = ctx.expressionPath().back();
        filter = m_document->substring(static_cast<int>(last.from()),static_cast<int>(last.length()));
    }

    model->setCompletionPosition(
        cursor.position() -
        (ctx.expressionPath().size() > 0 ? static_cast<int>(ctx.expressionPath().back().length()) : 0)
    );

//    if( filter == "" && insertion.isNumber() ){
//        model->setFilter(insertion);
//        return;
//    }

    QList<CodeCompletionSuggestion> suggestions;
    if ( ctx.context() & el::CursorContext::InImport ){
        suggestionsForImport(ctx, suggestions);
        model->setSuggestions(suggestions, filter);
    } else if ( ctx.context() & el::CursorContext::InLeftOfDeclaration ){
        auto propertyDeclaredType = ctx.propertyDeclaredType();
        if (m_documentTree->slice(propertyDeclaredType) == "on")
        {
            suggestionsForListener(ctx, cursor.position(), suggestions);
            model->setSuggestions(suggestions, filter);

        } else {
            suggestionsForLeftOfDeclaration(ctx, cursor.position(), suggestions);
            model->setSuggestions(suggestions, filter);

        }
    } else if ( ctx.context() & el::CursorContext::InRightOfDeclaration ){
        suggestionsForRightOfDeclaration(ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else {
        defaultSuggestions(ctx, cursor.position(), suggestions);

        if ( ctx.expressionPath().size() > 1 ){
            // TODO: ELEMENTS
            model->setSuggestions(suggestions, filter);
        } else {
            // suggestionsForGlobalContext(*ctx, suggestions); TODO: ELEMENTS
            // suggestionsForNamespaceTypes(ctx.expressionPath().size() > 1 ? ctx.expressionPath().first() : "", suggestions);
            model->setSuggestions(suggestions, filter);
        }

}

}

QList<int> LanguageLvHandler::languageFeatures() const{
    return {
//        DocumentHandler::LanguageHelp,
//        DocumentHandler::LanguageScope,
        CodeHandler::LanguageHighlighting,
        CodeHandler::LanguageCodeCompletion
    };
}

void LanguageLvHandler::suggestCompletion(int cursorPosition){
    CodeHandler* dh = static_cast<CodeHandler*>(parent());
    if ( !m_document || !dh )
        return;

    QTextCursor cursor(m_document->textDocument());
    cursor.setPosition(cursorPosition);
    QTextCursor newCursor;
    assistCompletion(
        cursor,
        QChar(),
        true,
        dh->completionModel(),
        newCursor
                );
}

void LanguageLvHandler::__documentContentsChanged(int /*position*/, int /*charsRemoved*/, int /*charsAdded*/){
    if ( !m_document->editingStateIs(ProjectDocument::Silent) ){
        m_scopeTimer.start();
    }
}

void LanguageLvHandler::__documentFormatUpdate(int /*position*/, int /*length*/){
    //TODO: Trigger highlighter for the specified section
    //    rehighlightSection(position, position + length);
}

void LanguageLvHandler::__cursorWritePositionChanged(QTextCursor cursor){
    CodeHandler* dh = static_cast<CodeHandler*>(parent());
    if ( dh && dh->editorFocus() && cursor.position() == dh->currentCursorPosition() &&
         !m_document->editingStateIs(ProjectDocument::Assisted) &&
         !m_document->editingStateIs(ProjectDocument::Silent)
        )
    {
        m_document->addEditingState(ProjectDocument::Assisted);
        QTextCursor newCursor;
        assistCompletion(
            cursor,
            dh->lastAddedChar(),
            false,
            dh->completionModel(),
            newCursor
        );
        m_document->removeEditingState(ProjectDocument::Assisted);
        if ( !newCursor.isNull() ){
            dh->requestCursorPosition(newCursor.position());
        }
    }
}

void LanguageLvHandler::__updateScope(){
    //TODO: Trigger new scope update
//    if ( d->projectHandler->scanMonitor()->hasProjectScope() && m_document )
    //        d->projectHandler->scanMonitor()->queueNewDocumentScope(m_document->file()->path(), m_document->content(), this);
}

void LanguageLvHandler::__aboutToDelete(){
    //TODO
}

void LanguageLvHandler::suggestionsForImport(const el::CursorContext &ctx, QList<CodeCompletionSuggestion> &suggestions)
{
    if (ctx.context() & el::CursorContext::InRelativeImport)
    {
        auto folderPath = m_document->file()->parentEntry()->path();
    }
    else {
        foreach (const QString& importPath, m_engine->importPathList()){
            //suggestionsForRecursiveImport(0, importPath, ctx.expressionPath(), suggestions); // TODO: ELEMENTS
        }
    }
    std::sort(suggestions.begin(), suggestions.end(), &CodeCompletionSuggestion::compare);

}

void LanguageLvHandler::suggestionsForListener(const el::CursorContext &ctx, int position, QList<CodeCompletionSuggestion> &suggestions)
{

}

void LanguageLvHandler::suggestionsForLeftOfDeclaration(const el::CursorContext &ctx, int position, QList<CodeCompletionSuggestion> &suggestions)
{

}

void LanguageLvHandler::suggestionsForRightOfDeclaration(const el::CursorContext &ctx, int position, QList<CodeCompletionSuggestion> &suggestions)
{

}

void LanguageLvHandler::defaultSuggestions(const el::CursorContext &ctx, int position, QList<CodeCompletionSuggestion> &suggestions)
{

}

}// namespace

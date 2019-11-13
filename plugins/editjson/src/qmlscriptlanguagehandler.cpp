#include "qmlscriptlanguagehandler.h"
#include "queryhighlighter.h"

#include "live/editorsettings.h"
#include "live/documenthandler.h"
#include "live/projectdocument.h"

#include "live/mlnodetoqml.h"

#include <QQmlEngine>

namespace lv{

QmlScriptLanguageHandler::QmlScriptLanguageHandler(
        EditorSettings* settings,
        const QString& settingsCategory,
        const MLNode& initialSettings,
        ViewEngine* engine,
        ProjectDocument *document,
        DocumentHandler *handler)
    : QObject(handler)
    , m_highlighter(nullptr)
    , m_engine(engine)
    , m_document(document)
    , m_settingsCategory(settingsCategory)
    , m_settings(settings)
{
    if ( m_document ){
        connect(
            m_document, &ProjectDocument::formatChanged,
            this, &QmlScriptLanguageHandler::__documentFormatUpdate);
        connect(
            m_document->textDocument(), &QTextDocument::cursorPositionChanged,
            this, &QmlScriptLanguageHandler::__cursorWritePositionChanged
        );
    }

    if ( !m_settingsCategory.isEmpty() ){
        m_settings->write(settingsCategory, initialSettings);
        connect(
            m_settings, &EditorSettings::refresh,
            this, &QmlScriptLanguageHandler::__settingsRefresh
        );
    }

}

QmlScriptLanguageHandler::~QmlScriptLanguageHandler(){

}

void QmlScriptLanguageHandler::assistCompletion(
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
//            QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursorChange);
//            if ( !(ctx->context() & QmlCompletionContext::InStringLiteral) ){
//                cursorChange.beginEditBlock();
//                cursorChange.insertText("\"");
//                cursorChange.endEditBlock();
//            } else {
//                cursorChange.movePosition(QTextCursor::Right);
//            }
            return;
        } else if ( insertion == '\'' ) {
            cursorChange = cursor;
            cursorChange.movePosition(QTextCursor::Left);
//            QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursorChange);
//            if ( !(ctx->context() & QmlCompletionContext::InStringLiteral) ){
//                cursorChange.beginEditBlock();
//                cursorChange.insertText("\'");
//                cursorChange.endEditBlock();
//            } else {
//                cursorChange.movePosition(QTextCursor::Right);
//            }
            return;

        } else if ( insertion != QChar('.') && !insertion.isLetterOrNumber() ){
            model->disable();
            return;
        }
    } else if ( !manuallyTriggered ){
        model->disable();
        return;
    }

    //TODO: Get completion context
}

QList<int> QmlScriptLanguageHandler::languageFeatures() const{
    return m_languageFeatures;
}

void QmlScriptLanguageHandler::suggestCompletion(int cursorPosition){
    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
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

void QmlScriptLanguageHandler::setLanguage(QmlLanguageObject *language){
    m_language = language;
}

void QmlScriptLanguageHandler::createHighlighter(const QString &capturePattern, const QJSValue &highlights){

}

void QmlScriptLanguageHandler::__documentFormatUpdate(int /*position*/, int /*length*/){
    //TODO: Trigger highlighter for the specified section
    //    rehighlightSection(position, position + length);
}

void QmlScriptLanguageHandler::__cursorWritePositionChanged(QTextCursor cursor){
    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
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

void QmlScriptLanguageHandler::__settingsRefresh(){
    QJSValue settings;
    MLNode n = m_settings->readFor(m_settingsCategory);
    ml::toQml(n, settings, m_engine->engine());
    emit settingsChanged(settings);
}

}// namespace

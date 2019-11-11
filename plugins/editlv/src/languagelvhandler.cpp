#include "languagelvhandler.h"
#include "languagelvhighlighter.h"

#include "live/documenthandler.h"
#include "live/projectdocument.h"

#include "languagelvextension.h"
#include "editlvsettings.h"

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
        DocumentHandler *handler)
    : QObject(handler)
    , d_ptr(new LanguageLvHandlerPrivate)
    , m_highlighter(new LanguageLvHighlighter(settings, handler, document ? document->textDocument() : nullptr))
    , m_settings(settings)
    , m_engine(engine->engine())
    , m_document(document)
    , m_newScope(false)
{
    Q_D(LanguageLvHandler);
    d->languageExtension = languageExtension;

    m_scopeTimer.setInterval(1000);
    m_scopeTimer.setSingleShot(true);

    connect(&m_scopeTimer, &QTimer::timeout, this, &LanguageLvHandler::__updateScope);

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    connect(dh, &DocumentHandler::aboutToDeleteHandler, this, &LanguageLvHandler::__aboutToDelete);

    d->languageExtension->addLanguageHandler(this);
//    d->projectHandler->scanMonitor()->addScopeListener(this);

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

QList<int> LanguageLvHandler::languageFeatures() const{
    return {
//        DocumentHandler::LanguageHelp,
//        DocumentHandler::LanguageScope,
        DocumentHandler::LanguageHighlighting,
        DocumentHandler::LanguageCodeCompletion
    };
}

void LanguageLvHandler::suggestCompletion(int cursorPosition){
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

void LanguageLvHandler::__updateScope(){
    //TODO: Trigger new scope update
//    if ( d->projectHandler->scanMonitor()->hasProjectScope() && m_document )
    //        d->projectHandler->scanMonitor()->queueNewDocumentScope(m_document->file()->path(), m_document->content(), this);
}

void LanguageLvHandler::__aboutToDelete(){
    //TODO
}

}// namespace

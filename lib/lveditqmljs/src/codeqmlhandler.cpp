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

#include "live/codeqmlhandler.h"
#include "qmljshighlighter_p.h"
#include "qmljssettings.h"
#include "qmljs/qmljsscanner.h"
#include "projectqmlscanner_p.h"
#include "projectqmlscanmonitor_p.h"

#include "qmlcompletioncontextfinder.h"
#include "projectqmlscopecontainer_p.h"
#include "qmllibraryinfo_p.h"
#include "documentqmlvaluescanner_p.h"
#include "documentqmlvalueobjects.h"
#include "qmljsbuiltintypes_p.h"
#include "qmljshighlighter_p.h"
#include "qmlbindingchannel.h"
#include "qmlbindingspan.h"
#include "qmlcodeconverter.h"
#include "qmladdcontainer.h"
#include "qmlscopesnap_p.h"
#include "qmlusagegraphscanner.h"

#include "qmljs/parser/qmljsast_p.h"
#include "qmljs/qmljsbind.h"

#include "live/documenthandler.h"
#include "live/codecompletionsuggestion.h"
#include "live/projectfile.h"
#include "live/project.h"
#include "live/editorsettings.h"
#include "live/projectqmlextension.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/editorglobalobject.h"
#include "live/palettecontainer.h"
#include "live/qmlpropertywatcher.h"
#include "live/runnablecontainer.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QFileInfo>
#include <QDirIterator>
#include <QTextDocument>
#include <QQmlComponent>
#include <QStringList>
#include <QWaitCondition>

namespace lv{

// CodeQmlHandlerPrivate
// ----------------------------------------------------------------------------

/// \private
class CodeQmlHandlerPrivate{

public:
    DocumentQmlScope::Ptr  documentScope;
    ProjectQmlExtension*   projectHandler;

    QmlScopeSnap snapScope() const{
        return QmlScopeSnap(projectHandler->scanMonitor()->projectScope(), documentScope);
    }
};

namespace qmlhandler_helpers{

    /**
     * @brief Checks wether value is a valid id in the given document scope.
     * Outputs the object path and object value if true.
     */
    bool isId(
        const QmlScopeSnap& scope,
        const QString& str,
        DocumentQmlInfo::ValueReference& documentValue,
        QmlInheritanceInfo& typePath,
        QString& typeLibraryKey
    ){
        documentValue = scope.document->info()->valueForId(str);
        if ( scope.document->info()->isValueNull(documentValue) )
            return false;

        QString typeName = scope.document->info()->extractTypeName(documentValue);
        if ( typeName != "" ){
            QmlInheritanceInfo ipath = scope.getTypePath(typeName);
            if ( !ipath.isEmpty() ){
                typePath = ipath;
                typeLibraryKey = scope.libraryFromUri(ipath.nodes.last().prefereredType().path()).path;
            }
        }

        return true;
    }

    /**
     * @brief isNamespace
     */
    bool isNamespace(DocumentQmlScope::Ptr documentScope, const QString& str){
        foreach( const DocumentQmlScope::ImportEntry& imp, documentScope->imports() ){
            if ( imp.first.as() == str ){
                return true;
            }
        }
        return false;
    }


    /**
     * @brief Create suggestions from an object type path
     */
    void suggestionsForObjectPath(
        const QmlInheritanceInfo& typePath,
        bool suggestProperties,
        bool suggestMethods,
        bool suggestSignals,
        bool suggestEnums,
        bool suggestGeneratedSlots,
        const QString& suffix,
        QList<CodeCompletionSuggestion>& suggestions
    ){
        for ( auto it = typePath.nodes.begin(); it != typePath.nodes.end(); ++it ){
            LanguageUtils::FakeMetaObject::ConstPtr object = QmlTypeInfoPrivate::typeObject(*it);
            if ( object.isNull() )
                continue;

            QList<CodeCompletionSuggestion> localSuggestions;

            if ( suggestProperties ){
                for ( int i = 0; i < object->propertyCount(); ++i ){
                    if ( !object->property(i).name().startsWith("__") ){
                        localSuggestions << CodeCompletionSuggestion(
                            object->property(i).name(),
                            object->property(i).typeName(),
                            it->prefereredType().name(),
                            object->property(i).name() + (object->property(i).isPointer() ? "" : suffix),
                            "qml/" + it->prefereredType().path() + "#" + it->prefereredType().name() + "." + object->property(i).name()
                        );
                    }
                }
            }
            for ( int i = 0; i < object->methodCount(); ++i ){
                LanguageUtils::FakeMetaMethod m = object->method(i);
                if ( (m.methodType() == LanguageUtils::FakeMetaMethod::Method ||
                      m.methodType() == LanguageUtils::FakeMetaMethod::Slot) && suggestMethods ){
                    QString completion = m.methodName() + "(";// + m.parameterNames().join(", ") + ")";
                    localSuggestions << CodeCompletionSuggestion(
                        m.methodName() + "()", "function", it->prefereredType().name(), completion
                    );
                }
                if ( m.methodType() == LanguageUtils::FakeMetaMethod::Signal && suggestSignals ){
                    QString completion = m.methodName() + "(" + m.parameterNames().join(", ") + ")";
                    localSuggestions << CodeCompletionSuggestion(
                        m.methodName() + "()", "signal", it->prefereredType().name(), completion
                    );
                }
                if ( m.methodType() == LanguageUtils::FakeMetaMethod::Signal && suggestGeneratedSlots ){
                    QString methodName = m.methodName();
                    if ( methodName.size() > 0 )
                        methodName[0] = methodName[0].toUpper();
                    localSuggestions << CodeCompletionSuggestion(
                        "on" + methodName, "slot", it->prefereredType().name(), "on" + methodName + suffix
                    );
                }
            }
            if ( suggestEnums ){
                for ( int i = 0; i < object->enumeratorCount(); ++i ){
                    LanguageUtils::FakeMetaEnum e = object->enumerator(i);
                    for ( int j = 0; j < e.keyCount(); ++j ){
                        localSuggestions << CodeCompletionSuggestion(
                            e.key(j),
                            e.name(),
                            it->prefereredType().name(),
                            e.key(j)
                        );
                    }
                }
            }
            if ( suggestGeneratedSlots ){
                // TODO for SIGNALS
                for ( int i = 0; i < object->propertyCount(); ++i ){
                    if ( !object->property(i).name().startsWith("__") ){
                        QString propertyName = object->property(i).name();
                        if ( propertyName.size() > 0 )
                            propertyName[0] = propertyName[0].toUpper();

                        localSuggestions << CodeCompletionSuggestion(
                            "on" + propertyName + "Changed",
                            "slot",
                            it->prefereredType().name(),
                            "on" + propertyName + "Changed" + suffix
                        );
                    }
                }
            }

            std::sort(localSuggestions.begin(), localSuggestions.end(), &CodeCompletionSuggestion::compare);
            suggestions << localSuggestions;
        }
    }
}

// CodeQmlHandler implementation
// ----------------------------------------------------------------------------

/**
 * \class lv::CodeQmlHandler
 * \ingroup lveditqmljs
 * \brief Main code handler for the qml extension.
 *
 *
 * Handles code completion, palette control, property and item searching.
 */

/**
 * \brief CodeQmlHandler constructor
 *
 * This is called through the ProjectQmlHandler.
 */
CodeQmlHandler::CodeQmlHandler(
        ViewEngine *engine,
        Project *,
        QmlJsSettings *settings,
        ProjectQmlExtension *projectHandler,
        ProjectDocument* document,
        DocumentHandler *handler)
    : QObject(handler)
    , m_target(nullptr)
    , m_highlighter(new QmlJsHighlighter(settings, handler, nullptr))
    , m_settings(settings)
    , m_engine(engine->engine())
    , m_completionContextFinder(new QmlCompletionContextFinder)
    , m_newScope(false)
    , m_editingFragment(nullptr)
    , d_ptr(new CodeQmlHandlerPrivate)
{
    Q_D(CodeQmlHandler);
    d->projectHandler = projectHandler;

    m_scopeTimer.setInterval(1000);
    m_scopeTimer.setSingleShot(true);
    connect(&m_scopeTimer, SIGNAL(timeout()), this, SLOT(updateScope()));
    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    connect(dh, SIGNAL(aboutToDeleteHandler()), this, SLOT(aboutToDelete()));

    d->projectHandler->addCodeQmlHandler(this);
    d->projectHandler->scanMonitor()->addScopeListener(this);

    setDocument(document);
}

/**
 * \brief CodeQmlHandler destructor
 */
CodeQmlHandler::~CodeQmlHandler(){
    Q_D(CodeQmlHandler);

    if ( d->projectHandler ){
        d->projectHandler->removeCodeQmlHandler(this);
        d->projectHandler->scanMonitor()->removeScopeListener(this);
    }

    delete m_completionContextFinder;
}

/**
 * \brief Implementation of code completion assist.
 *
 * Handles bracket and paranthesis auto-completion together with suggestions population.
 */
void CodeQmlHandler::assistCompletion(
        QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange)
{
    if ( !m_target )
        return;

    if ( insertion != QChar() && !manuallyTriggered ){
        if ( insertion == '{' ){
            cursorChange = cursor;
            cursorChange.beginEditBlock();
            cursorChange.insertText("}");
            cursorChange.endEditBlock();
            cursorChange.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
            return;
        } else if (insertion == '}'){
            if (cursor.positionInBlock() >= 5)
            {
                QTextBlock block = m_target->findBlock(cursor.position());
                QString text = block.text();

                if (text.mid(cursor.positionInBlock()-5,4) == "    ")
                {    
                    cursor.beginEditBlock();
                    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 5);
                    cursor.insertText("}");
                    cursor.endEditBlock();
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
            QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursorChange);
            if ( !(ctx->context() & QmlCompletionContext::InStringLiteral) ){
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
            QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursorChange);
            if ( !(ctx->context() & QmlCompletionContext::InStringLiteral) ){
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

    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);
    QString filter = ctx->expressionPath().size() > 0 ? ctx->expressionPath().last() : "";

    model->setCompletionPosition(
        cursor.position() -
        (ctx->expressionPath().size() > 0 ? ctx->expressionPath().last().length() : 0)
    );

    if( filter == "" && insertion.isNumber() ){
        model->setFilter(insertion);
        return;
    }
    if ( model->completionContext() ){
        QmlCompletionContext* modelCtx = static_cast<QmlCompletionContext*>(model->completionContext());
        if ( modelCtx && *modelCtx == *ctx && !m_newScope ){
            model->setFilter(filter);
            if ( model->rowCount() )
                model->enable();
            return;
        }
        model->removeCompletionContext();
    }
    m_newScope = false;

    QList<CodeCompletionSuggestion> suggestions;
    if ( ctx->context() & QmlCompletionContext::InStringLiteral ){
        if ( ctx->context() & QmlCompletionContext::InImport ){
            suggestionsForStringImport(extractQuotedString(cursor), suggestions, filter);
            model->setSuggestions(suggestions, filter);
        } else {
            model->setSuggestions(suggestions, filter);
        }
    } else if ( ctx->context() & QmlCompletionContext::InImport ){
        if ( ctx->context() & QmlCompletionContext::InImportVersion ){
            model->setSuggestions(suggestions, filter);
        } else {
            suggestionsForImport(*ctx, suggestions);
            model->setSuggestions(suggestions, filter);
        }
    } else if ( ctx->context() & QmlCompletionContext::InAfterOnLhsOfBinding ){
        suggestionsForLeftSignalBind(*ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else if ( ctx->context() & QmlCompletionContext::InLhsOfBinding ){
        suggestionsForLeftBind(*ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else if ( ctx->context() & QmlCompletionContext::InRhsofBinding ){
        suggestionsForRightBind(*ctx, cursor.position(), suggestions);
        model->setSuggestions(suggestions, filter);
    } else {

        if ( ctx->expressionPath().size() > 1 ){
            QmlScopeSnap scope = d_ptr->snapScope();

            QStringList expressionPathHead = ctx->expressionPath();
            expressionPathHead.removeLast();

            QmlScopeSnap::ExpressionChain expression = scope.evaluateExpression(
                scope.quickObjectDeclarationType(ctx->objectTypePath()), expressionPathHead, cursor.position()
            );

            if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ObjectNode ){
                suggestionsForValueObject(expression.documentValueObject, suggestions, true, true, false, true);
                qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, true, true, false, false, false, "", suggestions);
                return;
            } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ClassNode ){
                suggestionsForValueObject(expression.documentValueObject, suggestions, true, true, false, true);
                qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, true, true, false, true, false, "", suggestions);
            } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::NamespaceNode ){
                suggestionsForNamespaceTypes(expression.importAs, suggestions);
            } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::PropertyNode ){
                qmlhandler_helpers::suggestionsForObjectPath(expression.propertyChain.last().resultTypePath, true, true, false, false, false, "", suggestions);
            }
            model->setSuggestions(suggestions, filter);
        } else {
            suggestionsForGlobalQmlContext(*ctx, suggestions);
            suggestionsForNamespaceTypes(ctx->expressionPath().size() > 1 ? ctx->expressionPath().first() : "", suggestions);
            model->setSuggestions(suggestions, filter);
        }
    }

    if ( model->rowCount() )
        model->enable();
}

/**
 * \brief Assints the document that will be used by this class.
 */
void CodeQmlHandler::setDocument(ProjectDocument *document){
    Q_D(CodeQmlHandler);
    m_document      = document;
    m_target        = document->textDocument();
    m_highlighter->setTarget(m_target);
    d->documentScope = DocumentQmlScope::createEmptyScope(d->projectHandler->scanMonitor()->projectScope());

    if ( m_document ){
        connect(m_document->textDocument(), &QTextDocument::contentsChange,
                this, &CodeQmlHandler::__documentContentsChanged);
        connect(m_document, &ProjectDocument::formatChanged, this, &CodeQmlHandler::__documentFormatUpdate);
        connect(
            m_document->textDocument(), &QTextDocument::cursorPositionChanged,
            this, &CodeQmlHandler::__cursorWritePositionChanged
        );

        auto it = m_edits.begin();
        while( it != m_edits.end() ){
            QmlEditFragment* edit = *it;
            it = m_edits.erase(it);
            edit->emitRemoval();
            edit->deleteLater();
        }
    }

    if ( d->projectHandler->scanMonitor()->hasProjectScope() && document != nullptr ){
        d->projectHandler->scanMonitor()->scanNewDocumentScope(document->file()->path(), document->content(), this);
        d->projectHandler->scanner()->queueProjectScan();
    }
}

/**
 * \brief DocumentContentsChanged handler
 */
void CodeQmlHandler::__documentContentsChanged(int position, int, int){
    if ( !m_document->editingStateIs(ProjectDocument::Silent) ){
        if ( m_editingFragment ){
            if ( position < m_editingFragment->valuePosition() ||
                 position > m_editingFragment->valuePosition() + m_editingFragment->valueLength() )
            {
                cancelEdit();
            }
        }

        m_scopeTimer.start();
    }
}

void CodeQmlHandler::__documentFormatUpdate(int position, int length){
    rehighlightSection(position, position + length);
}

/// \private
void CodeQmlHandler::__cursorWritePositionChanged(QTextCursor cursor){
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

/**
 * \brief Called when a new scope is available from the scanMonitor
 */
void CodeQmlHandler::updateScope(){
    Q_D(CodeQmlHandler);
    if ( d->projectHandler->scanMonitor()->hasProjectScope() && m_document )
        d->projectHandler->scanMonitor()->queueNewDocumentScope(m_document->file()->path(), m_document->content(), this);
}

void CodeQmlHandler::rehighlightSection(int start, int end){
    QTextBlock bl = m_target->findBlock(start);
    while ( bl.isValid() && bl.position() < end){
        rehighlightBlock(bl);
        bl = bl.next();
    }
}

void CodeQmlHandler::resetProjectQmlExtension(){
    Q_D(CodeQmlHandler);
    d->projectHandler = nullptr;
}

QString CodeQmlHandler::getHelpEntity(int position){
    Q_D(CodeQmlHandler);
    if ( !m_document )
        return nullptr;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

    if ( ctx->context() & QmlCompletionContext::InImport ){

        QStringList importUri;

        if (ctx->context() & QmlCompletionContext::InImportVersion){
            importUri = ctx->objectTypePath();

            if ( !importUri.isEmpty() && (importUri.last().isEmpty() || importUri.last().front().isNumber() ) ){
                importUri.removeLast();
            }
            if ( !importUri.isEmpty() && (importUri.last().isEmpty() || importUri.last().front().isNumber() ) ){
                importUri.removeLast();
            }

            return  "qml/" + importUri.join(".");
        } else {
            QStringList expression = ctx->expressionPath();

            QTextBlock block = m_target->findBlock(cursor.position());
            int positionInBlock = cursor.position() - block.position();

            QString blockText = block.text().mid(positionInBlock);
            for ( QString::iterator blockCh = blockText.begin(); blockCh != blockText.end(); ++blockCh ){
                if ( blockCh->isLetterOrNumber() || *blockCh == QChar::fromLatin1('_') || *blockCh == QChar::fromLatin1('$') ){
                    expression.last().append(*blockCh);
                } else {
                    break;
                }
            }

            return "qml/" + expression.join(".");
        }

    } else {
        QStringList expression;
        int propertyPosition = ctx->propertyPosition();
        int propertyLength   = 0;

        QChar expressionEndDelimiter;

        if ( ctx->context() & QmlCompletionContext::InLhsOfBinding ){
            expression = ctx->expressionPath();

            if ( propertyPosition == -1 ){
                if ( m_target->characterAt(cursor.position()).isSpace() )
                    return nullptr;
                else
                    propertyPosition = cursor.position();
            }

            int advancedLength = DocumentQmlValueScanner::getExpressionExtent(
                m_target, cursor.position(), &expression, &expressionEndDelimiter
            );
            propertyLength = (cursor.position() - propertyPosition) + advancedLength;

            if ( expression.size() > 0 ){
                QmlScopeSnap scope = d->snapScope();
                if ( expressionEndDelimiter == QChar('{') && expression.size() < 3 ){ // object
                    QmlInheritanceInfo ipath = scope.getTypePath(expression);
                    if ( !ipath.isEmpty() ){
                        return ipath.nodes.first().prefereredType().join();
                    }
                } else { // property declaration
                    QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
                        scope.quickObjectDeclarationType(ctx->objectTypePath()), expression, cursor.position()
                    );

                    if ( propChain.size() == expression.size() && propChain.size() > 0 ){
                        QmlScopeSnap::PropertyReference& propref = propChain.last();
                        if ( !propref.classTypePath.isEmpty() ){
                            return propref.classTypePath.nodes.first().prefereredType().join();
                        }
                    }
                }
            }

        } else if ( ctx->context() & QmlCompletionContext::InRhsofBinding ){
            expression = ctx->expressionPath();

            QTextBlock block = m_target->findBlock(cursor.position());
            int positionInBlock = cursor.position() - block.position();

            QString blockText = block.text().mid(positionInBlock);
            for ( QString::iterator blockCh = blockText.begin(); blockCh != blockText.end(); ++blockCh ){
                if ( blockCh->isLetterOrNumber() || *blockCh == QChar::fromLatin1('_') || *blockCh == QChar::fromLatin1('$') ){
                    expression.last().append(*blockCh);
                } else {
                    break;
                }
            }

            QmlScopeSnap scope = d->snapScope();

            QmlScopeSnap::ExpressionChain expressionChain = scope.evaluateExpression(
                scope.quickObjectDeclarationType(ctx->objectTypePath()), expression, cursor.position()
            );

            if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::ObjectNode ){
                if ( !expressionChain.typeReference.isEmpty() ){
                    return expressionChain.typeReference.nodes.first().prefereredType().join();
                }
            } else if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::ClassNode ){
                if ( !expressionChain.typeReference.isEmpty() ){
                    return expressionChain.typeReference.nodes.first().prefereredType().join();
                }

            } else if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::EnumNode ){

                if ( !expressionChain.typeReference.isEmpty() ){
                    return expressionChain.typeReference.nodes.first().prefereredType().join() + "." + expressionChain.enumName;
                }

            } else if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::NamespaceNode ){

                foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
                    if ( imp.first.as() == expression.first() ){
                        return "qml/" + imp.first.path();
                    }
                }

            } else if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::PropertyNode ){

                if ( expressionChain.propertyChain.size() > 0 ){
                    QmlScopeSnap::PropertyReference& propref = expressionChain.propertyChain.last();
                    if ( !propref.classTypePath.isEmpty() ){
                        QmlTypeInfo tr = propref.classTypePath.nodes.first();
                        return tr.prefereredType().join();
                    }
                }

            }
        }

    }

    return "";
}

/**
 * \brief Calls a new rehighlight on the specified block
 */
void CodeQmlHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_highlighter ){
        m_highlighter->rehighlightBlock(block);
    }
}

/**
 * \brief Get a list of declarations from a specific cursor
 */
QList<QmlDeclaration::Ptr> CodeQmlHandler::getDeclarations(const QTextCursor& cursor){
    Q_D(CodeQmlHandler);

    QList<QmlDeclaration::Ptr> properties;
    int length = cursor.selectionEnd() - cursor.selectionStart();

    if ( length == 0 ){
        QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

        QStringList expression;
        int propertyPosition = ctx->propertyPosition();
        int propertyLength   = 0;

        QChar expressionEndDelimiter;

        if ( ctx->context() & QmlCompletionContext::InLhsOfBinding ){
            expression = ctx->expressionPath();

            if ( propertyPosition == -1 ){
                if ( m_target->characterAt(cursor.position()).isSpace() ){
                    return properties;
                }
                else
                    propertyPosition = cursor.position();
            }

            int advancedLength = DocumentQmlValueScanner::getExpressionExtent(
                m_target, cursor.position(), &expression, &expressionEndDelimiter
            );

            propertyLength = (cursor.position() - propertyPosition) + advancedLength;

        } else if ( ctx->context() & QmlCompletionContext::InRhsofBinding ){
            expression     = ctx->propertyPath();
            propertyLength = DocumentQmlValueScanner::getExpressionExtent(m_target, ctx->propertyPosition());
        } else if ( ctx->context() == 0 ){
            expression = ctx->expressionPath();

            QTextBlock cursorBlock = cursor.block();
            QString cursorBlockText = cursorBlock.text();
            propertyPosition = cursorBlock.position();
            for ( auto it = cursorBlockText.begin(); it != cursorBlockText.end(); ++it ){
                if ( !it->isSpace( ) )
                    break;
                ++propertyPosition;
            }

            int advancedLength = DocumentQmlValueScanner::getExpressionExtent(
                m_target, cursor.position(), &expression, &expressionEndDelimiter
            );
            propertyLength = (cursor.position() - propertyPosition) + advancedLength;

            if ( expressionEndDelimiter != '{' )
                expression = QStringList();
        }

        if ( expression.size() > 0 ){
            if ( expressionEndDelimiter == QChar('{') ){ // dealing with an object list declaration ( 'Object{' )

                QmlScopeSnap scope = d->snapScope();

                QmlInheritanceInfo ipath = scope.getTypePath(expression);

                if ( !ipath.isEmpty() ){

                    QmlTypeInfo tr = ipath.nodes.first();

                    properties.append(QmlDeclaration::create(
                        QStringList(),
                        ipath.languageType(),
                        QmlTypeReference(),
                        propertyPosition,
                        0,
                        m_document
                    ));
                }


            } else { // dealing with a property declaration
                QmlScopeSnap scope = d->snapScope();

                bool isSlot = false;
                if ( expression.length() == 1 && expression.first().startsWith("on") ){
                    QString signalName = expression.first().mid(2);
                    if ( !signalName.isEmpty() ){
                        signalName[0] = signalName[0].toLower();
                    }

                    QmlFunctionInfo associatedSignal = scope.getSignal(
                        scope.quickObjectDeclarationType(ctx->objectTypePath()), signalName, cursor.position()
                    );
                    QmlScopeSnap::PropertyReference associatedPropertyInfo;

                    if ( !associatedSignal.isValid() && signalName.endsWith("Changed") ){
                        associatedPropertyInfo = scope.getProperty(
                            scope.quickObjectDeclarationType(ctx->objectTypePath()),
                            signalName.mid(0, signalName.length() - 7), cursor.position());

                        if ( associatedPropertyInfo.isValid() ){
                            associatedSignal.name = signalName;
                            associatedSignal.accessType = QmlFunctionInfo::Public;
                            associatedSignal.functionType = QmlFunctionInfo::Signal;
                            associatedSignal.objectType = associatedPropertyInfo.classTypePath.languageType();
                        }
                    }

                    if ( associatedSignal.isValid() ){
                        isSlot = true;
                        properties.append(QmlDeclaration::create(
                            expression,
                            QmlTypeReference(QmlTypeReference::Qml, "slot"),
                            associatedSignal.objectType,
                            propertyPosition,
                            propertyLength,
                            m_document
                        ));
                    }
                }

                if ( !isSlot ){
                    QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
                        scope.quickObjectDeclarationType(ctx->objectTypePath()), expression, cursor.position()
                    );

                    if ( propChain.size() == expression.size() && propChain.size() > 0 ){
                        QmlScopeSnap::PropertyReference& propref = propChain.last();
                        QmlTypeReference qlt = propref.resultType();
                        if ( !qlt.isEmpty() ){
                            properties.append(QmlDeclaration::create(
                                expression, propref.resultType(), propref.propertyObjectType(), propertyPosition, propertyLength, m_document
                            ));
                        }
                    }
                }
            }
        }

    } else { // multiple declarations were selected

        DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(m_document->file()->path());
        docinfo->parse(m_target->toPlainText());

        DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

        QList<DocumentQmlValueObjects::RangeProperty*> rangeProperties = objects->propertiesBetween(
            cursor.selectionStart(), cursor.selectionEnd()
        );

        if ( rangeProperties.size() > 0 ){

            QmlScopeSnap scope = d->snapScope();

            for( auto it = rangeProperties.begin(); it != rangeProperties.end(); ++it ){
                DocumentQmlValueObjects::RangeProperty* rp = *it;
                QString propertyType = rp->type();

                if ( propertyType.isEmpty() ){
                    QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
                        scope.quickObjectDeclarationType(rp->object()), rp->name(), rp->begin
                    );

                    if ( propChain.size() == rp->name().size() && propChain.size() > 0 ){
                        QmlScopeSnap::PropertyReference& propref = propChain.last();
                        properties.append(QmlDeclaration::create(
                            rp->name(),
                            propref.resultType(),
                            propref.propertyObjectType(),
                            rp->begin,
                            rp->propertyEnd - rp->begin,
                            m_document
                        ));
                    }
                } else {
                    QmlTypeReference qlt;
                    if ( QmlTypeInfo::isObject(propertyType) ){
                        QmlTypeInfo tr = scope.getType(propertyType);
                    } else {
                        qlt = QmlTypeReference(QmlTypeReference::Qml, rp->type());
                    }
                    if ( !qlt.isEmpty() ){
                        QmlTypeInfo tr = scope.getType(rp->object());
                        properties.append(QmlDeclaration::create(
                            rp->name(), qlt, tr.prefereredType(), rp->begin, rp->propertyEnd - rp->begin, m_document
                        ));
                    }
                }
            }
        }
    }

    return properties;
}

/**
 * \brief Given a declaration identifier \p position and \p length, get the \p valuePosition and \p valueEnd
 * for that declaration
 */
bool CodeQmlHandler::findDeclarationValue(int position, int length, int &valuePosition, int &valueEnd){
    if ( length > 0 ){
        DocumentQmlValueScanner vs(m_document, position, length);
        if ( vs() ){
            valuePosition = vs.valuePosition();
            valueEnd      = vs.valueEnd();
            return true;
        } else {
            valuePosition = -1;
            valueEnd      = -1;
            return false;
        }
    } else {
        DocumentQmlValueScanner vs(m_document, position, length);
        int valueLength = vs.getBlockExtent(position);
        if ( valueLength != -1 ){
            valuePosition = position;
            valueEnd      = valueLength + position;
            return true;
        } else {
            valuePosition = -1;
            valueEnd      = -1;
            return false;
        }
    }
}

/**
 * \brief Creates an injection channel between a declaration and the runtime
 *
 * This method will find all binding channels available by parsing the contents
 * of the code structure, creating a set of binding pahts that describe where
 * each component resides, and use those binding paths to connect the position of the
 * code structure to the same values within the runtime.
 */
QmlEditFragment *CodeQmlHandler::createInjectionChannel(QmlDeclaration::Ptr declaration){
    Q_D(CodeQmlHandler);
    Project* project = d->projectHandler->project();

    if ( m_document ){
        QmlBindingPath::Ptr bp = DocumentQmlInfo::findDeclarationPath(m_target->toPlainText(), m_document, declaration);
        if ( !bp )
            return nullptr;

        Runnable* r = project->runnables()->runnableAt(bp->rootFile());

        if (r && r->type() != Runnable::LvFile ){
            QmlBindingChannel::Ptr bc = DocumentQmlInfo::traverseBindingPath(bp, r);
            if ( !bc || !bc->hasConnection() )
                return nullptr;

            QmlEditFragment* ef = new QmlEditFragment(declaration);
            bc->setEnabled(true);
            ef->bindingSpan()->setExpressionPath(bp);
            ef->bindingSpan()->setInputChannel(bc);
            ef->bindingSpan()->addOutputChannel(bc);
            return ef;
        }

        QString fileName = declaration->document()->file()->name();
        if ( fileName.length() && fileName.front().isUpper() ){ // if component
            QmlEditFragment* ef = new QmlEditFragment(declaration);
            ef->bindingModel(this);
            return ef;
        }
    }

    return nullptr;
}

void CodeQmlHandler::aboutToDelete()
{
    cancelEdit();

    if ( m_document ){
        auto it = m_edits.begin();
        while( it != m_edits.end() ){
            QmlEditFragment* edit = *it;
            it = m_edits.erase(it);
            edit->emitRemoval();
            edit->deleteLater();
        }
    }
}

QJSValue CodeQmlHandler::createCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, bool canShape, bool inImports)
{
    QJSValue result = m_engine->newObject();
    result.setProperty("canBind", canBind);
    result.setProperty("canUnbind", canUnbind);
    result.setProperty("canEdit", canEdit);
    result.setProperty("canAdjust", canAdjust);
    result.setProperty("canShape", canShape);
    result.setProperty("inImports", inImports);
    return result;
}

/**
 * \brief Adds an editing fragment to the current document
 */
bool CodeQmlHandler::addEditingFragment(QmlEditFragment *edit){
    auto it = m_edits.begin();
    while ( it != m_edits.end() ){
        QmlEditFragment* itEdit = *it;

        if ( itEdit->declaration()->position() < edit->declaration()->position() ){
            break;

        } else if ( itEdit->declaration()->position() == edit->declaration()->position() ){
            qWarning("Multiple editing fragments at the same position: %d", edit->position());
            return false;
        }
        ++it;
    }

    m_edits.insert(it, edit);
    emit numberOfConnectionsChanged();

    return true;
}

/**
 * \brief Removes an editing fragment from this document
 */
void CodeQmlHandler::removeEditingFragment(QmlEditFragment *edit){
    if (!edit)
        return;

    QObject* parent = edit->parent();
    if ( parent == this ){
        auto it = m_edits.begin();

        while( it != m_edits.end() ){
            QmlEditFragment* itEdit = *it;

            if ( itEdit->declaration()->position() < edit->declaration()->position() )
                break;

            if ( itEdit == edit ){
                m_edits.erase(it);
                emit numberOfConnectionsChanged();

                if ( m_editingFragment == edit ){
                    m_document->removeEditingState(ProjectDocument::Overlay);
                    m_editingFragment = nullptr;
                }
                edit->emitRemoval();
                edit->deleteLater();
                return;
            }

            ++it;
        }
    } else {
        QmlEditFragment* parentEf = static_cast<QmlEditFragment*>(parent);
        if ( parentEf ){
            parentEf->removeChildFragment(edit);
        }
    }
}

void CodeQmlHandler::removeAllEditingFragments()
{
    while (!m_edits.empty())
    {
        removeEditingFragment(*m_edits.begin());
    }
}

int CodeQmlHandler::findImportsPosition(int blockPos)
{
    if (blockPos == -1) return 0;
    auto block = m_document->textDocument()->findBlockByNumber(blockPos);
    return block.position() + 7; // e.g. import ^QtQuick
}

int CodeQmlHandler::findRootPosition(int blockPos)
{
    auto block = m_document->textDocument()->findBlockByNumber(blockPos + 1);

    auto reg = QRegExp("\\s*");
    while (block.isValid())
    {
        if (reg.exactMatch(block.text()))
        {
            block = block.next();
            continue;
        }

        break;
    }

    if (!block.isValid()) return 0;

    int position = block.position();
    auto text = block.text();
    int i = 0;
    while (text[i].isSpace()) ++i;

    return position + i + 1;
}

QmlEditFragment *CodeQmlHandler::findEditFragment(CodePalette *palette){
    auto it = m_edits.begin();
    while( it != m_edits.end() ){
        QmlEditFragment* edit = *it;
        for ( auto it = edit->begin(); it != edit->end(); ++it ){
            CodePalette* cp = *it;
            if ( cp == palette )
                return edit;
        }

        QmlEditFragment* recurseEdit = findEditFragmentIn(edit, palette);
        if ( recurseEdit )
            return recurseEdit;

        ++it;

    }
    return nullptr;
}

QmlEditFragment *CodeQmlHandler::findEditFragmentIn(QmlEditFragment* parent, CodePalette *palette){
    for ( auto it = parent->childFragments().begin(); it != parent->childFragments().end(); ++it ){
        QmlEditFragment* edit = *it;
        for ( auto paIt = edit->begin(); paIt != edit->end(); ++paIt ){
            if ( *paIt == palette )
                return edit;
        }

        QmlEditFragment* childEdit = findEditFragmentIn(edit, palette);
        if ( childEdit )
            return childEdit;
    }
    return nullptr;
}

void CodeQmlHandler::suggestionsForProposedExpression(
        QmlDeclaration::Ptr declaration,
        const QString &expression,
        CodeCompletionModel *model,
        bool suggestFunctions) const
{
    Q_D(const CodeQmlHandler);

    QmlScopeSnap scope = d->snapScope();

    QStringList expressionPathNotTrimmed = expression.split(".");
    QStringList expressionPath;
    for ( const QString& expr : expressionPathNotTrimmed ){
        expressionPath.append(expr.trimmed());
    }

    int completionPosition = expression.lastIndexOf(".");
    completionPosition++; // will be '0' if not found

    model->setCompletionPosition(completionPosition);
    QString filter = expressionPath.size() > 0 ? expressionPath.last() : "";

    int cursorPosition = declaration->valuePosition();

    QList<CodeCompletionSuggestion> suggestions;

    if ( expressionPath.size() > 1 ){
        QStringList expressionPathHead = expressionPath;
        expressionPathHead.removeLast();

        QmlScopeSnap::ExpressionChain expression = scope.evaluateExpression(
            declaration->parentType(), expressionPathHead, cursorPosition
        );

        if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ObjectNode ){
            suggestionsForValueObject(expression.documentValueObject, suggestions, !suggestFunctions, suggestFunctions, false, suggestFunctions);
            qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, !suggestFunctions, suggestFunctions, false, false, suggestFunctions, "", suggestions);
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ClassNode ){
            suggestionsForValueObject(expression.documentValueObject, suggestions, !suggestFunctions, suggestFunctions, false, suggestFunctions);
            qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, !suggestFunctions, suggestFunctions, false, false, suggestFunctions, "", suggestions);
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::NamespaceNode ){
            suggestionsForNamespaceTypes(expression.importAs, suggestions);
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::PropertyNode ){
            qmlhandler_helpers::suggestionsForObjectPath(expression.propertyChain.last().resultTypePath, true, true, false, false, false, "", suggestions);
        }
    } else {
        suggestions << CodeCompletionSuggestion("parent", "id", "", "parent");
        suggestionsForDocumentsIds(suggestions);
        DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(cursorPosition);
        if ( !scope.document->info()->isValueNull(documentValue) )
            suggestionsForValueObject(
                scope.document->info()->extractValueObject(documentValue),
                suggestions,
                !suggestFunctions,
                suggestFunctions,
                false,
                suggestFunctions
            );

        QmlInheritanceInfo typePath = scope.getTypePath(declaration->parentType());
        qmlhandler_helpers::suggestionsForObjectPath(typePath, !suggestFunctions, suggestFunctions, suggestFunctions, false, false, "", suggestions);
    }

    if ( !suggestions.isEmpty() ){
        model->enable();
    }

    model->setSuggestions(suggestions, filter);
}

bool CodeQmlHandler::findBindingForExpression(lv::QmlEditFragment *edit, const QString &expression){
    Q_D(const CodeQmlHandler);

    QmlScopeSnap scope = d->snapScope();

//    Project* project = d->projectHandler->project();

    QStringList expressionPathNotTrimmed = expression.split(".");
    QStringList expressionPath;
    for ( const QString& expr : expressionPathNotTrimmed ){
        expressionPath.append(expr.trimmed());
    }

    int cursorPosition = edit->declaration()->position();

    QmlScopeSnap::ExpressionChain expressionChain = scope.evaluateExpression(
        edit->declaration()->parentType(), expressionPath, cursorPosition
    );

    // qDebug() << expressionChain.toString(false);

    if ( !expressionChain.isValid() ){
        //TODO: Error
        qWarning("ERROR: Failed to evaluate expression.");
        return false;
    }

    // The watcher is a child of the receiver in order to be deleted
    // when the receiver is deleted and not point to a null location

    QmlBindingPath::Ptr bp = nullptr;

    if ( expressionChain.isId ){ // <id>.<property...>
        // TODO: get binding path of id, then continue

    } else if ( expressionChain.isParent ){ // <parent...>.<property...>

        // TODO: current binding path -> parent, then append property chain to current binding path

    } else { // <property...>
        // clone current binding path, append properties to it

        bp = edit->bindingSpan()->expressionPath()->clone();
        QmlBindingPath::Node* n = bp->root();
        if ( n ){
            while ( n->child )
                n = n->child;
        }

        if ( n && n->type() == QmlBindingPath::Node::Property ){
            if ( n->parent ){
                n = n->parent;
                delete n->child;
                n->child = nullptr;
            } else {
                delete n;
                n = nullptr;
            }
        }

        for ( const QmlScopeSnap::PropertyReference& prop : expressionChain.propertyChain){
            QmlBindingPath::PropertyNode* pn = new QmlBindingPath::PropertyNode;
            pn->propertyName = prop.property.name;
            pn->objectName = QStringList() << prop.classTypePath.nodes.first().prefereredType().join();

            if ( n ){
                pn->parent = n;
                n->child = pn;
            }
        }
    }

    QList<QmlBindingChannel::Ptr> bchannels = edit->bindingSpan()->outputChannels();
    for ( auto it = bchannels.begin(); it != bchannels.end(); ++it ){
        QmlBindingChannel::Ptr receivingChannel = *it;
        if ( receivingChannel->isEnabled() ){
            QmlBindingChannel::Ptr writeChannel = DocumentQmlInfo::traverseBindingPath(bp, receivingChannel->runnable());

            if ( !writeChannel->hasConnection() ){
                qWarning("Failed to find binding channel at: %s", qPrintable(writeChannel->bindingPath()->toString()));
            } else {
                //TODO: Check if receiver already has a watcher on this property and remove it

                QmlPropertyWatcher* watcher = new QmlPropertyWatcher(writeChannel->property());

                if ( receivingChannel->property().isValid() && receivingChannel->property().object() ){
                    QQmlProperty pp = receivingChannel->property();
                    pp.write(watcher->read());
                    watcher->onChange([pp](const QmlPropertyWatcher& watcher){
                        pp.write(watcher.read());
                    });
                    watcher->setParent(receivingChannel->property().object());
                }
            }
        }
    }
    return true;
}

bool CodeQmlHandler::findFunctionBindingForExpression(QmlEditFragment *edit, const QString &expression){
    Q_D(const CodeQmlHandler);

    QmlScopeSnap scope = d->snapScope();

//    Project* project = d->projectHandler->project();

    QStringList expressionPathNotTrimmed = expression.split(".");
    QStringList expressionPath;
    for ( const QString& expr : expressionPathNotTrimmed ){
        expressionPath.append(expr.trimmed());
    }

    int cursorPosition = edit->declaration()->position();

    // qDebug() << expressionPath;

    QmlScopeSnap::ExpressionChain expressionChain = scope.evaluateExpression(
        edit->declaration()->parentType(), expressionPath, cursorPosition
    );

    if ( !expressionChain.isValid() ){
        //TODO: Error
        qWarning("ERROR: Failed to evaluate expression.");
        return false;
    }

    // The watcher is a child of the receiver in order to be deleted
    // when the receiver is deleted and not point to a null location

    QmlBindingPath::Ptr bp = nullptr;

    if ( expressionChain.isId ){ // <id>.<property...>s
        DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueForId(expressionPath.first());

        int begin;
        int end;
        scope.document->info()->extractTypeNameRange(documentValue, begin, end);

        QTextCursor cursor(m_document->textDocument());
        cursor.setPosition(end);
        QList<QmlDeclaration::Ptr> declarations = getDeclarations(cursor);

        QmlBindingPath::Ptr newBp = DocumentQmlInfo::findDeclarationPath(
            m_document->textDocument()->toPlainText(),
            m_document,
            declarations.first());

        bp = newBp;

        QmlBindingPath::Node* n = bp->root();
        if ( n ){
            while ( n->child )
                n = n->child;
        }

        for ( const QmlScopeSnap::PropertyReference& prop : expressionChain.propertyChain){
            QmlBindingPath::PropertyNode* pn = new QmlBindingPath::PropertyNode;
            QString propertyName = prop.property.isValid() ? prop.property.name : prop.functionInfo.definition();
            pn->propertyName = propertyName;
            pn->objectName = QStringList() << prop.classTypePath.nodes.first().prefereredType().join();
            if ( n ){
                pn->parent = n;
                n->child = pn;
            }
        }

    } else if ( expressionChain.isParent ){ // <parent...>.<property...>

        // TODO: current binding path -> parent, then append property chain to current binding path

    } else { // <property...>
        // clone current binding path, append properties to it

        bp = edit->bindingSpan()->expressionPath()->clone();
        QmlBindingPath::Node* n = bp->root();
        if ( n ){
            while ( n->child )
                n = n->child;
        }

        if ( n && n->type() == QmlBindingPath::Node::Property ){
            if ( n->parent ){
                n = n->parent;
                delete n->child;
                n->child = nullptr;
            } else {
                delete n;
                n = nullptr;
            }
        }

        for ( const QmlScopeSnap::PropertyReference& prop : expressionChain.propertyChain){
            QmlBindingPath::PropertyNode* pn = new QmlBindingPath::PropertyNode;
            QString propertyName = prop.property.isValid() ? prop.property.name : prop.functionInfo.definition();
            pn->propertyName = propertyName;
            pn->objectName = QStringList() << prop.classTypePath.nodes.first().prefereredType().join();
            if ( n ){
                pn->parent = n;
                n->child = pn;
            }
        }
    }

    if ( bp.isNull() )
        return false;

    QList<QmlBindingChannel::Ptr> bchannels = edit->bindingSpan()->outputChannels();
    for ( auto it = bchannels.begin(); it != bchannels.end(); ++it ){
        QmlBindingChannel::Ptr signalChannel = *it;
        if ( signalChannel->isEnabled() ){
            QmlBindingChannel::Ptr functionChannel = DocumentQmlInfo::traverseBindingPath(bp, signalChannel->runnable());
            // qDebug() << functionChannel->hasConnection();
            if ( functionChannel.isNull() || !functionChannel->hasConnection() ){
                qWarning("Failed to find binding channel at: %s", qPrintable(bp->toString()));
            } else {
                if ( functionChannel->method().isValid() && functionChannel->property().object() ){
                    QMetaMethod signalMethod = signalChannel->property().method();
                    QMetaMethod slotMethod = functionChannel->method();
                    connect(signalChannel->property().object(), signalMethod, functionChannel->property().object(), slotMethod);
                }
            }
        }
    }

    return true;
}

QmlUsageGraphScanner *CodeQmlHandler::createScanner(){
    Q_D(CodeQmlHandler);
    return new QmlUsageGraphScanner(d->projectHandler->project(), d->snapScope());
}

int CodeQmlHandler::numberOfConnections()
{
    return m_edits.size();
}

QList<int> CodeQmlHandler::languageFeatures() const{
    return {
        DocumentHandler::LanguageHelp,
        DocumentHandler::LanguageScope,
        DocumentHandler::LanguageHighlighting,
        DocumentHandler::LanguageCodeCompletion
    };
}

QString CodeQmlHandler::help(int position){
    return getHelpEntity(position);
}

QmlEditFragment *CodeQmlHandler::openConnection(int position, QObject* /*currentApp*/){
    Q_D(CodeQmlHandler);

    if ( !m_document )
        return nullptr;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    bool inImports = isInImports(position);

    if ( properties.isEmpty() && !inImports)
        return nullptr;

    if (inImports)
    {
        QmlDeclaration::Ptr importDecl = QmlDeclaration::create(
            QStringList(),
            QmlTypeReference(QmlTypeReference::Qml, "import"),
            QmlTypeReference(QmlTypeReference::Qml), m_document);
        QmlEditFragment* ef = new QmlEditFragment(importDecl);

        auto model = importsModel();

        int startPosition = m_target->findBlockByNumber(model->firstBlock()).position();
        auto lastBlock = m_target->findBlockByNumber(model->lastBlock());
        int length = lastBlock.position() + lastBlock.length() - startPosition;
        ef->declaration()->setSection(
            m_document->createSection(QmlEditFragment::Section, startPosition, length)
        );
        ef->declaration()->section()->setUserData(ef);

        ef->declaration()->section()->onTextChanged(
                    [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
        {
            auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());
            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
        });

        addEditingFragment(ef);
        ef->setParent(this);
        DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
        if ( dh )
            dh->requestCursorPosition(ef->position());

        return ef;
    }

    QmlDeclaration::Ptr declaration = properties.first();

    for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
        QmlEditFragment* edit = *it;
        if ( edit->declaration()->position() == declaration->position() )
            return edit;
    }

    QmlEditFragment* ef = createInjectionChannel(declaration);

    if ( !ef ){
        return nullptr;
    }

    QTextCursor codeCursor(m_target);
    codeCursor.setPosition(ef->valuePosition());
    codeCursor.setPosition(ef->valuePosition() + ef->valueLength(), QTextCursor::KeepAnchor);

    ef->declaration()->setSection(m_document->createSection(
        QmlEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
    ));
    ef->declaration()->section()->setUserData(ef);
    ef->declaration()->section()->onTextChanged(
                [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
    {
        auto projectDocument = section->document();
        auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

        } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
            removeEditingFragment(editingFragment);
        } else {
            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
        }
    });

    QmlBindingChannel::Ptr inputChannel = ef->bindingSpan()->inputChannel();
    if ( inputChannel && inputChannel->listIndex() == -1 ){
        inputChannel->property().connectNotifySignal(ef, SLOT(updateValue()));
    }

    addEditingFragment(ef);
    ef->setParent(this);

    rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(ef->valuePosition());

    return ef;
}

QmlEditFragment *CodeQmlHandler::openNestedConnection(QmlEditFragment* editParent, int position){
    //TODO: Fix this with relative binding paths
    if ( !m_document || !editParent )
        return nullptr;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    if ( properties.isEmpty() )
        return nullptr;

    QmlDeclaration::Ptr declaration = properties.first();

    for ( auto it = editParent->childFragments().begin(); it != editParent->childFragments().end(); ++it ){
        QmlEditFragment* edit = *it;
        if ( edit->declaration()->position() == declaration->position() )
            return edit;
    }

    QmlEditFragment* ef = createInjectionChannel(declaration);
    if ( !ef ){
        return nullptr;
    }

    QTextCursor codeCursor(m_target);
    codeCursor.setPosition(ef->valuePosition());
    codeCursor.setPosition(ef->valuePosition() + ef->valueLength(), QTextCursor::KeepAnchor);

    ef->declaration()->setSection(m_document->createSection(
        QmlEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
    ));
    ef->declaration()->section()->setUserData(ef);
    ef->declaration()->section()->onTextChanged(
                [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
    {
        auto projectDocument = section->document();
        auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

        } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
            removeEditingFragment(editingFragment);
        } else {
            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
        }
    });

    QmlBindingChannel::Ptr inputChannel = ef->bindingSpan()->inputChannel();
    if ( inputChannel && inputChannel->listIndex() == -1 ){
        inputChannel->property().connectNotifySignal(ef, SLOT(updateValue()));
    }

    editParent->addChildFragment(ef);
    ef->setParent(editParent);

    rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(ef->valuePosition());

    return ef;
}

QList<QObject *> CodeQmlHandler::openNestedObjects(QmlEditFragment *edit){
    Q_D(CodeQmlHandler);

    QList<QObject*> fragments;

    QmlScopeSnap scope = d->snapScope();

    QString source = m_target->toPlainText();
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(m_document->file()->path());
    docinfo->parse(source);

    DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
    DocumentQmlValueObjects::RangeObject* currentOb = objects->objectAtPosition(edit->position());

    if ( currentOb ){
        for ( int i = 0; i < currentOb->children.size(); ++i ){
            DocumentQmlValueObjects::RangeObject* child = currentOb->children[i];
            QString currentObDeclaration = m_document->substring(child->begin, child->identifierEnd - child->begin);
            int splitter = currentObDeclaration.indexOf('.');
            QString obName = currentObDeclaration.mid(splitter + 1);
            QString obNs   = splitter == -1 ? "" : currentObDeclaration.mid(0, splitter);

            QmlInheritanceInfo obPath = scope.getTypePath(obNs, obName);

            if ( !obPath.isEmpty() ){

                QmlDeclaration::Ptr declaration = QmlDeclaration::create(
                    QStringList(),
                    obPath.languageType(),
                    QmlTypeReference(),
                    child->begin,
                    0,
                    m_document
                );
                declaration->setValuePositionOffset(0);
                declaration->setValueLength(child->end - child->begin);

                QmlBindingPath::Ptr bp = QmlBindingPath::create();
                bp->appendIndex(i);

                QmlEditFragment* ef = new QmlEditFragment(declaration);
                ef->declaration()->setSection(m_document->createSection(
                    QmlEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
                ));
                ef->declaration()->section()->setUserData(ef);
                ef->declaration()->section()->onTextChanged(
                            [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
                {
                    auto projectDocument = section->document();
                    auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

                    if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

                        int length = editingFragment->declaration()->valueLength();
                        editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

                    } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
                        removeEditingFragment(editingFragment);
                    } else {
                        int length = editingFragment->declaration()->valueLength();
                        editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
                    }
                });

                edit->addChildFragment(ef);
                ef->setParent(edit);

                ef->setRelativeBinding(bp);

                fragments.append(ef);

            }
        }
    }

    return fragments;
}

void CodeQmlHandler::removeConnection(QmlEditFragment *edit){
    removeEditingFragment(edit);
}

void CodeQmlHandler::deleteObject(QmlEditFragment *edit){
    QList<QObject*> toRemove;
    QList<QmlBindingChannel::Ptr> channels = edit->bindingSpan()->outputChannels();
    for ( auto it = channels.begin(); it != channels.end(); ++it ){
        const QmlBindingChannel::Ptr& bc = *it;
        if ( bc->isEnabled() ){
            if ( bc->property().propertyTypeCategory() == QQmlProperty::List ){
                QQmlListReference ppref = qvariant_cast<QQmlListReference>(bc->property().read());
                if (ppref.canAt()){
                    toRemove.append(ppref.at(bc->listIndex()));
                }
            }
        }
    }

    int pos = edit->declaration()->valuePosition();
    int len = edit->declaration()->valueLength();

    removeConnection(edit);

    m_document->addEditingState(ProjectDocument::Runtime);
    m_document->insert(pos, len, "");
    m_document->removeEditingState(ProjectDocument::Runtime);

    for ( QObject* o : toRemove )
        o->deleteLater();
}

/**
 * \brief Finds the available list of palettes at the current \p cursor position
 *
 * The \p unrepeated parameter makes sure the palettes that are found at that
 * \p position are not already opened.
 */
lv::PaletteList* CodeQmlHandler::findPalettes(int position, bool unrepeated){
    Q_D(CodeQmlHandler);
    if ( !m_document )
        return nullptr;

    cancelEdit();

    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    bool inImports = isInImports(position);
    if ( properties.isEmpty() && !inImports )
        return nullptr;

    if (inImports){
        PaletteList* importList = d->projectHandler->paletteContainer()->findPalettes("qml/import");
        importList->setPosition(position);
        return importList;
    }

    QmlDeclaration::Ptr declaration = properties.first();

    PaletteList* lpl = d->projectHandler->paletteContainer()->findPalettes(declaration->type().join());
    if ( declaration->isListDeclaration() ){
        lpl = d->projectHandler->paletteContainer()->findPalettes("qml/childlist", lpl);
    } else {
        lpl = d->projectHandler->paletteContainer()->findPalettes("qml/property", lpl);
    }

    lpl->setPosition(declaration->position());
    if ( unrepeated ){
        for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
            QmlEditFragment* edit = *it;
            if ( edit->declaration()->position() < declaration->position() ){
                break;
            } else if ( edit->declaration()->position() == declaration->position()  ){

                for ( auto it = edit->begin(); it != edit->end(); ++it ){
                    CodePalette* loadedPalette = *it;

                    for ( int i = 0; i < lpl->size(); ++i ){
                        PaletteLoader* loader = lpl->loaderAt(i);
                        if ( PaletteContainer::palettePath(loader) == loadedPalette->path() ){
                            lpl->remove(loader);
                        }
                    }
                }

            }
        }
    }
    return lpl;
}

/**
 * \brief Opens the palette \p index from a given \p paletteList
 */
lv::CodePalette* CodeQmlHandler::openPalette(lv::QmlEditFragment* edit, lv::PaletteList *paletteList, int index){
    if ( !m_document || !edit || !paletteList )
        return nullptr;

    // check if duplicate
    PaletteLoader* paletteLoader = paletteList->loaderAt(index);

    for ( auto it = edit->begin(); it != edit->end(); ++it ){
        CodePalette* loadedPalette = *it;
        if ( loadedPalette->path() == PaletteContainer::palettePath(paletteLoader) ){
            return loadedPalette;
        }
    }

    if ( edit->bindingPalette() && edit->bindingPalette()->path() == PaletteContainer::palettePath(paletteLoader) ){
        edit->addPalette(edit->bindingPalette());
        return edit->bindingPalette();
    }

    CodePalette* palette = paletteList->loadAt(index);
    QmlCodeConverter* cvt = new QmlCodeConverter(edit, palette);
    palette->setExtension(cvt, true);
    edit->addPalette(palette);
    edit->updatePaletteValue(palette);


    connect(palette, &CodePalette::valueChanged, cvt, &QmlCodeConverter::updateFromPalette);

    rehighlightSection(edit->position(), edit->valuePosition() + edit->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(edit->valuePosition());

    return palette;
}

bool CodeQmlHandler::isForAnObject(lv::QmlEditFragment *ef){
    return isForAnObject(ef->declaration());
}

/**
 * \brief Removes a palette given its container object.
 */
lv::QmlEditFragment *CodeQmlHandler::removePalette(lv::CodePalette *palette){
    if ( !palette )
        return nullptr;

    lv::QmlEditFragment* edit = findEditFragment(palette);
    if ( edit ){
        edit->removePalette(palette);
    }
    return edit;
}

/**
 * \brief Open a specific binding palette for \p edit fragment given a \p paletteList and an \p index
 *
 * \returns A pointer to the opened lv::CodePalette.
 */
CodePalette *CodeQmlHandler::openBinding(QmlEditFragment *edit, PaletteList *paletteList, int index){
    if ( !m_document || !edit || !paletteList )
        return nullptr;

    // check if duplicate
    PaletteLoader* paletteLoader = paletteList->loaderAt(index);
    if ( edit->bindingPalette() ){
        if ( edit->bindingPalette()->path() == PaletteContainer::palettePath(paletteLoader) )
            return edit->bindingPalette();
    }
    CodePalette* cp = edit->palette(PaletteContainer::palettePath(paletteLoader));
    if ( cp ){
        edit->setBindingPalette(cp);
        return cp;
    }

    CodePalette* palette = paletteList->loadAt(index);
    QmlCodeConverter* cvt = new QmlCodeConverter(edit, palette);
    palette->setExtension(cvt, true);
    edit->setBindingPalette(palette);
    edit->updatePaletteValue(palette);

    connect(palette, &CodePalette::valueChanged, cvt, &QmlCodeConverter::updateFromPalette);

    rehighlightSection(edit->position(), edit->valuePosition() + edit->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(edit->valuePosition());

    return palette;

}

/**
 * \brief Integrates a given palette box within the editor
 */
void CodeQmlHandler::frameEdit(QQuickItem *box, lv::QmlEditFragment *edit){
    if (!edit)
        return;

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());

    int pos = edit->declaration()->position();
    QTextBlock tb = m_document->textDocument()->findBlock(pos);
    QTextBlock tbend = m_document->textDocument()->findBlock(pos + edit->declaration()->length());

    dh->lineBoxAdded(tb.blockNumber() + 1, tbend.blockNumber() + 1, static_cast<int>(box->height()), box);
}

/**
 * \brief Finds the boundaries of the code block containing the cursor position
 *
 * Mostly used for fragments
 */
QJSValue CodeQmlHandler::contextBlockRange(int position){
    DocumentQmlValueScanner vs(m_document, position, 0);
    int start = vs.getBlockStart(position);
    int end   = vs.getBlockEnd(start + 1);
    QJSValue ob = m_engine->newObject();

    ob.setProperty("start", m_target->findBlock(start).blockNumber());
    ob.setProperty("end", m_target->findBlock(end).blockNumber());

    return ob;
}

lv::QmlImportsModel *CodeQmlHandler::importsModel()
{
    lv::QmlImportsModel* result = new lv::QmlImportsModel(this);

    QString source = m_target->toPlainText();
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(m_document->file()->path());
    if ( docinfo->parse(source) ){
        auto imports = docinfo->internalBind()->imports();
        for ( QList<QmlJS::ImportInfo>::iterator it = imports.begin(); it != imports.end(); ++it ){

            QString module = it->name();
            QString version = it->version().majorVersion() != -1 ? (QString::number(it->version().majorVersion()) + "." + QString::number(it->version().minorVersion())) : "";
            QString qual = it->as();
            int line = it->ast()? static_cast<int>(it->ast()->importToken.startLine-1) : -1;

            result->addItem(module, version, qual, line);
        }
    }

    return result;
}

void CodeQmlHandler::addLineAtPosition(QString line, int pos)
{
    if (!m_target) return;
    QTextCursor c(m_target);
    if (pos == 0)
    {
        c.beginEditBlock();
        c.insertText(line + "\n");
        c.endEditBlock();
    } else {
        auto block = m_target->findBlockByNumber(pos-1);
        c = QTextCursor(block);
        c.beginEditBlock();
        c.movePosition(QTextCursor::EndOfBlock);
        c.insertText("\n" + line);
        c.endEditBlock();
    }
}

void CodeQmlHandler::removeLineAtPosition(int pos)
{
    if (!m_target) return;
    auto block = m_target->findBlockByNumber(pos);
    QTextCursor c(block);
    c.beginEditBlock();
    c.select(QTextCursor::BlockUnderCursor);
    c.removeSelectedText();
    c.endEditBlock();
}

/**
 * \brief Receive different qml based information about a given cursor position
 */
QJSValue CodeQmlHandler::cursorInfo(int position, int length){
    Q_D(CodeQmlHandler);
    bool canBind = false, canUnbind = false, canEdit = false, canAdjust = false, canShape = false;

    if ( !m_document )
        return createCursorInfo(canBind, canUnbind, canEdit, canAdjust, canShape);

    QTextCursor cursor(m_target);
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
//    qDebug() << properties.length();
//    if ( properties.length() ){

//    }

    QmlCompletionContext::ConstPtr qcc = m_completionContextFinder->getContext(cursor);
    if ( qcc->context() & QmlCompletionContext::InImport || qcc->context() & QmlCompletionContext::InImportVersion ){
        canShape = true;
    }

    if (qcc->context() == 0 && importsModel()->rowCount() == 0)
    {
        // check if blank
        if (position == 0) canShape = true;
        else {
            bool isBlank = true;
            for (int i = 0; i<position; i++)
            {
                if (!m_document->textDocument()->characterAt(i).isSpace())
                {
                    isBlank = false;
                    break;
                }
            }
            if (isBlank) canShape = true;
        }

    }

    if ( properties.isEmpty())
        return createCursorInfo(canBind, canUnbind, canEdit, canAdjust, canShape);

    if ( properties.size() == 1 ){
        QmlDeclaration::Ptr firstdecl = properties.first();

        if ( isForAnObject(firstdecl) )
            canShape = true;

        canEdit = true;

        int paletteCount = d->projectHandler->paletteContainer()->countPalettes(firstdecl->type().join());
        if ( paletteCount > 0 ){
            int totalLoadedPalettes = 0;
            canBind = true;
            canShape = true;

            for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
                QmlEditFragment* edit = *it;
                if ( edit->declaration()->position() == firstdecl->position() ){
                    if ( edit->bindingPalette() ){
                        canBind = false;
                        canUnbind = true;
                    }

                    for ( auto paIt = edit->begin(); paIt != edit->end(); ++paIt ){
                        CodePalette* cp = *paIt;
                        if ( cp->type() == "edit/qml" )
                            canEdit = false;
                        else
                            ++totalLoadedPalettes;
                    }
                }
            }

            if ( totalLoadedPalettes < paletteCount ){
                canAdjust = true;
            }
        }
    }
    return createCursorInfo(canBind, canUnbind, canEdit, canAdjust, canShape);
}

bool CodeQmlHandler::isInImports(int position)
{
    QTextCursor cursor(m_target);
    cursor.setPosition(position);


    QmlCompletionContext::ConstPtr qcc = m_completionContextFinder->getContext(cursor);
    if ( qcc->context() & QmlCompletionContext::InImport || qcc->context() & QmlCompletionContext::InImportVersion ){
        return true;
    }

    if (qcc->context() == 0 && importsModel()->rowCount() == 0)
    {
        // check if blank
        if (position == 0) return true;
        else {
            bool isBlank = true;
            for (int i = 0; i<position; i++)
            {
                if (!m_document->textDocument()->characterAt(i).isSpace())
                {
                    isBlank = false;
                    break;
                }
            }
            if (isBlank) return true;
        }

    }

    return false;
}

/**
 * \brief Closes the bindings between the given position and length
 */
void CodeQmlHandler::closeBinding(int position, int length){
    if ( !m_document )
        return;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    for ( QList<QmlDeclaration::Ptr>::iterator it = properties.begin(); it != properties.end(); ++it ){
        int position = (*it)->position();

        auto editIt = m_edits.begin();
        while ( editIt != m_edits.end() ){
            QmlEditFragment* edit = *editIt;

            if ( edit->declaration()->position() == position ){
                if ( edit->bindingPalette() ){

                    edit->removeBindingPalette();

                    if ( edit->totalPalettes() == 0 ){
                        editIt = m_edits.erase(editIt);
                        edit->emitRemoval();
                        edit->deleteLater();
                    }
                    break;
                }
            } else {
                ++editIt;
            }
        }
    }

    rehighlightSection(position, position + length);
}

/**
 * \brief Opens an editing palette for the given \p edit fragment
 */
lv::CodePalette* CodeQmlHandler::edit(lv::QmlEditFragment *edit){
    Q_D(CodeQmlHandler);
    if ( !m_document )
        return nullptr;

    QList<QmlEditFragment*> toRemove;

    for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
        QmlEditFragment* curentEdit = *it;
        if ( curentEdit->declaration()->position() < edit->declaration()->position() ){
            break;
        } else if ( curentEdit->declaration()->position() == edit->declaration()->position() ){
            if ( curentEdit->totalPalettes() == 1 && (*curentEdit->begin())->type() == "edit/qml" )
                 return nullptr;

            vlog("editqmljs-codehandler").v() <<
                "Removing editing frag  with no of palettes \'" << curentEdit->totalPalettes() << "\' at " <<
                edit->declaration()->position() << " due to edit.";

            if ( curentEdit != edit )
                toRemove.append(curentEdit);
        }
    }


    for ( auto it = toRemove.begin(); it != toRemove.end(); ++it ){
        removeEditingFragment(*it);
    }
    if ( m_editingFragment && m_editingFragment != edit )
        removeEditingFragment(m_editingFragment);

    PaletteLoader* loader = d->projectHandler->paletteContainer()->findPalette("edit/qml");
    CodePalette* palette = d->projectHandler->paletteContainer()->createPalette(loader);

    edit->addPalette(palette);

    edit->declaration()->section()->onTextChanged([](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText){
        auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        int length = editingFragment->declaration()->valueLength();
        editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
    });

    palette->setExtension(new QmlCodeConverter(edit, this), true);

    connect(palette, &CodePalette::valueChanged, [this, edit](){
        if ( edit->totalPalettes() > 0 ){
            CodePalette* cp = *edit->begin();
            edit->bindingSpan()->commit(cp->value());
        }
        m_document->removeEditingState(ProjectDocument::Overlay);
        removeEditingFragment(edit);
    });

    m_document->addEditingState(ProjectDocument::Overlay);
    m_editingFragment = edit;

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    rehighlightSection(edit->valuePosition(), edit->valuePosition() + edit->valueLength());
    if ( dh ){
        dh->requestCursorPosition(edit->valuePosition());
    }

    return palette;
}

/**
 * \brief Cancels the current editing palette
 */
void CodeQmlHandler::cancelEdit(){
    m_document->removeEditingState(ProjectDocument::Overlay);
    if ( m_editingFragment ){
        removeEditingFragment(m_editingFragment);
    }
}

/**
 * \brief Get the insertion options at the given \p position
 *
 * Returns an lv::QmlAddContainer for all the options
 */
QmlAddContainer *CodeQmlHandler::getAddOptions(int position){
    Q_D(CodeQmlHandler);
    if ( !m_document || !m_target )
        return nullptr;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);
    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

    ctx->expressionPath();

    QStringList expression;
    int propertyPosition = -1;
    int propertyLength   = 0;

    QChar expressionEndDelimiter;

    if ( ctx->context() & QmlCompletionContext::InLhsOfBinding ||
         ctx->context() & QmlCompletionContext::InAfterOnLhsOfBinding){
        expression = ctx->expressionPath();

        int advancedLength = DocumentQmlValueScanner::getExpressionExtent(
            m_target, cursor.position(), &expression, &expressionEndDelimiter
        );
        propertyLength = (cursor.position() - propertyPosition) + advancedLength;
        propertyPosition = cursor.position();

    } else if ( ctx->context() & QmlCompletionContext::InRhsofBinding )
    {
        expression     = ctx->propertyPath();
        propertyLength = DocumentQmlValueScanner::getExpressionExtent(m_target, ctx->propertyPosition());
        propertyPosition = ctx->propertyPosition();
    }

    if ( propertyPosition == -1 )
        return nullptr;

    QmlAddContainer* addContainer = new QmlAddContainer(propertyPosition, ctx->objectTypePath());
    QmlScopeSnap scope = d->snapScope();

    if ( !ctx->objectTypePath().empty() ){
        QString type = ctx->objectTypeName();
        QString typeNamespace = ctx->objectTypePath().size() > 1 ? ctx->objectTypePath()[0] : "";
        QmlInheritanceInfo typePath = scope.getTypePath(typeNamespace, type);

        for ( auto it = typePath.nodes.begin(); it != typePath.nodes.end(); ++it ){
            LanguageUtils::FakeMetaObject::ConstPtr object = QmlTypeInfoPrivate::typeObject(*it);

            QString objectTypeName = "";
            if ( object->exports().size() > 0 )
                objectTypeName = object->exports().first().type;

            for (int i = 0; i<object->methodCount(); ++i)
            {
                auto method = object->method(i);
                if (method.methodType() == LanguageUtils::FakeMetaMethod::Signal)
                {
                    auto name = method.methodName();
                    name = QString("on") + name[0].toUpper() + name.mid(1);


                    addContainer->eventModel()->addItem(QmlEventModel::EventData(
                        name,
                        objectTypeName,
                        "method",
                        "", //TODO: Find library path
                        name
                    ));

                }
            }

            for ( int i = 0; i < object->propertyCount(); ++i ){
                if ( !object->property(i).name().startsWith("__") ){
                    addContainer->propertyModel()->addItem(QmlPropertyModel::PropertyData(
                        object->property(i).name(),
                        objectTypeName,
                        object->property(i).typeName(),
                        "", //TODO: Find library path
                        object->property(i).name())
                    );
                }
            }
            addContainer->propertyModel()->updateFilters();
        }
    }

    // import global objects


    QStringList exports;
    scope.project->implicitLibraries()->libraryInfo(scope.document->path())->listExports(&exports);

    foreach( const QString& e, exports ){
        if ( e != scope.document->componentName() ){
            addContainer->itemModel()->addItem(
                QmlItemModel::ItemData(e, "implicit", scope.document->path(), e)
            );
        }
    }

    foreach( const QString& defaultLibrary, scope.project->defaultLibraries() ){
        QStringList exports;
        scope.project->globalLibraries()->libraryInfo(defaultLibrary)->listExports(&exports);
        foreach( const QString& e, exports ){
            addContainer->itemModel()->addItem(
                QmlItemModel::ItemData(e, "QtQml", "QtQml", e)
            );
        }
    }

    // import namespace objects

    QSet<QString> imports;

    foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
        if ( imp.first.as() != "" ){
            imports.insert(imp.first.as());
        }
    }
    imports.insert("");

    for ( QSet<QString>::iterator it = imports.begin(); it != imports.end(); ++it ){

        foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
            if ( imp.first.as() == *it ){
                QStringList exports;
                scope.project->globalLibraries()->libraryInfo(imp.second)->listExports(&exports);
                int segmentPosition = imp.second.lastIndexOf('/');
                QString libraryName = imp.second.mid(segmentPosition + 1);

                foreach( const QString& e, exports ){
                    addContainer->itemModel()->addItem(
                        QmlItemModel::ItemData(e, libraryName, libraryName, e)
                    );
                }
            }
        }
    }

    addContainer->itemModel()->updateFilters();

    return addContainer;
}

/**
 * \brief Add a property given the \p addText at the specified \p position
 */
int CodeQmlHandler::addProperty(
        int position,
        const QString &object,
        const QString &type,
        const QString &name,
        bool assignDefault)
{
    DocumentQmlValueScanner qvs(m_document, position, 1);
    int blockStart = qvs.getBlockStart(position) + 1;
    int blockEnd = qvs.getBlockEnd(position);

    QTextBlock tbStart = m_target->findBlock(blockStart);
    QTextBlock tbEnd   = m_target->findBlock(blockEnd);

    QString insertionText;
    int insertionPosition = position;
    int cursorPosition = position;

    // Check wether the property is already added

    QTextCursor sourceSelection(m_target);
    sourceSelection.setPosition(blockStart);
    sourceSelection.setPosition(blockEnd, QTextCursor::KeepAnchor);

    QString source = object + "{" + sourceSelection.selectedText().replace(QChar(QChar::ParagraphSeparator), "\n") + "}";

    lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create(m_document->file()->path());
    if ( docinfo->parse(source) ){
        lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
        lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

        for ( auto it = root->properties.begin(); it != root->properties.end(); ++it ){
            lv::DocumentQmlValueObjects::RangeProperty* p = *it;
            QString propertyName = source.mid(p->begin, p->propertyEnd - p->begin);

            if ( propertyName == name ){ // property already exists, simply position the cursor accordingly
                lv::DocumentHandler* dh = static_cast<DocumentHandler*>(parent());

                int sourceOffset = blockStart - 1 - object.size();

                if ( dh ){
                    dh->requestCursorPosition(sourceOffset + p->valueBegin);
                }
                return sourceOffset + p->begin;
            }
        }
    }

    // Check where to insert the property

    if ( tbStart == tbEnd ){ // inline object declaration
        insertionPosition = blockEnd;
        insertionText = "; " + name + ": " + (assignDefault ? QmlTypeInfo::typeDefaultValue(type) : "");
        cursorPosition = insertionPosition + name.size() + 4;
    } else { // multiline object declaration
        QString indent = getBlockIndent(tbStart);
        insertionPosition = tbEnd.position();
        QTextBlock tbIt = tbEnd.previous();
        bool found = false;
        while ( tbIt != tbStart && tbIt.isValid() ){
            if ( !isBlockEmptySpace(tbIt) ){
                indent = getBlockIndent(tbIt);
                insertionPosition = tbIt.position() + tbIt.length();
                found = true;
                break;
            }
            tbIt = tbIt.previous();
        }

        insertionText = indent + (found ? "" : "    ")+ name + ": " + (assignDefault ? QmlTypeInfo::typeDefaultValue(type) : "") + "\n";
        cursorPosition = insertionPosition + indent.size() + (found ? 0:4) + name.size() + 2;
    }


    m_document->addEditingState(ProjectDocument::Palette);
    QTextCursor cs(m_target);
    cs.setPosition(insertionPosition);
    cs.beginEditBlock();
    cs.insertText(insertionText);
    cs.endEditBlock();
    m_document->removeEditingState(ProjectDocument::Palette);

    m_scopeTimer.stop();
    updateScope();

    lv::DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh ){
        dh->requestCursorPosition(cursorPosition);
    }

    return cursorPosition - name.size() - 2;
}

int CodeQmlHandler::addEvent(
        int position,
        const QString &object,
        const QString &type,
        const QString &name)
{
    DocumentQmlValueScanner qvs(m_document, position, 1);
    int blockStart = qvs.getBlockStart(position) + 1;
    int blockEnd = qvs.getBlockEnd(position);

    QTextBlock tbStart = m_target->findBlock(blockStart);
    QTextBlock tbEnd   = m_target->findBlock(blockEnd);

    QString insertionText;
    int insertionPosition = position;
    int cursorPosition = position;

    // Check whether the property is already added

    QTextCursor sourceSelection(m_target);
    sourceSelection.setPosition(blockStart);
    sourceSelection.setPosition(blockEnd, QTextCursor::KeepAnchor);

    QString source = object + "{" + sourceSelection.selectedText().replace(QChar(QChar::ParagraphSeparator), "\n") + "}";

    lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create(m_document->file()->path());
    if ( docinfo->parse(source) ){
        lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
        lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

        for ( auto it = root->properties.begin(); it != root->properties.end(); ++it ){
            lv::DocumentQmlValueObjects::RangeProperty* p = *it;
            QString propertyName = source.mid(p->begin, p->propertyEnd - p->begin);
            if ( propertyName == name ){ // property already exists, simply position the cursor accordingly
                lv::DocumentHandler* dh = static_cast<DocumentHandler*>(parent());

                int sourceOffset = blockStart - 1 - object.size();

                if ( dh ){
                    dh->requestCursorPosition(sourceOffset + p->valueBegin);
                }
                return sourceOffset + p->begin;
            }
        }
    }

    // Check where to insert the property

    if ( tbStart == tbEnd ){ // inline object declaration
        insertionPosition = blockEnd;
        insertionText = "; " + name + ": {    }    ";
        cursorPosition = insertionPosition + name.size() + 5;
    } else { // multiline object declaration
        QString indent = getBlockIndent(tbStart);
        insertionPosition = tbEnd.position();
        QTextBlock tbIt = tbEnd.previous();
        bool found = false;
        while ( tbIt != tbStart && tbIt.isValid() ){
            if ( !isBlockEmptySpace(tbIt) ){
                indent = getBlockIndent(tbIt);
                insertionPosition = tbIt.position() + tbIt.length();
                found = true;
                break;
            }
            tbIt = tbIt.previous();
        }

        insertionText = indent + (found ? "" : "    ")+ name + ": {\n"
                + indent +(found ? "    " : "        ")+ "\n"
                + indent + (found ? "" : "    ")+"}\n";
        cursorPosition = insertionPosition + 2*indent.size() + (found ? 4:12) + name.size() + 4;
    }


    m_document->addEditingState(ProjectDocument::Palette);
    QTextCursor cs(m_target);
    cs.setPosition(insertionPosition);
    cs.beginEditBlock();
    cs.insertText(insertionText);
    cs.endEditBlock();
    m_document->removeEditingState(ProjectDocument::Palette);

    m_scopeTimer.stop();
    updateScope();

    lv::DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh ){
        dh->requestCursorPosition(cursorPosition);
    }

    return cursorPosition - name.size() - 2;
}

/**
 * \brief Adds an item given the \p addText at the specitied \p position
 */
int CodeQmlHandler::addItem(int position, const QString &, const QString &ctype){
    DocumentQmlValueScanner qvs(m_document, position, 1);
    int blockStart = qvs.getBlockStart(position) + 1;
    int blockEnd = qvs.getBlockEnd(position);

    QTextBlock tbStart = m_target->findBlock(blockStart);
    QTextBlock tbEnd   = m_target->findBlock(blockEnd);

    QString id, type;
    int idx = ctype.indexOf('#');

    if (idx != -1)
    {
        type = ctype.left(idx);
        id = ctype.mid(idx+1);
    }
    else {
        type = ctype;
    }

    QString insertionText;
    int insertionPosition = position;
    int cursorPosition = position;

    // Check where to insert the item

    if ( tbStart == tbEnd ){ // inline object declaration
        insertionPosition = blockEnd;
        insertionText = "; " + type + "{";
        if (id != "") insertionText += " id: " + id + " ";
        insertionText += "} ";
        cursorPosition = insertionPosition + type.size() + 3;
    } else { // multiline object declaration
        QString indent = getBlockIndent(tbStart) + "    ";
        insertionPosition = tbEnd.position();
        QTextBlock tbIt = tbEnd.previous();
        while ( tbIt != tbStart && tbIt.isValid() ){
            if ( !isBlockEmptySpace(tbIt) ){
                indent = getBlockIndent(tbIt);
                insertionPosition = tbIt.position() + tbIt.length();
                break;
            }
            tbIt = tbIt.previous();
        }

        insertionText = indent + type + "{\n";
        if (id != "") insertionText += indent + "    id: " + id + "\n";
        insertionText += indent + "}\n";
        cursorPosition = insertionPosition + indent.size() + type.size() + 1;
    }

    m_document->addEditingState(ProjectDocument::Palette);
    QTextCursor cs(m_target);
    cs.setPosition(insertionPosition);
    cs.beginEditBlock();
    cs.insertText(insertionText);
    cs.endEditBlock();
    m_document->removeEditingState(ProjectDocument::Palette);

    m_scopeTimer.stop();
    updateScope();

    lv::DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if (dh){
        dh->requestCursorPosition(cursorPosition);
    }

    return cursorPosition - 1 - type.size();
}

void CodeQmlHandler::addItemToRuntime(QmlEditFragment *edit, const QString &ctype, QObject *currentApp){
    Q_D(CodeQmlHandler);
    if ( !edit || !currentApp )
        return;

    QString type; QString id;
    if (ctype.contains('#'))
    {
        auto spl = ctype.split('#');
        type = spl[0];
        id = spl[1];
    } else type = ctype;

    const QList<QmlBindingChannel::Ptr>& outputChannels = edit->bindingSpan()->outputChannels();
    for ( auto it = outputChannels.begin(); it != outputChannels.end(); ++it ){
        QmlBindingChannel::Ptr bc = *it;

        if ( bc->canModify() ){
            QQmlProperty& p = bc->property();

            QObject* result = QmlCodeConverter::create(
                *d->documentScope, type + "{}", "temp"
            );

            if ( p.propertyTypeCategory() == QQmlProperty::List ){
                QQmlListReference ppref = qvariant_cast<QQmlListReference>(p.read());
                QObject* obat = ppref.at(bc->listIndex());

                QQmlProperty assignmentProperty(obat);
                if ( assignmentProperty.propertyTypeCategory() == QQmlProperty::List ){
                    QQmlListReference assignmentList = qvariant_cast<QQmlListReference>(assignmentProperty.read());
                    vlog("editqmljs-codehandler").v() <<
                        "Adding : " << result->metaObject()->className() << " to " << obat->metaObject()->className() << ", "
                        "property " << assignmentProperty.name();
                    if ( assignmentList.canAppend() ){
                        assignmentList.append(result);
                        return;
                    }
                } else {
                    vlog("editqmljs-codehandler").v() <<
                        "Assigning : " << result->metaObject()->className() << " to " << obat->metaObject()->className() << ", "
                        "property " << assignmentProperty.name();
                    assignmentProperty.write(QVariant::fromValue(result));
                    return;
                }
            } else {
                //TODO: Property based asignment
            }
        }
    }
}

/**
 * \brief Update palette binding channels for a new runtime root.
 */
void CodeQmlHandler::updateRuntimeBindings(){
    QString source = m_target->toPlainText();
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(m_document->file()->path());
    docinfo->parse(source);
    DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();

    QList<QmlEditFragment*> toRemove;

    for ( auto it = toRemove.begin(); it != toRemove.end(); ++it ){
        removeEditingFragment(*it);
    }
}

QJSValue CodeQmlHandler::getDocumentIds(){
    Q_D(CodeQmlHandler);
    QmlScopeSnap scope = d->snapScope();
    QStringList ids = scope.document->info()->extractIds();

    QJSValue result = m_engine->newArray(static_cast<quint32>(ids.length()));

    for ( int i = 0; i < ids.length(); ++i ){
        result.setProperty(static_cast<quint32>(i), ids[i]);
    }
    return result;
}

void CodeQmlHandler::suggestCompletion(int cursorPosition){
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

/**
 * \brief Handler for when a new document scope is ready
 */
void CodeQmlHandler::newDocumentScopeReady(const QString &, lv::DocumentQmlScope::Ptr documentScope){
    Q_D(CodeQmlHandler);
    if ( !documentScope->info()->isParsedCorrectly() && documentScope->imports().isEmpty() ){
        documentScope->transferImports(d->documentScope->imports());
    }

    d->documentScope = documentScope;
    m_newScope = true;
}

/**
 * \brief Handler for when a new project scope is ready
 */
void CodeQmlHandler::newProjectScopeReady(){
    m_newScope = true;
}

void CodeQmlHandler::suggestionsForGlobalQmlContext(
        const QmlCompletionContext &,
        QList<CodeCompletionSuggestion> &suggestions
){
    suggestions << CodeCompletionSuggestion("import", "", "", "import ");
    suggestions << CodeCompletionSuggestion("pragma singleton", "", "", "pragma singleton");
}

void CodeQmlHandler::suggestionsForImport(
        const QmlCompletionContext& context,
        QList<CodeCompletionSuggestion> &suggestions)
{

    foreach (const QString& importPath, m_engine->importPathList()){
        suggestionsForRecursiveImport(0, importPath, context.expressionPath(), suggestions);
    }
    std::sort(suggestions.begin(), suggestions.end(), &CodeCompletionSuggestion::compare);
}

void CodeQmlHandler::suggestionsForStringImport(
        const QString& enteredPath,
        QList<CodeCompletionSuggestion> &suggestions,
        QString& filter)
{
    if ( m_document ){
        QStringList enteredPathSegments = enteredPath.split('/');
        filter = enteredPathSegments.size() > 0 ? enteredPathSegments.last() : "";
        QString path = m_document->file()->path();
        QString dirPath = QFileInfo(path).path();
        suggestionsForRecursiveImport(0, dirPath, enteredPathSegments, suggestions);
    }
    std::sort(suggestions.begin(), suggestions.end(), &CodeCompletionSuggestion::compare);
}

void CodeQmlHandler::suggestionsForRecursiveImport(
        int index,
        const QString& dir,
        const QStringList& expression,
        QList<CodeCompletionSuggestion> &suggestions)
{
    if ( index >= expression.size() - 1 ){
        QDirIterator dit(dir);
        while( dit.hasNext() ){
            dit.next();
            if ( dit.fileInfo().isDir() && dit.fileName() != "." && dit.fileName() != ".."){
                suggestions << CodeCompletionSuggestion(dit.fileName(), "", "", dit.fileName());
            }
        }
    } else {
        QString path = dir + (dir.endsWith("/") ? "" : "/") + expression[index];
        ++index;
        suggestionsForRecursiveImport(index, path, expression, suggestions);
    }
}

void CodeQmlHandler::suggestionsForValueObject(
        const QmlTypeInfo &object,
        QList<CodeCompletionSuggestion> &suggestions,
        bool extractProperties,
        bool extractFunctions,
        bool extractSlots,
        bool extractSignals) const
{
    if ( extractProperties ){
        for ( int i = 0; i < object.totalProperties(); ++i ){
            QmlPropertyInfo qpi = object.propertyAt(i);
            suggestions << CodeCompletionSuggestion(qpi.name, qpi.typeName.name(), object.prefereredType().name(), qpi.typeName.join());
        }
    }
    if ( extractFunctions || extractSignals || extractSlots ){
        for ( int i = 0; i < object.totalFunctions(); ++i ){

            QmlFunctionInfo qfi = object.functionAt(i);
            if ( qfi.functionType == QmlFunctionInfo::Slot && extractSlots ){
                suggestions << CodeCompletionSuggestion(qfi.name, "slot", qfi.objectType.name(), qfi.name);
            } else if ( qfi.functionType == QmlFunctionInfo::Signal && extractSignals ){
                QString completion = qfi.name + "(";
                for ( auto argit = qfi.parameters.begin(); argit != qfi.parameters.end(); ++argit ){
                    if ( completion.at(completion.length() - 1) != QChar('(') )
                        completion += ", ";
                    completion += argit->first;
                }
                completion += ")";
                suggestions << CodeCompletionSuggestion(qfi.name + "()", "signal", qfi.objectType.name(), completion);
            } else if ( qfi.functionType == QmlFunctionInfo::Method && extractFunctions ){
                QString completion = qfi.name + "(";
                for ( auto argit = qfi.parameters.begin(); argit != qfi.parameters.end(); ++argit ){
                    if ( completion.at(completion.length() - 1) != QChar('(') )
                        completion += ", ";
                    completion += argit->first;
                }
                completion += ")";
                suggestions << CodeCompletionSuggestion(qfi.name + "()", "function", qfi.objectType.name(), completion);
            }
        }
    }
}

void CodeQmlHandler::suggestionsForNamespaceTypes(
    const QString &typeNameSpace,
    QList<CodeCompletionSuggestion> &suggestions) const
{
    Q_D(const CodeQmlHandler);
    QmlScopeSnap scope = d->snapScope();

    if ( typeNameSpace.isEmpty() ){
        QStringList exports;
        scope.project->implicitLibraries()->libraryInfo(scope.document->path())->listExports(&exports);

        foreach( const QString& e, exports ){
            if ( e != scope.document->componentName() )
                suggestions << CodeCompletionSuggestion(e, "", "implicit", e);
        }


        foreach( const QString& defaultLibrary, scope.project->defaultLibraries() ){
            QStringList exports;
            scope.project->globalLibraries()->libraryInfo(defaultLibrary)->listExports(&exports);
            foreach( const QString& e, exports ){
                suggestions << CodeCompletionSuggestion(e, "", "QtQml", e, "qml/QtQml#" + e);
            }
        }
    }

    foreach( const DocumentQmlScope::ImportEntry& imp, scope.document->imports() ){
        if ( imp.first.as() == typeNameSpace ){
            QStringList exports;

            QmlLibraryInfo::Ptr libinfo = scope.project->globalLibraries()->libraryInfo(imp.second);

            libinfo->listExports(&exports);
            int segmentPosition = imp.second.lastIndexOf('/');
            QString libraryName = imp.second.mid(segmentPosition + 1);

            foreach( const QString& e, exports ){
                suggestions << CodeCompletionSuggestion(e, "", libraryName, e, "qml/" + libinfo->importNamespace() + "#" + e);
            }
        }
    }
}

void CodeQmlHandler::suggestionsForNamespaceImports(QList<CodeCompletionSuggestion> &suggestions){
    Q_D(CodeQmlHandler);
    QMap<QString, QString> imports;
    DocumentQmlScope::Ptr document = d->documentScope;

    QList<CodeCompletionSuggestion> localSuggestions;

    foreach( const DocumentQmlScope::ImportEntry& imp, document->imports() ){
        if ( imp.first.as() != "" ){
            imports[imp.first.as()] = imp.first.path();
        }
    }
    for ( QMap<QString, QString>::iterator it = imports.begin(); it != imports.end(); ++it ){
        localSuggestions << CodeCompletionSuggestion(it.key(), "import", it.value(), it.key());
    }
    std::sort(localSuggestions.begin(), localSuggestions.end(), &CodeCompletionSuggestion::compare);

    suggestions << localSuggestions;
}

void CodeQmlHandler::suggestionsForDocumentsIds(QList<CodeCompletionSuggestion> &suggestions) const{
    Q_D(const CodeQmlHandler);
    QStringList ids = d->documentScope->info()->extractIds();
    ids.sort();
    foreach( const QString& id, ids ){
        suggestions << CodeCompletionSuggestion(id, "id", "", id);
    }
}

void CodeQmlHandler::suggestionsForLeftBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(CodeQmlHandler);
    if ( context.objectTypePath().size() == 0 )
        return;
    
    QmlScopeSnap scope = d->snapScope();

    if ( context.expressionPath().size() > 1 ){
        QString firstSegment = context.expressionPath()[0];

        QmlScopeSnap::PropertyReference propRef = scope.getProperty(
            scope.quickObjectDeclarationType(context.objectTypePath()), firstSegment, cursorPosition
        );

        if ( propRef.isValid() ) {
            if ( !propRef.resultTypePath.isEmpty() ){
                QmlInheritanceInfo typePath = propRef.resultTypePath;
                for ( int i = 1; i < context.expressionPath().size() - 1; ++i ){
                    typePath = scope.propertyTypePath(
                        typePath,
                        context.expressionPath()[i]
                    );
                }

                if ( !typePath.isEmpty() )
                    qmlhandler_helpers::suggestionsForObjectPath(typePath, true, false, false, false, false, ": ", suggestions);
            }
        } else if ( qmlhandler_helpers::isNamespace(scope.document, firstSegment) ){
            if ( context.expressionPath().size() == 2 )
                suggestionsForNamespaceTypes(firstSegment, suggestions);
        }

    } else {
        suggestions << CodeCompletionSuggestion("function", "", "", "function ");
        suggestions << CodeCompletionSuggestion("property", "", "", "property ");

        if ( !context.objectTypePath().isEmpty() ){
            QmlInheritanceInfo typePath = scope.getTypePath(context.objectTypePath());
            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, false, false, false, false, ": ", suggestions);
        }
        suggestionsForNamespaceTypes("", suggestions);
        suggestionsForNamespaceImports(suggestions);
    }
}

void CodeQmlHandler::suggestionsForRightBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(CodeQmlHandler);

    QmlScopeSnap scope = d->snapScope();

    if ( context.expressionPath().size() > 1 ){

        QStringList expressionPathHead = context.expressionPath();
        expressionPathHead.removeLast();

        QmlScopeSnap::ExpressionChain expression = scope.evaluateExpression(
            scope.quickObjectDeclarationType(context.objectTypePath()), expressionPathHead, cursorPosition
        );

        if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ObjectNode ){
            suggestionsForValueObject(expression.documentValueObject, suggestions, true, true, false, true);
            qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, true, true, false, false, false, "", suggestions);
            return;
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ClassNode ){
            suggestionsForValueObject(expression.documentValueObject, suggestions, true, true, false, true);
            qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, true, true, false, true, false, "", suggestions);
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::NamespaceNode ){
            suggestionsForNamespaceTypes(expression.importAs, suggestions);
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::PropertyNode ){
            qmlhandler_helpers::suggestionsForObjectPath(expression.propertyChain.last().resultTypePath, true, true, false, false, false, "", suggestions);
        }

    } else {
        /// single expression path
        if ( context.propertyName() == "id" ){

            /// special case for id
            QString type = context.objectTypePath().size() > 0 ? context.objectTypePath().last() : "";
            if ( type.isEmpty() )
                return;
            type[0] = type[0].toLower();
            suggestions << CodeCompletionSuggestion(type, "id", "", type);
        } else {

            /// other cases
            suggestions << CodeCompletionSuggestion("parent", "id", "", "parent");
            suggestionsForDocumentsIds(suggestions);
            DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(cursorPosition);
            if ( !scope.document->info()->isValueNull(documentValue) )
                suggestionsForValueObject(
                    scope.document->info()->extractValueObject(documentValue),
                    suggestions,
                    true,
                    true,
                    false,
                    true
                );

            QString type = context.objectTypePath().size() > 0 ? context.objectTypePath().last() : "";
            if ( type == "" )
                return;
            QString typeNamespace = context.objectTypePath().size() > 1 ? context.objectTypePath().first() : "";
            QmlInheritanceInfo typePath = scope.getTypePath(typeNamespace, type);
            qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, false, false, false, "", suggestions);
            suggestionsForNamespaceTypes("", suggestions);
        }
    }
}

void CodeQmlHandler::suggestionsForLeftSignalBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(CodeQmlHandler);
    QmlScopeSnap scope = d->snapScope();

    DocumentQmlInfo::ValueReference documentValue = scope.document->info()->valueAtPosition(cursorPosition);
    if ( !scope.document->info()->isValueNull(documentValue) )
        suggestionsForValueObject(
            scope.document->info()->extractValueObject(documentValue),
            suggestions,
            true,
            true,
            true,
            true
        );

    QString type = context.objectTypePath().size() > 0 ? context.objectTypePath()[0] : "";
    if ( type == "" )
        return;
    QString typeNamespace = context.objectTypePath().size() > 1 ? context.objectTypePath()[1] : "";
    QmlInheritanceInfo typePath = scope.getTypePath(typeNamespace, type);
    qmlhandler_helpers::suggestionsForObjectPath(typePath, true, true, true, true, true, ": ", suggestions);
}

QString CodeQmlHandler::extractQuotedString(const QTextCursor &cursor) const{
    QTextBlock block = cursor.block();
    int localCursorPosition = cursor.positionInBlock();
    QString blockString = block.text();

    int startOfQuote = blockString.lastIndexOf('\"', localCursorPosition - 1);
    if ( startOfQuote != -1 )
        return blockString.mid(startOfQuote + 1, localCursorPosition - startOfQuote - 1);
    else
        return blockString.mid(0, localCursorPosition);
}

QString CodeQmlHandler::getBlockIndent(const QTextBlock &bl){
    QString blockText = bl.text();

    for ( int i = 0; i < blockText.length(); ++i ){
        if ( !blockText[i].isSpace() )
            return blockText.mid(0, i);
    }
    return blockText;
}

bool CodeQmlHandler::isBlockEmptySpace(const QTextBlock &bl){
    QString blockText = bl.text();
    for ( auto it = blockText.begin(); it != blockText.end(); ++it )
        if ( !it->isSpace() )
            return false;

    return true;
}

bool CodeQmlHandler::isForAnObject(const lv::QmlDeclaration::Ptr &declaration){
    if ( !declaration->type().path().isEmpty() )
        return true;
    if (declaration->type().name() == "import")
        return false;
    return QmlTypeInfo::isObject(declaration->type().name());
}

}// namespace

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
#include "qmlprojectmonitor_p.h"

#include "qmleditfragmentcontainer.h"
#include "qmlcompletioncontextfinder.h"
#include "documentqmlvaluescanner_p.h"
#include "documentqmlvalueobjects.h"
#include "qmljsbuiltintypes_p.h"
#include "qmljshighlighter_p.h"
#include "qmlbindingchannel.h"
#include "qmladdcontainer.h"
#include "qmlusagegraphscanner.h"
#include "documentqmlchannels.h"

#include "qmljs/parser/qqmljsast_p.h"
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
#include "live/hookcontainer.h"

#include "qmlsuggestionmodel.h"
#include "qmlbuilder.h"
#include "qmlwatcher.h"

#include <QJSValueIterator>
#include <QQmlEngine>
#include <QQmlContext>
#include <QFileInfo>
#include <QDirIterator>
#include <QTextDocument>
#include <QQmlComponent>
#include <QStringList>
#include <QWaitCondition>
#include <queue>
#include <QSet>

namespace lv{

// CodeQmlHandlerPrivate
// ----------------------------------------------------------------------------

/// \private
class CodeQmlHandlerPrivate{

public:
    CodeQmlHandlerPrivate()
        : m_documentParseSync(false)
        , m_documentBackgroundParseSync(false)
        , m_documentValueObjectsSync(false)
        , m_documentIsScanning(false)
        , m_documentUpdatedFromBackground(false)
    {
        m_documentInfo = DocumentQmlInfo::createAndParse("", "");
    }


    QmlScopeSnap snapScope() const{
        return QmlScopeSnap(projectHandler->scanMonitor()->projectScope(), m_documentInfo);
    }

    void syncParse(ProjectDocument* document){
        if ( !m_documentParseSync ){
            m_documentInfo = DocumentQmlInfo::create(document->file()->path());
            m_documentInfo->parse(document->contentString());
            m_documentParseSync = true;
        }
    }

    void syncObjects(ProjectDocument* document){
        if ( !m_documentValueObjectsSync ){
            syncParse(document);
            m_documentObjects = m_documentInfo->createObjects();
            m_documentValueObjectsSync = true;
        }
    }

    void objectSentToBackgroundScan(){
        m_documentBackgroundParseSync = true;
    }

    void assignDocumentFromBackgroundSync(const DocumentQmlInfo::Ptr& document){
        m_documentIsScanning = false;
        if ( !m_documentParseSync ){
            m_documentInfo = document;
            if ( m_documentBackgroundParseSync ){
                m_documentParseSync        = true;
                m_documentValueObjectsSync = false;
                m_documentUpdatedFromBackground = true;
            }
        }
        if ( m_documentBackgroundParseSync ){
            m_documentUpdatedFromBackground = true;
        }
    }

    void documentQueuedForScanning(){
        m_documentIsScanning          = true;
        m_documentBackgroundParseSync = true;
    }

    void documentChanged(){
        m_documentParseSync             = false;
        m_documentBackgroundParseSync   = false;
        m_documentValueObjectsSync      = false;
        m_documentUpdatedFromBackground = false;
    }

    const DocumentQmlValueObjects::Ptr& documentObjects() const{ return m_documentObjects; }
    const DocumentQmlInfo::Ptr documentInfo() const{ return m_documentInfo;}
    bool wasDocumentUpdatedFromBackground() const{ return m_documentUpdatedFromBackground; }
    bool isDocumentBeingScanned() const{ return m_documentIsScanning; }
    bool isDocumentParsed() const{ return m_documentParseSync; }

    ProjectQmlExtension*  projectHandler;

private:
    DocumentQmlInfo::Ptr         m_documentInfo;
    DocumentQmlValueObjects::Ptr m_documentObjects;

    bool                  m_documentParseSync;
    bool                  m_documentBackgroundParseSync;
    bool                  m_documentValueObjectsSync;
    bool                  m_documentIsScanning;
    bool                  m_documentUpdatedFromBackground;
};

namespace qmlhandler_helpers{

    void suggestionsForObject(
        const QmlTypeInfo::Ptr& ti,
        bool suggestProperties,
        bool suggestMethods,
        bool suggestSignals,
        bool suggestEnums,
        bool suggestGeneratedSlots,
        const QString& suffix,
        QList<CodeCompletionSuggestion>& suggestions
    ){
        if ( !ti )
            return;

        QList<CodeCompletionSuggestion> localSuggestions;

        if ( suggestProperties ){
            for ( int i = 0; i < ti->totalProperties(); ++i ){
                if ( !ti->propertyAt(i).name.startsWith("__") ){
                    localSuggestions << CodeCompletionSuggestion(
                        ti->propertyAt(i).name,
                        ti->propertyAt(i).typeName.name(),
                        ti->prefereredType().name(),
                        ti->propertyAt(i).name + (ti->propertyAt(i).isPointer ? "" : suffix),
                        ti->prefereredType().join() + "." + ti->propertyAt(i).name
                    );
                }
            }
        }
        for ( int i = 0; i < ti->totalFunctions(); ++i ){
            QmlFunctionInfo m = ti->functionAt(i);
            if ( (m.functionType == QmlFunctionInfo::Method ||
                  m.functionType == QmlFunctionInfo::Slot) && suggestMethods ){
                QString completion = m.name + "(";// + m.parameters.join(", ") + ")";
                localSuggestions << CodeCompletionSuggestion(
                    m.name + "()", "function", ti->prefereredType().name(), completion
                );
            }
            if ( m.functionType == QmlFunctionInfo::Signal && suggestSignals ){
                QString completion = m.name + "(";// + m.parameters.join(", ") + ")";
                localSuggestions << CodeCompletionSuggestion(
                    m.name + "()", "signal", ti->prefereredType().name(), completion
                );
            }
            if ( m.functionType == QmlFunctionInfo::Signal && suggestGeneratedSlots ){
                QString methodName = m.name;
                if ( methodName.size() > 0 )
                    methodName[0] = methodName[0].toUpper();
                localSuggestions << CodeCompletionSuggestion(
                    "on" + methodName, "slot", ti->prefereredType().name(), "on" + methodName + suffix
                );
            }
        }
        if ( suggestEnums ){
            for ( int i = 0; i < ti->totalEnums(); ++i ){
                QmlEnumInfo e = ti->enumAt(i);
                for ( int j = 0; j < e.keys.size(); ++j ){
                    localSuggestions << CodeCompletionSuggestion(
                        e.keys.at(j),
                        e.name,
                        ti->prefereredType().name(),
                        e.keys.at(j)
                    );
                }
            }
        }
        if ( suggestGeneratedSlots ){
            for ( int i = 0; i < ti->totalProperties(); ++i ){
                if ( !ti->propertyAt(i).name.startsWith("__") ){
                    QString propertyName = ti->propertyAt(i).name;
                    if ( propertyName.size() > 0 )
                        propertyName[0] = propertyName[0].toUpper();

                    localSuggestions << CodeCompletionSuggestion(
                        "on" + propertyName + "Changed",
                        "slot",
                        ti->prefereredType().name(),
                        "on" + propertyName + "Changed" + suffix
                    );
                }
            }
        }

        std::sort(localSuggestions.begin(), localSuggestions.end(), &CodeCompletionSuggestion::compare);
        suggestions << localSuggestions;
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
            const QmlTypeInfo::Ptr& ti = *it;
            suggestionsForObject(
                ti,
                suggestProperties,
                suggestMethods,
                suggestSignals,
                suggestEnums,
                suggestGeneratedSlots,
                suffix,
                suggestions
            );
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
    , m_engine(engine)
    , m_completionContextFinder(new QmlCompletionContextFinder)
    , m_newScope(false)
    , m_editingFragment(nullptr)
    , m_editContainer(new QmlEditFragmentContainer(this))
    , m_bindingChannels(nullptr)
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
    connect(d->projectHandler->scanMonitor(), &QmlProjectMonitor::libraryScanQueueCleared,
            this, &CodeQmlHandler::__whenLibraryScanQueueCleared);

    setDocument(document);

    m_bindingChannels = new DocumentQmlChannels(projectHandler->channelsDispatcher(), this);
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
                qmlhandler_helpers::suggestionsForObject(expression.documentValueObject, true, true, false, false, false, "", suggestions);
                qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, true, true, false, false, false, "", suggestions);
                return;
            } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ClassNode ){
                qmlhandler_helpers::suggestionsForObject(expression.documentValueObject, true, true, false, true, false, "", suggestions);
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

    if ( m_document ){
        connect(m_document->textDocument(), &QTextDocument::contentsChange,
                this, &CodeQmlHandler::__documentContentsChanged);
        connect(m_document, &ProjectDocument::formatChanged, this, &CodeQmlHandler::__documentFormatUpdate);
        connect(
            m_document->textDocument(), &QTextDocument::cursorPositionChanged,
            this, &CodeQmlHandler::__cursorWritePositionChanged
        );

        m_editContainer->clearAllFragments();
    }

    if ( d->projectHandler->scanMonitor()->hasProjectScope() && document != nullptr ){
        d->documentQueuedForScanning();
        d->projectHandler->scanMonitor()->queueDocumentScan(document->file()->path(), document->contentString(), this);
    }
}

/**
 * \brief DocumentContentsChanged handler
 */
void CodeQmlHandler::__documentContentsChanged(int position, int, int){
    Q_D(CodeQmlHandler);
    d->documentChanged();
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
    if ( !d->isDocumentBeingScanned() && d->projectHandler->scanMonitor()->hasProjectScope() && m_document ){
        d->projectHandler->scanMonitor()->queueDocumentScan(m_document->file()->path(), m_document->contentString(), this);
        d->documentQueuedForScanning();
    }
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
                        return ipath.nodes.first()->prefereredType().join();
                    }
                } else { // property declaration
                    QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
                        scope.quickObjectDeclarationType(ctx->objectTypePath()), expression, cursor.position()
                    );

                    if ( propChain.size() == expression.size() && propChain.size() > 0 ){
                        QmlScopeSnap::PropertyReference& propref = propChain.last();
                        if ( !propref.classTypePath.isEmpty() ){
                            return propref.classTypePath.nodes.first()->prefereredType().join();
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
                    return expressionChain.typeReference.nodes.first()->prefereredType().join();
                }
            } else if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::ClassNode ){
                if ( !expressionChain.typeReference.isEmpty() ){
                    return expressionChain.typeReference.nodes.first()->prefereredType().join();
                }

            } else if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::EnumNode ){

                if ( !expressionChain.typeReference.isEmpty() ){
                    return expressionChain.typeReference.nodes.first()->prefereredType().join() + "." + expressionChain.enumName;
                }

            } else if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::NamespaceNode ){
                DocumentQmlInfo::Ptr di = scope.document;
                foreach( const DocumentQmlInfo::Import& imp, di->imports() ){
                    if ( imp.as() == expression.first() ){
                        return "qml/" + imp.uri();
                    }
                }

            } else if ( expressionChain.lastSegmentType() == QmlScopeSnap::ExpressionChain::PropertyNode ){

                if ( expressionChain.propertyChain.size() > 0 ){
                    QmlScopeSnap::PropertyReference& propref = expressionChain.propertyChain.last();
                    if ( !propref.classTypePath.isEmpty() ){
                        QmlTypeInfo::Ptr tr = propref.classTypePath.nodes.first();
                        return tr->prefereredType().join();
                    }
                }

            }
        }

    }

    return "";
}

void CodeQmlHandler::addPropertiesAndFunctionsToModel(const QmlInheritanceInfo &typePath, QmlSuggestionModel *model, bool isForNode)
{
    for ( auto it = typePath.nodes.begin(); it != typePath.nodes.end(); ++it ){
        const QmlTypeInfo::Ptr& ti = *it;

        for (int i = 0; i < ti->totalFunctions(); ++i)
        {
            auto method = ti->functionAt(i);
            if (method.functionType == QmlFunctionInfo::Signal)
            {
                auto name = method.name;
                name = QString("on") + name[0].toUpper() + name.mid(1);

                model->addItem(QmlSuggestionModel::ItemData(
                    name,
                    ti->prefereredType().name(),
                    "method",
                    "",
                    ti->exportType().join() + "." + name,
                    name,
                    QmlSuggestionModel::ItemData::Event)
                );
            } else if (isForNode) {
                auto name = method.name;

                model->addItem(QmlSuggestionModel::ItemData(
                    name,
                    ti->prefereredType().name(),
                    "method",
                    "",
                    ti->exportType().join() + "." + name,
                    name,
                    QmlSuggestionModel::ItemData::Function)
                );

            }
        }

        for ( int i = 0; i < ti->totalProperties(); ++i ){
            QString propertyName = ti->propertyAt(i).name;
            if ( !propertyName.startsWith("__")){
                model->addItem(
                    QmlSuggestionModel::ItemData(
                        propertyName,
                        ti->prefereredType().name(),
                        ti->propertyAt(i).typeName.name(),
                        "",
                        ti->exportType().join() + "." + propertyName,
                        propertyName,
                        QmlSuggestionModel::ItemData::Property,
                        ti->propertyAt(i).isPointer)
                );
            }


            if ( propertyName.size() > 0 )
                propertyName[0] = propertyName[0].toUpper();

            if (propertyName != "ObjectName"){
                propertyName = "on" + propertyName + "Changed";
                model->addItem(
                    QmlSuggestionModel::ItemData(
                        propertyName,
                        ti->prefereredType().name(),
                        "method",
                        "",
                        "", //TODO: Find library path
                        propertyName,
                        QmlSuggestionModel::ItemData::Event)
                );
            }
        }
        model->updateFilters();
    }
}

void CodeQmlHandler::addObjectsToModel(const QmlScopeSnap &scope, QmlSuggestionModel *model)
{
    // import global objects

    QStringList exports;
    QmlLibraryInfo::Ptr lib = scope.project->libraryInfo(scope.document->path());
    if ( lib ){
        exports = lib->listExports();
    }

    foreach( const QString& e, exports ){
        if ( e != scope.document->componentName() ){
            model->addItem(
                QmlSuggestionModel::ItemData(
                    e,
                    "",
                    "",
                    "implicit",
                    scope.document->path(),
                    e,
                    QmlSuggestionModel::ItemData::Object)
            );
        }
    }

    for( const QString& defaultLibrary: scope.project->defaultLibraries() ){
        QmlLibraryInfo::Ptr defaultLib = scope.project->libraryInfo(defaultLibrary);
        QStringList defaultExports;
        if ( defaultLib ){
            defaultExports = defaultLib->listExports();
        }
        for( const QString& de: defaultExports ){
            if ( de != "Component"){
                model->addItem(
                    QmlSuggestionModel::ItemData(
                        de,
                        "",
                        "",
                        "QtQml",
                        "QtQml",
                        de,
                        QmlSuggestionModel::ItemData::Object)
                );
            }
        }
    }

    // import namespace objects

    QSet<QString> imports;

    foreach( const DocumentQmlInfo::Import& imp, scope.document->imports() ){
        if ( imp.as() != "" ){
            imports.insert(imp.as());
        }
    }
    imports.insert("");

    for ( QSet<QString>::iterator it = imports.begin(); it != imports.end(); ++it ){

        foreach( const DocumentQmlInfo::Import& imp, scope.document->imports() ){
            if ( imp.as() == *it ){
                QStringList libexports;
                QmlLibraryInfo::Ptr implib = scope.project->libraryInfo(imp.uri());
                if ( !lib.isNull() ){
                    libexports = implib->listExports();
                }

                for ( const QString& exp: libexports ){
                    if ( exp != "Component"){
                        model->addItem(
                            QmlSuggestionModel::ItemData(
                                exp,
                                "",
                                "",
                                imp.uri(),
                                imp.uri(),
                                exp,
                                QmlSuggestionModel::ItemData::Object)
                        );
                    }
                }
            }
        }
    }

    model->updateFilters();
}

/**
 * \brief Calls a new rehighlight on the specified block
 */
void CodeQmlHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_highlighter ){
        m_highlighter->rehighlightBlock(block);
    }
}

QmlDeclaration::Ptr CodeQmlHandler::getDeclarationViaCompletionContext(int position) const{
    Q_D(const CodeQmlHandler);

    // get imports

    QTextCursor cursor(m_document->textDocument());
    cursor.setPosition(position);

    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

    QStringList expression;
    int propertyPosition = ctx->propertyPosition();
    int propertyLength   = 0;

    QChar expressionEndDelimiter;

    if ( ctx->context() & QmlCompletionContext::InLhsOfBinding ){
        expression = ctx->expressionPath();

        if ( propertyPosition == -1 ){
            if ( m_target->characterAt(cursor.position()).isSpace() ){
                return nullptr;
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
                return QmlDeclaration::create(
                    QStringList(),
                    ipath.languageType(),
                    QmlTypeReference(),
                    propertyPosition,
                    0,
                    m_document
                );
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
                        signalName.mid(0, signalName.length() - 7), position);

                    if ( associatedPropertyInfo.isValid() ){
                        associatedSignal.name = signalName;
                        associatedSignal.accessType = QmlFunctionInfo::Public;
                        associatedSignal.functionType = QmlFunctionInfo::Signal;
                        associatedSignal.objectType = associatedPropertyInfo.classTypePath.languageType();
                    }
                }

                if ( associatedSignal.isValid() ){
                    isSlot = true;
                    return QmlDeclaration::create(
                        expression,
                        QmlTypeReference(QmlTypeReference::Qml, "slot"),
                        associatedSignal.objectType,
                        propertyPosition,
                        propertyLength,
                        m_document
                    );
                }
            }

            if ( !isSlot ){
                QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
                    scope.quickObjectDeclarationType(ctx->objectTypePath()), expression, position
                );

                if ( propChain.size() == expression.size() && propChain.size() > 0 ){
                    QmlScopeSnap::PropertyReference& propref = propChain.last();
                    QmlTypeReference qlt = propref.resultType();

                    if ( !qlt.isEmpty() ){
                        return QmlDeclaration::create(
                            expression,
                            propref.resultType(),
                            propref.propertyObjectType(),
                            propertyPosition,
                            propertyLength,
                            m_document
                        );
                    }
                }
            }
        }
    }
    return nullptr;
}

QList<QmlDeclaration::Ptr> CodeQmlHandler::getDeclarationsViaParsedDocument(int position, int length){
    Q_D(CodeQmlHandler);

    QList<QmlDeclaration::Ptr> declarations;
    d->syncParse(m_document);
    d->syncObjects(m_document);

    //TODO: Get objects as well, and imports

    DocumentQmlInfo::Ptr docinfo = d->documentInfo();
    DocumentQmlValueObjects::Ptr objects = d->documentObjects();

    QList<DocumentQmlValueObjects::RangeProperty*> rangeProperties = objects->propertiesBetween(
        position, length
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
                    declarations.append(QmlDeclaration::create(
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
                    QmlTypeInfo::Ptr tr = scope.getType(propertyType);
                    qlt = tr->prefereredType();
                } else {
                    qlt = QmlTypeReference(QmlTypeReference::Qml, rp->type());
                }
                if ( !qlt.isEmpty() ){
                    QmlTypeInfo::Ptr tr = scope.getType(rp->object());
                    declarations.append(QmlDeclaration::create(
                        rp->name(), qlt, tr->prefereredType(), rp->begin, rp->propertyEnd - rp->begin, m_document
                    ));
                }
            }
        }
    }

    return declarations;
}

/**
 * \brief Get a list of declarations from a specific cursor
 */
QList<QmlDeclaration::Ptr> CodeQmlHandler::getDeclarations(const QTextCursor& cursor){
    QList<QmlDeclaration::Ptr> properties;
    int length = cursor.selectionEnd() - cursor.selectionStart();

    if ( length == 0 ){
        QmlDeclaration::Ptr declaration = getDeclarationViaCompletionContext(cursor.position());
        if ( declaration )
            properties << declaration;

    } else { // multiple declarations were selected
        return getDeclarationsViaParsedDocument(cursor.position(), length);
    }

    return properties;
}

/**
 * \brief Creates an injection channel between a declaration and the runtime
 *
 * This method will find all binding channels available by parsing the contents
 * of the code structure, creating a set of binding pahts that describe where
 * each component resides, and use those binding paths to connect the position of the
 * code structure to the same values within the runtime.
 */
QmlEditFragment *CodeQmlHandler::createInjectionChannel(QmlDeclaration::Ptr declaration, QmlEditFragment* parentEdit){
    Q_D(CodeQmlHandler);
    Project* project = d->projectHandler->project();

    if ( m_document ){
        d->syncParse(m_document);
        d->syncObjects(m_document);
        QmlBindingPath::Ptr bp = DocumentQmlInfo::findDeclarationPath(m_document, d->documentObjects()->root(), declaration);
        if ( !bp )
            return nullptr;

        QmlEditFragment* ef = new QmlEditFragment(declaration, this);
        if ( declaration->isForProperty() ){
            if ( declaration->valueObjectScopeOffset() > -1 ){
                // in case a property also initializes an object, capture the object type
                int objectInitStart = declaration->valuePosition();
                QStringList initType = m_document->substring(objectInitStart, declaration->valueObjectScopeOffset()).trimmed().split('.');
                QmlScopeSnap scope = d->snapScope();
                QmlTypeInfo::Ptr ti = scope.getType(initType);
                if ( ti ){
                    ef->setObjectInitializeType(ti->prefereredType());
                }
            }
        }

        QmlBindingChannel::Ptr documentChannel = m_bindingChannels->selectedChannel();
        if ( documentChannel ){

            if ( parentEdit ){
                auto parentBp = parentEdit->fullBindingPath();

                int copyLength = bp->length() - parentBp->length();
                QmlBindingPath::Ptr relativeBp = QmlBindingPath::create();

                while ( copyLength > 0){
                    relativeBp = QmlBindingPath::join(bp->tailPath(), relativeBp);
                    bp = bp->headPath();
                    copyLength--;
                }

                QmlBindingChannel::Ptr newChannel = DocumentQmlChannels::traverseBindingPathFrom(parentEdit->channel(), relativeBp);
                if ( !newChannel ){
                    //TODO
                    qWarning("Warning: Failed to get new channel at: %s from %s", qPrintable(relativeBp->toString()), qPrintable(parentBp->toString()));
                } else {
                    newChannel->setEnabled(true);
                    ef->setChannel(newChannel);
                }

            } else {
                QmlBindingChannel::Ptr newChannel = DocumentQmlChannels::traverseBindingPathFrom(documentChannel, bp);
                if ( !newChannel ){
                    //TODO
                    qWarning("Warning: Failed to get new channel from document at: %s", qPrintable(bp->toString()));
                } else {
                    newChannel->setEnabled(true);
                    ef->setChannel(newChannel);
                }
            }
        }

        if ( !ef->channel() ){
            delete ef;
            return nullptr;
        }
        return ef;
    }

    return nullptr;
}

void CodeQmlHandler::aboutToDelete()
{
    cancelEdit();

    if ( m_document ){
        m_editContainer->clearAllFragments();
    }
}

QVariantList CodeQmlHandler::nestedObjectsInfo(lv::QmlEditFragment* ef)
{
    return ef->nestedObjectsInfo();
}

QString CodeQmlHandler::getFragmentId(QmlEditFragment *ef)
{
    Q_D(CodeQmlHandler);

    QList<QObject*> fragments;

    d->syncParse(m_document);
    d->syncObjects(m_document);
    QmlScopeSnap scope = d->snapScope();

    DocumentQmlValueObjects::Ptr objects = d->documentObjects();
    DocumentQmlValueObjects::RangeObject* currentOb = objects->objectAtPosition(ef->position());

    QString id;
    if ( currentOb ){
        for (int k = 0; k < currentOb->properties.size(); ++k)
        {
            auto prop = currentOb->properties[k];
            if (prop->name().size() == 1 && prop->name()[0] == "id"){
                auto docString = m_document->contentString();
                id = docString.mid(prop->valueBegin, prop->end - prop->valueBegin);
                break;
            }
        }
    }

    ef->setObjectId(id);
    return id;
}

QJSValue CodeQmlHandler::createCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, bool canShape, bool inImports)
{
    QJSValue result = m_engine->engine()->newObject();
    result.setProperty("canBind",   canBind);
    result.setProperty("canUnbind", canUnbind);
    result.setProperty("canEdit",   canEdit);
    result.setProperty("canAdjust", canAdjust);
    result.setProperty("canShape",  canShape);
    result.setProperty("inImports", inImports);
    return result;
}

/**
 * \brief Adds an editing fragment to the current document
 */
bool CodeQmlHandler::addEditingFragment(QmlEditFragment *edit){
    //TOMOVE
    return m_editContainer->addEdit(edit);
}

/**
 * \brief Removes an editing fragment from this document
 */
void CodeQmlHandler::removeEditingFragment(QmlEditFragment *edit){
    //TOMOVE
    m_editContainer->removeEdit(edit);
}

int CodeQmlHandler::findImportsPosition(int blockPos)
{
    if (blockPos == -1) return 0;
    auto block = m_document->textDocument()->findBlockByNumber(blockPos);
    return block.position() + 7; // e.g. import ^QtQuick
}

int CodeQmlHandler::findRootPosition(){
    Q_D(CodeQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);

    if ( d->documentObjects()->root() ){
        return d->documentObjects()->root()->begin;
    }
    return -1;
}

lv::QmlEditFragment *CodeQmlHandler::findObjectFragmentByPosition(int position)
{
    //TOMOVE
    return m_editContainer->findObjectFragmentByPosition(position);
}

lv::QmlEditFragment *CodeQmlHandler::findFragmentByPosition(int position)
{
    //TOMOVE
    return m_editContainer->findFragmentByPosition(position);
}

QJSValue CodeQmlHandler::editingFragments(){
    //TOMOVE
    return m_editContainer->allEdits();
}

lv::QmlEditFragment* CodeQmlHandler::findChildPropertyFragmentByName(lv::QmlEditFragment* parent, QString name) const
{
    //TOMOVE
    return m_editContainer->findChildPropertyFragmentByName(parent, name);
}

void CodeQmlHandler::toggleComment(int position, int length)
{
    if ( !m_document )
        return;

    Q_D(CodeQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);

    auto td = m_document->textDocument();
    auto firstBlock = td->findBlock(position);
    auto lastBlock = td->findBlock(position + length);

    bool found = false;
    for (auto it = firstBlock; it.isValid() && it != lastBlock.next(); it = it.next()){
        auto txt = it.text();
        if (txt.length() > 2 && txt.left(2) != "//")
        {
            found = true;
            break;
        }
    }

    for (auto it = firstBlock; it.isValid() && it != lastBlock.next(); it = it.next())
    {
        QTextCursor cursor(td);
        cursor.setPosition(it.position());
        if (found){
            cursor.beginEditBlock();
            cursor.insertText("//");
            cursor.endEditBlock();
        } else {
            cursor.beginEditBlock();
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
            cursor.removeSelectedText();
            cursor.endEditBlock();
        }
    }
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
            qmlhandler_helpers::suggestionsForObject(expression.documentValueObject, !suggestFunctions, suggestFunctions, false, false, suggestFunctions, "", suggestions);
            qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, !suggestFunctions, suggestFunctions, false, false, suggestFunctions, "", suggestions);
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ClassNode ){
            qmlhandler_helpers::suggestionsForObject(expression.documentValueObject, !suggestFunctions, suggestFunctions, false, false, suggestFunctions, "", suggestions);
            qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, !suggestFunctions, suggestFunctions, false, false, suggestFunctions, "", suggestions);
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::NamespaceNode ){
            suggestionsForNamespaceTypes(expression.importAs, suggestions);
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::PropertyNode ){
            qmlhandler_helpers::suggestionsForObjectPath(expression.propertyChain.last().resultTypePath, true, true, false, false, false, "", suggestions);
        }
    } else {
        suggestions << CodeCompletionSuggestion("parent", "id", "", "parent");
        suggestionsForDocumentsIds(suggestions);
        DocumentQmlInfo::ValueReference documentValue = scope.document->valueAtPosition(cursorPosition);
        if ( !scope.document->isValueNull(documentValue) )
            qmlhandler_helpers::suggestionsForObject(
                scope.document->extractValueObject(documentValue),
                !suggestFunctions,
                suggestFunctions,
                suggestFunctions,
                false,
                false,
                "",
                suggestions);

        QmlInheritanceInfo typePath = scope.getTypePath(declaration->parentType());
        qmlhandler_helpers::suggestionsForObjectPath(typePath, !suggestFunctions, suggestFunctions, suggestFunctions, false, false, "", suggestions);
    }

    if ( !suggestions.isEmpty() ){
        model->enable();
    }

    model->setSuggestions(suggestions, filter);
}

bool CodeQmlHandler::findBindingForExpression(lv::QmlEditFragment *edit, const QString &expression){
    Q_D(CodeQmlHandler);

    QmlScopeSnap scope = d->snapScope();

    QStringList expressionPathNotTrimmed = expression.split(".");
    QStringList expressionPath;
    for ( const QString& expr : expressionPathNotTrimmed ){
        expressionPath.append(expr.trimmed());
    }

    int cursorPosition = edit->declaration()->position();

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

    if ( expressionChain.isId ){ // <id>.<property...>
        DocumentQmlInfo::ValueReference documentValue = scope.document->valueForId(expressionPath.first());

        int begin;
        int end;
        scope.document->extractTypeNameRange(documentValue, begin, end);

        QTextCursor cursor(m_document->textDocument());
        cursor.setPosition(end);
        QList<QmlDeclaration::Ptr> declarations = getDeclarations(cursor);

        d->syncParse(m_document);
        d->syncObjects(m_document);
        QmlBindingPath::Ptr newBp = DocumentQmlInfo::findDeclarationPath(
            m_document,
            d->documentObjects()->root(),
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
            pn->objectName = QStringList() << prop.classTypePath.nodes.first()->prefereredType().join();
            if ( n ){
                pn->parent = n;
                n->child = pn;
            }
        }

    } else if ( expressionChain.isParent ){ // <parent...>.<property...>

        bp = edit->fullBindingPath()->parentObjectPath();
        if ( !bp )
            return false;


        for ( const QmlScopeSnap::PropertyReference& prop : expressionChain.propertyChain){
            bp->appendProperty(
                prop.property.name,
                QStringList() << prop.classTypePath.nodes.first()->prefereredType().join()
            );
        }

    } else { // <property...>
        // clone current binding path, append properties to it

        bp = edit->fullBindingPath()->clone();
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
            pn->objectName = QStringList() << prop.classTypePath.nodes.first()->prefereredType().join();

            if ( n ){
                pn->parent = n;
                n->child = pn;
            }
        }
    }

    QmlBindingChannel::Ptr receivingChannel = edit->channel();
    if ( receivingChannel->isEnabled() ){

        QmlBindingChannel::Ptr documentChannel = m_bindingChannels->selectedChannel();
        if ( documentChannel ){
            if ( documentChannel->isBuilder() )
                return true;

            QmlBindingChannel::Ptr writeChannel = DocumentQmlChannels::traverseBindingPathFrom(documentChannel, bp);

            if ( !writeChannel->hasConnection() ){
                qWarning("Failed to find binding channel at: %s", qPrintable(writeChannel->bindingPath()->toString()));
            } else {
                // width is the receiving, need to remove the previous listening channel

                if ( receivingChannel->property().isValid() && receivingChannel->property().object() ){
                    QQmlProperty recivingProperty = receivingChannel->property();

                    QByteArray watcherName = "__" + recivingProperty.name().toUtf8() + "Watcher";
                    QVariant previousWatcherVariant = recivingProperty.object()->property(watcherName);
                    if ( previousWatcherVariant.isValid() ){
                        QObject* previousWatcher = previousWatcherVariant.value<QObject*>();
                        delete previousWatcher;
                        recivingProperty.object()->setProperty(watcherName, QVariant());
                    }

                    QmlPropertyWatcher* watcher = new QmlPropertyWatcher(writeChannel->property());

                    // receiving channel vs write channel
                    // clearing up stuff
                    recivingProperty.write(watcher->read());
                    watcher->onChange([recivingProperty](const QmlPropertyWatcher& ww){
                        recivingProperty.write(ww.read());
                    });
                    // delete the watcher with the receiving channel
                    watcher->setParent(receivingChannel->property().object());

                    recivingProperty.object()->setProperty(watcherName, QVariant::fromValue(watcher));
                }
            }
        }
    }
    return true;
}

bool CodeQmlHandler::findFunctionBindingForExpression(QmlEditFragment *edit, const QString &expression){
    Q_D(CodeQmlHandler);

    QmlScopeSnap scope = d->snapScope();

    QStringList expressionPathNotTrimmed = expression.split(".");
    QStringList expressionPath;
    for ( const QString& expr : expressionPathNotTrimmed ){
        expressionPath.append(expr.trimmed());
    }

    int cursorPosition = edit->declaration()->position();

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

    if ( expressionChain.isId ){ // <id>.<property...>
        DocumentQmlInfo::ValueReference documentValue = scope.document->valueForId(expressionPath.first());

        int begin;
        int end;
        scope.document->extractTypeNameRange(documentValue, begin, end);

        QTextCursor cursor(m_document->textDocument());
        cursor.setPosition(end);
        QList<QmlDeclaration::Ptr> declarations = getDeclarations(cursor);

        d->syncParse(m_document);
        d->syncObjects(m_document);
        QmlBindingPath::Ptr newBp = DocumentQmlInfo::findDeclarationPath(
            m_document,
            d->documentObjects()->root(),
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
            pn->objectName = QStringList() << prop.classTypePath.nodes.first()->prefereredType().join();
            if ( n ){
                pn->parent = n;
                n->child = pn;
            }
        }

    } else if ( expressionChain.isParent ){ // <parent...>.<property...>

        bp = edit->fullBindingPath()->parentObjectPath();
        if ( !bp )
            return false;


        for ( const QmlScopeSnap::PropertyReference& prop : expressionChain.propertyChain){

            bp->appendProperty(
                prop.property.isValid() ? prop.property.name : prop.functionInfo.definition(),
                QStringList() << prop.classTypePath.nodes.first()->prefereredType().join()
            );
        }

    } else { // <property...>
        // clone current binding path, append properties to it

        bp = edit->fullBindingPath()->clone();
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
            pn->objectName = QStringList() << prop.classTypePath.nodes.first()->prefereredType().join();
            if ( n ){
                pn->parent = n;
                n->child = pn;
            }
        }
    }

    if ( bp.isNull() )
        return false;

    QmlBindingChannel::Ptr signalChannel = edit->channel();
    if ( signalChannel->isEnabled() ){

        QmlBindingChannel::Ptr documentChannel = m_bindingChannels->selectedChannel();
        if ( documentChannel ){
            if ( documentChannel->isBuilder() )
                return true;

            QmlBindingChannel::Ptr functionChannel = DocumentQmlChannels::traverseBindingPathFrom(documentChannel, bp);
            if ( functionChannel.isNull() || !functionChannel->hasConnection() ){
                qWarning("Failed to find binding channel at: %s", qPrintable(bp->toString()));
                return false;
            } else {
                if ( functionChannel->method().isValid() && functionChannel->property().object() ){
                    QMetaMethod signalMethod = signalChannel->property().method();
                    QMetaMethod slotMethod = functionChannel->method();
                    disconnect(signalChannel->property().object(), signalMethod, nullptr, QMetaMethod());
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

QmlEditFragment *CodeQmlHandler::openConnection(int position){
    if ( !m_document )
        return nullptr;

    Q_D(CodeQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);

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
        QmlEditFragment* ef = new QmlEditFragment(importDecl, this);

        auto model = importsModel();

        int startPosition = m_target->findBlockByNumber(model->firstBlock()).position();
        auto lastBlock = m_target->findBlockByNumber(model->lastBlock()-1);
        int length = lastBlock.position() + lastBlock.length() - startPosition;
        ef->declaration()->setSection(
            m_document->createSection(QmlEditFragment::Section, startPosition, length)
        );
        ef->declaration()->section()->setUserData(ef);

        ef->declaration()->section()->onTextChanged(
                    [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
        {
            auto projectDocument = section->document();
            auto editingFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

            if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

                int editLength = editingFragment->declaration()->valueLength();
                editingFragment->declaration()->setValueLength(editLength - charsRemoved + addedText.size());

            } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
                removeEditingFragment(editingFragment);
            } else {
                int editLength = editingFragment->declaration()->valueLength();
                editingFragment->declaration()->setValueLength(editLength - charsRemoved + addedText.size());
            }

        });

        addEditingFragment(ef);
        ef->setParent(this);
        DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
        if ( dh )
            dh->requestCursorPosition(ef->position());

        return ef;
    }

    QmlDeclaration::Ptr declaration = properties.first();

    auto test = findFragmentByPosition(declaration->position());
    if (test && test->declaration()->position() == declaration->position()) // it was already opened
    {
        return test;
    }

    QmlEditFragment* ef = createInjectionChannel(declaration);

    if ( !ef ){
        QmlError(m_engine, CREATE_EXCEPTION(lv::Exception, "Cannot create injection channel for declaration: " + QString::number(declaration->position()).toStdString(), lv::Exception::toCode("~Injection")), this).jsThrow();
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

    QmlBindingChannel::Ptr inputChannel = ef->channel();
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
    if ( !m_document || !editParent )
        return nullptr;
    Q_D(CodeQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);

    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
    if ( properties.isEmpty() )
        return nullptr;

    QmlDeclaration::Ptr declaration = properties.first();

    auto test = findFragmentByPosition(declaration->position());
    if (test && test->declaration()->position() == declaration->position()) // it was already opened
    {
        return test;
    }

    QmlEditFragment* ef = createInjectionChannel(declaration, editParent);
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

    QmlBindingChannel::Ptr inputChannel = ef->channel();
    if ( inputChannel && inputChannel->listIndex() == -1 ){
        inputChannel->property().connectNotifySignal(ef, SLOT(updateValue()));
    }


    if (ef->location() == QmlEditFragment::Object)
        populateObjectInfoForFragment(ef);
    if (ef->location() == QmlEditFragment::Property)
        populatePropertyInfoForFragment(ef);
    editParent->addChildFragment(ef);
    ef->setParent(editParent);

    rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(ef->valuePosition());

    return ef;
}

lv::QmlEditFragment *CodeQmlHandler::createReadOnlyFragment(QmlEditFragment *parentFragment, QString name)
{
    Q_D(CodeQmlHandler);
    int position = parentFragment->valuePosition() + parentFragment->valueLength()-1;
    QTextCursor cursor(m_target);
    cursor.setPosition(position);

    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

    QStringList expression(name);
    QmlDeclaration::Ptr declaration = nullptr;

    QmlScopeSnap scope = d->snapScope();
    QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
        scope.quickObjectDeclarationType(ctx->objectTypePath()), expression, cursor.position()
    );

    if ( propChain.size() == expression.size() && propChain.size() > 0 ){
        QmlScopeSnap::PropertyReference& propref = propChain.last();
        QmlTypeReference qlt = propref.resultType();

        if ( !qlt.isEmpty() ){
            declaration = QmlDeclaration::create(
                expression,
                propref.resultType(),
                propref.propertyObjectType(),
                position,
                0,
                m_document
            );
            declaration->setValuePositionOffset(0);
        }
    }

    auto result = new QmlEditFragment(declaration, this);
    result->checkIfGroup();

    result->addFragmentType(QmlEditFragment::FragmentType::ReadOnly);
    parentFragment->addChildFragment(result);
    result->setParent(parentFragment);

    return result;
}

QList<QObject *> CodeQmlHandler::openNestedObjects(QmlEditFragment *edit){
    Q_D(CodeQmlHandler);

    QList<QObject*> fragments;

    d->syncParse(m_document);
    d->syncObjects(m_document);

    auto docString = m_document->contentString();

    QmlScopeSnap scope = d->snapScope();

    DocumentQmlValueObjects::Ptr objects = d->documentObjects();
    DocumentQmlValueObjects::RangeObject* currentOb = objects->objectAtPosition(edit->position());

    if ( currentOb ){
        for ( int i = 0; i < currentOb->children.size(); ++i ){
            DocumentQmlValueObjects::RangeObject* child = currentOb->children[i];
            QString currentObDeclaration = m_document->substring(child->begin, child->identifierEnd - child->begin);
            int splitter = currentObDeclaration.indexOf('.');
            QString obName = currentObDeclaration.mid(splitter + 1);
            QString obNs   = splitter == -1 ? "" : currentObDeclaration.mid(0, splitter);

            QmlInheritanceInfo obPath = scope.getTypePath(obNs, obName);

            QString id;
            // find id
            for (int k = 0; k < child->properties.size(); ++k)
            {
                auto prop = child->properties[k];
                if (prop->name().size() == 1 && prop->name()[0] == "id"){
                    id = docString.mid(prop->valueBegin, prop->end - prop->valueBegin);
                    break;
                }
            }

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

                auto test = findFragmentByPosition(declaration->position());
                if (test && test->declaration()->position() == declaration->position()) // it was already opened
                {
                    fragments.append(test);
                    continue;
                }

                QmlEditFragment* ef = createInjectionChannel(declaration, edit);
                if ( !ef ){
                    continue;
                }
                ef->declaration()->setSection(m_document->createSection(
                    QmlEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
                ));

                ef->setObjectId(id);
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

                if (ef->location() == QmlEditFragment::Object)
                    populateObjectInfoForFragment(ef);
                edit->addChildFragment(ef);
                ef->setParent(edit);

//                ef->setRelativeBinding(bp);

                fragments.append(ef);

            }
        }
    }

    return fragments;
}

QList<QObject *> CodeQmlHandler::openNestedProperties(QmlEditFragment *edit){
    Q_D(CodeQmlHandler);

    QList<QObject*> fragments;
    d->syncParse(m_document);
    d->syncObjects(m_document);
    QmlScopeSnap scope = d->snapScope();

    if (edit->isGroup()){
        auto children = edit->childFragments();
        for (auto child: children)
            fragments.push_back(qobject_cast<QObject*>(child));
        return fragments;
    }

    QString source = m_document->contentString();
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(m_document->file()->path());
    docinfo->parse(source);

    DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
    DocumentQmlValueObjects::RangeObject* currentOb = d->documentObjects()->objectAtPosition(edit->position());

    if ( !currentOb ) return fragments;

    for ( int i = 0; i < currentOb->properties.size(); ++i ){
        DocumentQmlValueObjects::RangeProperty* rp = currentOb->properties[i];

        if (rp->name().size() == 1 && rp->name()[0] == "id"){
            const QString& document = m_document->contentString();
            QString id = document.mid(rp->valueBegin, rp->end-rp->valueBegin);
            edit->setObjectId(id);
            continue;
        }


        QmlEditFragment* p = edit;
        for (int n = 0; n < rp->name().length(); ++n){
            QString propName = rp->name()[n];
            auto child = findChildPropertyFragmentByName(p, propName);
            if (!child) {
                if ( n == rp->name().length()-1 ){
                    QTextCursor cursor(m_target);
                    cursor.setPosition(rp->begin);

                    QString propertyType = rp->type();

                    QList<QmlDeclaration::Ptr> properties = getDeclarations(cursor);
                    if ( properties.isEmpty() )
                        continue;
                    QmlDeclaration::Ptr property = properties.first();

                    if ( propertyType.isEmpty() ){

                        QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
                            scope.quickObjectDeclarationType(rp->object()), rp->name(), rp->begin
                        );

                        if ( propChain.size() == rp->name().size() && propChain.size() > 0 ){
                            QmlScopeSnap::PropertyReference& propref = propChain.last();
                            property = QmlDeclaration::create(
                                rp->name(),
                                propref.resultType(),
                                propref.propertyObjectType(),
                                rp->begin,
                                rp->propertyEnd - rp->begin,
                                m_document
                            );
                        }
                    } else {
                        QmlTypeReference qlt;
                        if ( !QmlTypeInfo::isObject(propertyType) ){
                            qlt = QmlTypeReference(QmlTypeReference::Qml, rp->type());
                        }
                        if ( !qlt.isEmpty() ){
                            QmlTypeInfo::Ptr tr = scope.getType(rp->object());

                            property = QmlDeclaration::create(
                                rp->name(), qlt, tr->prefereredType(), rp->begin, rp->propertyEnd - rp->begin, m_document
                            );
                        }
                    }

                    QmlEditFragment* ef = createInjectionChannel(property, edit);

                    if (!ef)
                        continue;

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

                    QmlBindingChannel::Ptr inputChannel = ef->channel();
                    if ( inputChannel && inputChannel->listIndex() == -1 ){
                        inputChannel->property().connectNotifySignal(ef, SLOT(updateValue()));
                    }

                    if (ef->location() == QmlEditFragment::Property)
                        populatePropertyInfoForFragment(ef);



                    p->addChildFragment(ef);
                    ef->setParent(p);

                    rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

                    DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
                    if ( dh )
                        dh->requestCursorPosition(ef->valuePosition());
                } else {
                    child = createReadOnlyFragment(p, propName);
                }
            }
            else {
            }
            if ( n == 0 )
                fragments.push_back(child);

            p = child;

        }
    }

    return fragments;
}

void CodeQmlHandler::removeConnection(QmlEditFragment *edit){
    m_editContainer->derefEdit(edit);
}

void CodeQmlHandler::eraseObject(QmlEditFragment *edit){
    QList<QObject*> toRemove;

    bool objectProperty = false;
    const QmlBindingChannel::Ptr& bc = edit->channel();
    if ( bc->isEnabled() ){
        if ( bc->property().propertyTypeCategory() == QQmlProperty::List ){
            QQmlListReference ppref = qvariant_cast<QQmlListReference>(bc->property().read());
            if (ppref.canAt()){
                QObject* parent = ppref.count() > 0 ? ppref.at(0)->parent() : ppref.object();

                // create correct order for list reference
                QObjectList ordered;
                for (auto child: parent->children())
                {
                    bool found = false;
                    for (int i = 0; i < ppref.count(); ++i)
                        if (child == ppref.at(i)){
                            found = true;
                            break;
                        }
                    if (found)
                        ordered.push_back(child);
                }

                toRemove.append(ordered[bc->listIndex()]);
            }
        } else {
            objectProperty = true;
            bc->property().write(QVariant::fromValue(nullptr));
        }
    }

    int pos = edit->declaration()->valuePosition();
    int len = edit->declaration()->valueLength();

    removeEditingFragment(edit);

    m_document->addEditingState(ProjectDocument::Runtime);
    m_document->insert(pos, len, objectProperty ? "null" : "");
    m_document->removeEditingState(ProjectDocument::Runtime);

    if ( !toRemove.isEmpty() ){
        for ( QObject* o : toRemove ){
            o->deleteLater();
        }
        m_bindingChannels->desyncInactiveChannels();
    }
}

QString CodeQmlHandler::propertyType(QmlEditFragment *edit, const QString &propertyName){
    Q_D(CodeQmlHandler);
    QmlScopeSnap snap = d->snapScope();

    QmlInheritanceInfo ii = snap.getTypePath(edit->declaration()->type());
    for ( const QmlTypeInfo::Ptr& ti : ii.nodes ){
        QmlPropertyInfo pi = ti->propertyAt(propertyName);
        if ( pi.isValid() )
            return pi.typeName.name();
    }
    return "";
}

lv::PaletteList* CodeQmlHandler::findPalettesFromFragment(lv::QmlEditFragment* fragment, bool includeExpandables)
{
    if (!fragment) return nullptr;

    if (!fragment->isOfFragmentType(QmlEditFragment::ReadOnly)){
        return findPalettes(fragment->position(), includeExpandables);
    }


    auto declaration = fragment->declaration();
    return palettesForDeclaration(declaration, includeExpandables);
}


/**
 * \brief Finds the available list of palettes at the current \p cursor position
 */
lv::PaletteList* CodeQmlHandler::findPalettes(int position, bool includeExpandables){
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

    return palettesForDeclaration(declaration, includeExpandables);
}

/**
 * \brief Opens the palette \p index from a given \p paletteList
 */
QJSValue CodeQmlHandler::openPalette(lv::QmlEditFragment* edit, lv::PaletteList *paletteList, int index){
    if ( !m_document || !edit || !paletteList )
        return QJSValue();

    // check if duplicate
    PaletteLoader* paletteLoader = paletteList->loaderAt(index);

    if ( edit->bindingPalette() && edit->bindingPalette()->path() == PaletteContainer::palettePath(paletteLoader) ){
        edit->addPalette(edit->bindingPalette());
        return m_engine->engine()->newQObject(edit->bindingPalette());
    }

    if ( PaletteContainer::hasItem(paletteLoader) ){
        CodePalette* palette = paletteList->loadAt(index);
        palette->setEditFragment(edit);
        edit->addPalette(palette);
        edit->updatePaletteValue(palette);


        connect(palette, &CodePalette::valueChanged, edit, &QmlEditFragment::updateFromPalette);

        rehighlightSection(edit->position(), edit->valuePosition() + edit->valueLength());

        DocumentHandler* dh = static_cast<DocumentHandler*>(parent());
        if ( dh )
            dh->requestCursorPosition(edit->valuePosition());

        return m_engine->engine()->newQObject(palette);
    } else {
        return paletteList->contentAt(index);
    }
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

    lv::QmlEditFragment* edit = palette->editFragment();
    if ( edit ){
        edit->removePalette(palette);
    }
    return edit;
}

QString CodeQmlHandler::defaultPalette(QmlEditFragment *fragment){
    if ( !fragment )
        return nullptr;

    QString result;
    if ( fragment->location() == QmlEditFragment::Property ){
        result = m_settings->defaultPalette(fragment->declaration()->parentType().join() + "." + fragment->identifier());
    }
    if ( result.isEmpty() ){
        result = m_settings->defaultPalette(fragment->type());
    }

    for ( auto it = fragment->begin(); it != fragment->end(); ++it ){
        CodePalette* loadedPalette = *it;
        if (loadedPalette->name() == result){
            result = "";
            break;
        }
    }

    return result;
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
    palette->setEditFragment(edit);

    edit->setBindingPalette(palette);
    edit->updatePaletteValue(palette);

    connect(palette, &CodePalette::valueChanged, edit, &QmlEditFragment::updateFromPalette);

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
    QJSValue ob = m_engine->engine()->newObject();

    ob.setProperty("start", m_target->findBlock(start).blockNumber());
    ob.setProperty("end", m_target->findBlock(end).blockNumber());

    return ob;
}

lv::QmlImportsModel *CodeQmlHandler::importsModel(){
    Q_D(CodeQmlHandler);

    lv::QmlImportsModel* result = new QmlImportsModel(m_engine, this);
    d->syncObjects(m_document);

    DocumentQmlInfo::Ptr docinfo = d->documentInfo();

    if (docinfo->isParsedCorrectly())
    {
        auto imports = docinfo->internalBind()->imports();
        for ( QList<QmlJS::ImportInfo>::iterator it = imports.begin(); it != imports.end(); ++it ){

            QString module = it->name();
            QString version = it->version().majorVersion() != -1 ? (QString::number(it->version().majorVersion()) + "." + QString::number(it->version().minorVersion())) : "";
            QString qual = it->as();
            int line = it->ast()? static_cast<int>(it->ast()->importToken.startLine-1) : -1;

            result->addItem(module, version, qual, line);
        }
    } else {
        //TOMOVE: Move to DocumentQmlInfo::parseImports()
        // manual parse
        QString content = m_document->contentString();
        auto lines = content.split('\n');
        QList<std::pair<QStringList, int>> mid;

        for (int i = 0; i < lines.size(); ++i)
        {
            if (lines[i].length() == 0) continue;
            QStringList fragments = lines[i].split(';');
            for (auto fragment: fragments){
                auto parts = fragment.split(' ',  QString::SkipEmptyParts);
                
                if (parts.size() != 3 && parts.size() != 5)
                    return result;

                if (parts[0] != "import")
                    return result;

                if (parts.size() == 5 && parts[3] != "as")
                    return result;

                auto p = std::make_pair(QStringList(), i);

                p.first.push_back(parts[1]);
                p.first.push_back(parts[2]);
                p.first.push_back(parts.size() == 5 ? parts[4] : "");

                mid.push_back(p);
            }
        }

        for (auto p: mid){
            result->addItem(p.first[0], p.first[1], p.first[2], p.second);
        }
    }
    return result;
}

void CodeQmlHandler::addLineAtPosition(QString line, int pos)
{
    //TOMOVE: to ProjectDocument
    if (!m_target) return;

    m_document->addEditingState(ProjectDocument::Palette);

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

    m_document->removeEditingState(ProjectDocument::Palette);
}

void CodeQmlHandler::removeLineAtPosition(int pos)
{
    //TOMOVE: to ProjectDocument
    if (!m_target) return;
    auto block = m_target->findBlockByNumber(pos);
    m_document->addEditingState(ProjectDocument::Palette);
    QTextCursor c(block);
    c.beginEditBlock();
    c.select(QTextCursor::BlockUnderCursor);
    c.removeSelectedText();
    c.endEditBlock();
    m_document->removeEditingState(ProjectDocument::Palette);
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

            QmlEditFragment* edit = m_editContainer->topEditAtPosition(firstdecl->position());
            if ( edit ){
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

        QmlEditFragment* edit = m_editContainer->topEditAtPosition(position);
        if ( edit ){
            edit->removeBindingPalette();
            if ( edit->totalPalettes() == 0 ){
                m_editContainer->removeEdit(edit);
            }
        }
    }

    rehighlightSection(position, position + length);
}

QJSValue CodeQmlHandler::expand(QmlEditFragment *edit, const QJSValue &val){
    Q_D(CodeQmlHandler);
    if ( val.hasProperty("palettes") ){
        QJSValue palettesVal = val.property("palettes");
        QJSValueIterator palettesIt(palettesVal);
        while ( palettesIt.next() ){
            QString paletteName = palettesIt.value().toString();

            PaletteLoader* paletteLoader = d->projectHandler->paletteContainer()->findPaletteByName(paletteName);
            if ( !paletteLoader )
                return QJSValue();

            // check if duplicate
            for ( auto it = edit->begin(); it != edit->end(); ++it ){
                CodePalette* loadedPalette = *it;
                if ( loadedPalette->path() == PaletteContainer::palettePath(paletteLoader) ){
                    return m_engine->engine()->newQObject(loadedPalette);
                }
            }

            if ( edit->bindingPalette() && edit->bindingPalette()->path() == PaletteContainer::palettePath(paletteLoader) ){
                edit->addPalette(edit->bindingPalette());
                return m_engine->engine()->newQObject(edit->bindingPalette());
            }

            if ( PaletteContainer::hasItem(paletteLoader) ){
                CodePalette* palette = d->projectHandler->paletteContainer()->createPalette(paletteLoader);
                m_engine->engine()->setObjectOwnership(palette, QQmlEngine::CppOwnership);

                palette->setEditFragment(edit);

                edit->addPalette(palette);
                edit->updatePaletteValue(palette);

                connect(palette, &CodePalette::valueChanged, edit, &QmlEditFragment::updateFromPalette);

                rehighlightSection(edit->position(), edit->valuePosition() + edit->valueLength());

                return m_engine->engine()->newQObject(palette);
            } else {
                return d->projectHandler->paletteContainer()->paletteContent(paletteLoader);
            }
        }
    }
    return QJSValue();
}

/**
 * \brief Opens an editing palette for the given \p edit fragment
 */
lv::CodePalette* CodeQmlHandler::edit(lv::QmlEditFragment *edit){
    Q_D(CodeQmlHandler);
    if ( !m_document )
        return nullptr;

    QList<QmlEditFragment*> toRemove;

    QmlEditFragment* currentEdit = m_editContainer->topEditAtPosition(edit->declaration()->position());

    if ( currentEdit ){
        if ( currentEdit->totalPalettes() == 1 && (*currentEdit->begin())->type() == "edit/qml" )
             return nullptr;

        vlog("editqmljs-codehandler").v() <<
            "Removing editing frag  with no of palettes \'" << currentEdit->totalPalettes() << "\' at " <<
            edit->declaration()->position() << " due to edit.";

        if ( currentEdit != edit )
            toRemove.append(currentEdit);
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

    palette->setEditFragment(edit);

    connect(palette, &CodePalette::valueChanged, [this, edit](){
        if ( edit->totalPalettes() > 0 ){
            CodePalette* cp = *edit->begin();
            edit->commit(cp->value());
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
QmlAddContainer *CodeQmlHandler::getAddOptions(int position, int filter, lv::QmlEditFragment* fragment){
    Q_D(CodeQmlHandler);
    if ( !m_document || !m_target )
        return nullptr;
    QmlScopeSnap scope = d->snapScope();

    QStringList objectTypePath;
    QmlInheritanceInfo typePath;
    int propertyPosition = 0;
    if (filter & AddOptionsFilter::ReadOnly){
        auto declaration = fragment->declaration();

        objectTypePath = QStringList(declaration->type().name());
        typePath = scope.generateTypePathFromClassName(declaration->type().name(), declaration->type().path());

        propertyPosition = fragment->position();
    } else {
        QTextCursor cursor(m_target);
        cursor.setPosition(position);
        QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

        ctx->expressionPath();

        QStringList expression;
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

        }
        else if ( ctx->context() & QmlCompletionContext::InRhsofBinding ){
            expression     = ctx->propertyPath();
            propertyLength = DocumentQmlValueScanner::getExpressionExtent(m_target, ctx->propertyPosition());
            propertyPosition = ctx->propertyPosition();
        }
        else propertyPosition = position;

        if ( propertyPosition == -1 )
            return nullptr;

        objectTypePath = ctx->objectTypePath();

        if (!ctx->objectTypePath().empty()){
            QString type = ctx->objectTypeName();
            QString typeNamespace = ctx->objectTypePath().size() > 1 ? ctx->objectTypePath()[0] : "";
            typePath = scope.getTypePath(typeNamespace, type);
        }
    }

    QmlAddContainer* addContainer = new QmlAddContainer(propertyPosition, objectTypePath);

    addContainer->model()->addPropertiesAndFunctionsToModel(typePath, filter);
    if ((filter & AddOptionsFilter::ReadOnly) == 0){
        addContainer->model()->addObjectsToModel(scope);
    }
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
        bool assignDefault,
        lv::QmlEditFragment* parentGroup)
{
    Q_D(CodeQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);

    DocumentQmlValueScanner qvs(m_document, position, 1);
    int blockStart = qvs.getBlockStart(position) + 1;
    int blockEnd = qvs.getBlockEnd(position);

    QTextBlock tbStart = m_target->findBlock(blockStart);
    QTextBlock tbEnd   = m_target->findBlock(blockEnd);

    QString insertionText;
    int insertionPosition = position;
    int cursorPosition = position;

    QString fullName = name;
    if (parentGroup){
        auto p=parentGroup;
        while (p){
            QmlEditFragment *group = qobject_cast<QmlEditFragment*>(p);

            if (group && group->isGroup()){
                fullName = group->identifier() + "." + fullName;
                p = p->parentFragment();
            } else break;
        }
    }


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
            QString propertyName = p->name().join(".");

            if ( propertyName == fullName ){ // property already exists, simply position the cursor accordingly
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
        insertionText = "; " + fullName + ": " + (assignDefault ? QmlTypeInfo::typeDefaultValue(type) : "");
        cursorPosition = insertionPosition + fullName.size() + 4;
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

        insertionText = indent + (found ? "" : "    ")+ fullName + ": " + (assignDefault ? QmlTypeInfo::typeDefaultValue(type) : "") + "\n";
        cursorPosition = insertionPosition + indent.size() + (found ? 0:4) + fullName.size() + 2;
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

    return cursorPosition - fullName.size() - 2;
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

    if (blockEnd == 0) blockEnd = m_target->characterCount() -1;

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

int CodeQmlHandler::insertRootItem(const QString &name)
{
    Q_D(CodeQmlHandler);
    d->syncObjects(m_document);

    // add the root via code

    QString type; QString id;
    if (name.contains('#'))
    {
        auto spl = name.split('#');
        type = spl[0];
        id = spl[1];
    } else type = name;

    int insertionPosition = m_target->characterCount()-1;

    QString insertionText = "\n" + type + "{\n";
    if (id != "") insertionText += "    id: " + id;
    insertionText += "\n}\n";

    m_document->addEditingState(ProjectDocument::Palette);
    QTextCursor cs(m_target);
    cs.setPosition(insertionPosition);
    cs.beginEditBlock();
    cs.insertText(insertionText);
    cs.endEditBlock();
    m_document->removeEditingState(ProjectDocument::Palette);
    m_scopeTimer.stop();
    updateScope();

    d->syncObjects(m_document);

    QObject* newRoot = QmlEditFragment::createObject(
        d->documentInfo(), type + "{}", "temp"
    );
    if ( !newRoot )
        return -1;

    Project* project = d->projectHandler->project();
    Runnable* r = project->runnables()->runnableAt(m_document->file()->path());

    if ( !r )
        return -1;

    QQmlContext* ctx = r->createContext();
    newRoot->setParent(r->runSpace());

    QQuickItem* newRootItem = qobject_cast<QQuickItem*>(newRoot);
    QQuickItem* runSpaceItem = qobject_cast<QQuickItem*>(r->runSpace());
    if ( newRootItem && runSpaceItem ){
        newRootItem->setParentItem(runSpaceItem);
    }

    r->engineObjectReady(newRoot, QUrl(), r, ctx);

    return insertionPosition + 2;
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

    QmlBindingChannel::Ptr bc = edit->channel();

    if ( bc->canModify() ){
        QQmlProperty& p = bc->property();

        QObject* result = QmlEditFragment::createObject(
            d->documentInfo(), type + "{}", "temp"
        );

        if ( bc->type() == QmlBindingChannel::ListIndex || bc->type() == QmlBindingChannel::Object ){

            QObject* obat = bc->object();

            QQmlProperty assignmentProperty(obat);
            if ( assignmentProperty.propertyTypeCategory() == QQmlProperty::List ){
                QQmlListReference assignmentList = qvariant_cast<QQmlListReference>(assignmentProperty.read());
                vlog("editqmljs-codehandler").v() <<
                    "Adding : " << result->metaObject()->className() << " to " << obat->metaObject()->className() << ", "
                    "property " << assignmentProperty.name();

                QQmlEngine::setObjectOwnership(result, QQmlEngine::CppOwnership);
                if ( assignmentList.canAppend() ){
                    result->setParent(obat);
                    assignmentList.append(result);

                    m_bindingChannels->desyncInactiveChannels();

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

/**
 * \brief Update palette binding channels for a new runtime root.
 */
void CodeQmlHandler::updateRuntimeBindings(){
    QList<QmlEditFragment*> toRemove;
    for ( auto it = toRemove.begin(); it != toRemove.end(); ++it ){
        removeEditingFragment(*it);
    }
}

QmlEditFragment *CodeQmlHandler::createObject(int position, const QString &type, QmlEditFragment *parent, QObject *currentApp)
{
    int opos = addItem(position, "", type);
    addItemToRuntime(parent, type, currentApp);
    return openNestedConnection(parent, opos);
}

QJSValue CodeQmlHandler::getDocumentIds(){
    Q_D(CodeQmlHandler);
    QmlScopeSnap scope = d->snapScope();
    QStringList ids = scope.document->extractIds();

    QJSValue result = m_engine->engine()->newArray(static_cast<quint32>(ids.length()));

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
void CodeQmlHandler::newDocumentScanReady(DocumentQmlInfo::Ptr documentInfo){
    Q_D(CodeQmlHandler);
    if ( !documentInfo->isParsedCorrectly() && documentInfo->imports().isEmpty() ){
        documentInfo->transferImports(d->documentInfo()->imports());
    }

    d->assignDocumentFromBackgroundSync(documentInfo);
    m_newScope = true;

    QJSValueList args;
    args << d->wasDocumentUpdatedFromBackground();

    QLinkedList<QJSValue> callbacks = m_documentParseListeners;
    m_documentParseListeners.clear();

    for ( QJSValue cb : callbacks ){
        QJSValue res = cb.call();
        if ( res.isError() ){
            m_engine->throwError(res, this);
        }
    }
}

void CodeQmlHandler::__whenLibraryScanQueueCleared(){
    QLinkedList<QJSValue> callbacks = m_importsScannedListeners;
    m_importsScannedListeners.clear();

    for ( QJSValue cb : callbacks ){
        QJSValue res = cb.call();
        if ( res.isError() ){
            m_engine->throwError(res, this);
        }
    }
    emit importsScanned();
}

bool CodeQmlHandler::areImportsScanned(){
    Q_D(CodeQmlHandler);
    d->syncParse(m_document);
    QmlScopeSnap scope = d->snapScope();
    return scope.areDocumentLibrariesReady();
}

void CodeQmlHandler::onDocumentParse(QJSValue callback){
    Q_D(CodeQmlHandler);
    if ( d->wasDocumentUpdatedFromBackground() ){
        callback.call(QJSValueList() << true);
    } else {
        m_documentParseListeners.append(callback);
        if ( !d->isDocumentBeingScanned() ){
            d->documentQueuedForScanning();
            d->projectHandler->scanMonitor()->queueDocumentScan(m_document->file()->path(), m_document->contentString(), this);
        }
    }
}

void CodeQmlHandler::onImportsScanned(QJSValue callback){
    if ( areImportsScanned() ){
        QJSValue res = callback.call();
        if ( res.isError() ){
            m_engine->throwError(res, this);
        }
    } else {
        m_importsScannedListeners.append(callback);
    }
}

void CodeQmlHandler::removeSyncImportsListeners(){
    m_importsScannedListeners.clear();
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
    foreach (const QString& importPath, m_engine->engine()->importPathList()){
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

void CodeQmlHandler::suggestionsForNamespaceTypes(
    const QString &typeNameSpace,
    QList<CodeCompletionSuggestion> &suggestions) const
{
    Q_D(const CodeQmlHandler);
    QmlScopeSnap scope = d->snapScope();

    if ( typeNameSpace.isEmpty() ){
        QStringList exports = scope.project->libraryInfo(scope.document->path())->listExports();

        foreach( const QString& e, exports ){
            if ( e != scope.document->componentName() )
                suggestions << CodeCompletionSuggestion(e, "", "implicit", e);
        }

        for( const QString& defaultLibrary: scope.project->defaultLibraries() ){
            QStringList exportsIn;
            QmlLibraryInfo::Ptr lib = scope.project->libraryInfo(defaultLibrary);
            if ( lib )
                exportsIn = lib->listExports();
            for( const QString& e: exportsIn ){
                suggestions << CodeCompletionSuggestion(e, "", "QtQml", e, "qml/QtQml#" + e);
            }
        }
    }

    for( const DocumentQmlInfo::Import& imp: scope.document->imports() ){
        if ( imp.as() == typeNameSpace ){
            QmlLibraryInfo::Ptr lib = scope.project->libraryInfo(imp.uri());
            if ( lib ){
                QStringList exports = lib->listExports();
                for( const QString& e: exports ){
                    suggestions << CodeCompletionSuggestion(e, "", imp.uri(), e, "qml/" + lib->uri() + "#" + e);
                }
            }
        }
    }
}

void CodeQmlHandler::suggestionsForNamespaceImports(QList<CodeCompletionSuggestion> &suggestions){
    Q_D(CodeQmlHandler);
    QMap<QString, QString> imports;
    DocumentQmlInfo::Ptr document = d->documentInfo();

    QList<CodeCompletionSuggestion> localSuggestions;

    foreach( const DocumentQmlInfo::Import& imp, document->imports() ){
        if ( imp.as() != "" ){
            imports[imp.as()] = imp.uri();
        }
    }
    for ( auto it = imports.begin(); it != imports.end(); ++it ){
        localSuggestions << CodeCompletionSuggestion(it.key(), "import", it.value(), it.key());
    }
    std::sort(localSuggestions.begin(), localSuggestions.end(), &CodeCompletionSuggestion::compare);

    suggestions << localSuggestions;
}

void CodeQmlHandler::suggestionsForDocumentsIds(QList<CodeCompletionSuggestion> &suggestions) const{
    Q_D(const CodeQmlHandler);
    QStringList ids = d->documentInfo()->extractIds();
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
        } else if ( scope.document->hasImportAs(firstSegment) ){
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
            qmlhandler_helpers::suggestionsForObject(expression.documentValueObject, true, true, false, false, false, "", suggestions);
            qmlhandler_helpers::suggestionsForObjectPath(expression.typeReference, true, true, false, false, false, "", suggestions);
            return;
        } else if ( expression.lastSegmentType() == QmlScopeSnap::ExpressionChain::ClassNode ){
            qmlhandler_helpers::suggestionsForObject(expression.documentValueObject, true, true, false, true, false, "", suggestions);
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
            DocumentQmlInfo::ValueReference documentValue = scope.document->valueAtPosition(cursorPosition);
            if ( !scope.document->isValueNull(documentValue) )
                qmlhandler_helpers::suggestionsForObject(
                    scope.document->extractValueObject(documentValue), true, true, false, false, false, "", suggestions);

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

    DocumentQmlInfo::ValueReference documentValue = scope.document->valueAtPosition(cursorPosition);
    if ( !scope.document->isValueNull(documentValue) )
        qmlhandler_helpers::suggestionsForObject(
            scope.document->extractValueObject(documentValue), true, true, true, true, true, "", suggestions);

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
    if (declaration->type().name() == "slot")
        return false;
    return QmlTypeInfo::isObject(declaration->type().name());
}

PaletteList *CodeQmlHandler::palettesForDeclaration(QmlDeclaration::Ptr declaration, bool includeExpandables)
{
    Q_D(CodeQmlHandler);

    PaletteList* lpl = d->projectHandler->paletteContainer()->findPalettes(declaration->type().join(), includeExpandables);

    if (declaration->isForComponent()){
        lpl = d->projectHandler->paletteContainer()->findPalettes("qml/Component", includeExpandables, lpl);
    } else {
        if (declaration->type().name()[0].isUpper() && declaration->type().language() == QmlTypeReference::Qml){
            lpl = d->projectHandler->paletteContainer()->findPalettes("qml/Object", includeExpandables, lpl);
        }

        if ( declaration->isListDeclaration() ){
            lpl = d->projectHandler->paletteContainer()->findPalettes("qml/childlist", includeExpandables, lpl);
        } else {
            lpl = d->projectHandler->paletteContainer()->findPalettes("qml/property", includeExpandables, lpl);
        }
    }


    lpl->setPosition(declaration->position());

    return lpl;
}

void CodeQmlHandler::populateNestedObjectsForFragment(lv::QmlEditFragment *edit)
{
    Q_D(CodeQmlHandler);

    QList<QObject*> fragments;

    edit->clearNestedObjectsInfo();

    d->syncParse(m_document);
    d->syncObjects(m_document);

    QmlScopeSnap scope = d->snapScope();

    DocumentQmlValueObjects::Ptr objects = d->documentObjects();
    DocumentQmlValueObjects::RangeObject* currentOb = objects->objectAtPosition(edit->position());

    if ( currentOb ){
        for ( int i = 0; i < currentOb->children.size(); ++i ){
            DocumentQmlValueObjects::RangeObject* object = currentOb->children[i];
            QString currentObDeclaration = m_document->substring(object->begin, object->identifierEnd - object->begin);
            int splitter = currentObDeclaration.indexOf('.');
            QString obName = currentObDeclaration.mid(splitter + 1);
            QString obNs   = splitter == -1 ? "" : currentObDeclaration.mid(0, splitter);

            QVariantMap objInfo;

            objInfo.insert("name", obName);
            objInfo.insert("namespace", obNs);

            auto conn = findObjectFragmentByPosition(object->begin);
            auto cast = qobject_cast<QObject*>(conn);
            objInfo.insert("connection", QVariant::fromValue(cast));


            QVariantList propertiesList;
            QSet<QString> setOfNames;
            auto properties = object->properties;
            for (int p = 0; p < properties.length(); ++p)
            {
                auto property = properties[p];
                QVariantMap propMap;

                QString propName = property->name()[0];
                if (setOfNames.contains(propName)) continue;


                setOfNames.insert(propName);
                QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
                    scope.quickObjectDeclarationType(property->object()), QStringList(propName), property->begin
                );

                QmlScopeSnap::PropertyReference& propref = propChain.last();
                propMap.insert("isWritable", propref.property.isWritable);



                propMap.insert("name", propName);

                QString value = m_document->substring(property->valueBegin, property->end - property->valueBegin);

                if (property->name().size() == 1 && property->name()[0] == "id"){
                    objInfo.insert("id", value);
                    continue;
                }

                QmlEditFragment* fragment = nullptr;
                if (propref.property.isWritable)
                    fragment = openNestedConnection(conn, property->begin);
                else {
                    fragment = createReadOnlyFragment(conn, propName);

                }
                auto fcast = qobject_cast<QObject*>(fragment);
                propMap.insert("connection", QVariant::fromValue(fcast));


                QTextCursor cursor(m_document->textDocument());
                cursor.setPosition(property->end);
                QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);
                if (ctx->context() & QmlCompletionContext::InRhsofBinding)
                {
                    auto expression = ctx->expressionPath();
                    propMap.insert("value", expression);

                } else {
                    propMap.insert("value", value);
                }



                propertiesList.push_back(propMap);

            }


            objInfo.insert("properties", propertiesList);

            QVariantList subobjectsList;
            for (int so = 0; so < object->children.size(); ++so)
            {
                auto subobject = object->children[so];
                QVariantMap soMap;



                QString name = m_document->substring(subobject->begin, subobject->identifierEnd - subobject->begin);
                soMap.insert("name", name);


                auto conn = findObjectFragmentByPosition(subobject->begin);
                auto cast = qobject_cast<QObject*>(conn);
                soMap.insert("connection", QVariant::fromValue(cast));

                auto soProperties = subobject->properties;
                for (auto sop = 0; sop < soProperties.size(); ++sop)
                {
                    auto soProperty = soProperties[sop];

                    QString sopValue = m_document->substring(soProperty->valueBegin, soProperty->end - soProperty->valueBegin);

                    if (soProperty->name().size() == 1 && soProperty->name()[0] == "id"){
                        soMap.insert("id", sopValue);
                        continue;
                    }
                }


                subobjectsList.push_back(soMap);
            }

            objInfo.insert("subobjects", subobjectsList);

            edit->addNestedObjectInfo(objInfo);
        }
    }
}

void CodeQmlHandler::populateObjectInfoForFragment(QmlEditFragment *edit)
{
    Q_D(CodeQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);

    QmlScopeSnap scope = d->snapScope();

    DocumentQmlValueObjects::Ptr objects = d->documentObjects();
    DocumentQmlValueObjects::RangeObject* object = objects->objectAtPosition(edit->position());

    QString currentObDeclaration = m_document->substring(object->begin, object->identifierEnd - object->begin);

    int splitter = currentObDeclaration.indexOf('.');

    QString obName = currentObDeclaration.mid(splitter + 1);

    QString obNs   = splitter == -1 ? "" : currentObDeclaration.mid(0, splitter);

    QVariantMap objInfo;


    objInfo.insert("name", obName);
    objInfo.insert("namespace", obNs);

    auto cast = qobject_cast<QObject*>(edit);
    objInfo.insert("connection", QVariant::fromValue(cast));


    QVariantList propertiesList;
    auto properties = object->properties;
    for (int p = 0; p < properties.length(); ++p)
    {
        auto property = properties[p];
        QVariantMap propMap;
        propMap.insert("name", property->name());

        QString value = m_document->substring(property->valueBegin, property->end - property->valueBegin);

        if (property->name().size() == 1 && property->name()[0] == "id"){
            objInfo.insert("id", value);
            continue;
        }

        openNestedConnection(edit, property->begin);

        QTextCursor cursor(m_document->textDocument());
        cursor.setPosition(property->end);
        QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);
        if (ctx->context() & QmlCompletionContext::InRhsofBinding)
        {
            auto expression = ctx->expressionPath();
            propMap.insert("value", expression);

        } else {
            propMap.insert("value", value);
        }

        QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
            scope.quickObjectDeclarationType(property->object()), property->name(), property->begin
        );



        if ( propChain.size() == property->name().size() && propChain.size() > 0 ){
            QmlScopeSnap::PropertyReference& propref = propChain.last();
            propMap.insert("isWritable", propref.property.isWritable);
        }

        propertiesList.push_back(propMap);

    }


    objInfo.insert("properties", propertiesList);


    QVariantList subobjectsList;
    for (int so = 0; so < object->children.size(); ++so)
    {
        auto subobject = object->children[so];
        QVariantMap soMap;



        QString name = m_document->substring(subobject->begin, subobject->identifierEnd - subobject->begin);
        soMap.insert("name", name);


        auto conn = openNestedConnection(edit, subobject->begin);
        auto cast = qobject_cast<QObject*>(conn);
        soMap.insert("connection", QVariant::fromValue(cast));

        auto soProperties = subobject->properties;
        for (auto sop = 0; sop < soProperties.size(); ++sop)
        {
            auto soProperty = soProperties[sop];

            QString sopValue = m_document->substring(soProperty->valueBegin, soProperty->end - soProperty->valueBegin);

            if (soProperty->name().size() == 1 && soProperty->name()[0] == "id"){
                soMap.insert("id", sopValue);
                continue;
            }
        }


        subobjectsList.push_back(soMap);
    }

    objInfo.insert("subobjects", subobjectsList);

    edit->setObjectInfo(objInfo);
}

void CodeQmlHandler::populatePropertyInfoForFragment(QmlEditFragment *edit)
{
    Q_D(CodeQmlHandler);

    QVariantMap propMap;


    d->syncParse(m_document);
    d->syncObjects(m_document);

    auto cast = qobject_cast<QObject*>(edit);
    propMap.insert("connection", QVariant::fromValue(cast));

    QmlScopeSnap scope = d->snapScope();

    if (edit->isGroup()){

        propMap.insert("name", edit->identifier());
        propMap.insert("isWritable", false);

        edit->setObjectInfo(propMap);
        return;
    }

    DocumentQmlValueObjects::Ptr objects = d->documentObjects();
    auto set = objects->propertiesBetween(edit->declaration()->position(), edit->declaration()->position() + edit->declaration()->length());

    if (set.size() != 1) {
        edit->setObjectInfo(propMap);
        return;
    }

    auto property = set[0];

    propMap.insert("name", property->name());

    QString value = m_document->substring(property->valueBegin, property->end - property->valueBegin);

    QTextCursor cursor(m_document->textDocument());
    cursor.setPosition(property->end);
    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);
    if (ctx->context() & QmlCompletionContext::InRhsofBinding)
    {
        auto expression = ctx->expressionPath();
        propMap.insert("value", expression);

    } else {
        propMap.insert("value", value);
    }

    QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
        scope.quickObjectDeclarationType(property->object()), property->name(), property->begin
    );



    if ( propChain.size() == property->name().size() && propChain.size() > 0 ){
        QmlScopeSnap::PropertyReference& propref = propChain.last();
        propMap.insert("isWritable", propref.property.isWritable);
    }

    edit->setObjectInfo(propMap);
}

QVariantMap CodeQmlHandler::propertiesWritable(QmlEditFragment *ef)
{
    Q_D(CodeQmlHandler);
    QVariantMap result;

    if ( !m_document || !m_target )
        return result;

    if (ef->location() != QmlEditFragment::Object) return result;

    d->syncObjects(m_document);


    QmlScopeSnap scope = d->snapScope();

    QmlInheritanceInfo typePath;

    if (ef->isGroup()){
        typePath = scope.generateTypePathFromClassName(ef->declaration()->type().name(), ef->declaration()->type().path());
    }
    else {
        QTextCursor cursor(m_target);
        cursor.setPosition(ef->valuePosition() + ef->valueLength()-1);
        QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

        ctx->expressionPath();
        if (!ctx->objectTypePath().empty()){
            QString type = ctx->objectTypeName();
            QString typeNamespace = ctx->objectTypePath().size() > 1 ? ctx->objectTypePath()[0] : "";
            typePath = scope.getTypePath(typeNamespace, type);
        }

    }

    for ( auto it = typePath.nodes.begin(); it != typePath.nodes.end(); ++it ){
        const QmlTypeInfo::Ptr& ti = *it;
        for ( int i = 0; i < ti->totalProperties(); ++i ){
            QString propertyName = ti->propertyAt(i).name;
            if ( !propertyName.startsWith("__") ){
                result.insert(propertyName, ti->propertyAt(i).isWritable);
            }
        }
    }

    return result;
}

}// namespace

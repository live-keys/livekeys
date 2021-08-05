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

#include "live/languageqmlhandler.h"
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

#include "live/codehandler.h"
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
#include "live/shared.h"
#include "live/hookcontainer.h"

#include "qmlsuggestionmodel.h"
#include "qmlbuilder.h"
#include "qmlwatcher.h"
#include "qmlmetatypeinfo_p.h"

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
#include "live/codepalette.h"

namespace lv{

// LanguageQmlHandlerPrivate
// ----------------------------------------------------------------------------

/// \private
class LanguageQmlHandlerPrivate{

public:
    LanguageQmlHandlerPrivate()
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
                    m.name + "(", "function", ti->prefereredType().name(), completion
                );
            }
            if ( m.functionType == QmlFunctionInfo::Signal && suggestSignals ){
                QString completion = m.name + "(";// + m.parameters.join(", ") + ")";
                localSuggestions << CodeCompletionSuggestion(
                    m.name + "(", "signal", ti->prefereredType().name(), completion
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

// LanguageQmlHandler implementation
// ----------------------------------------------------------------------------

/**
 * \class lv::LanguageQmlHandler
 * \ingroup lveditqmljs
 * \brief Main code handler for the qml extension.
 *
 *
 * Handles code completion, palette control, property and item searching.
 */

/**
 * \brief LanguageQmlHandler constructor
 *
 * This is called through the ProjectQmlHandler.
 */
LanguageQmlHandler::LanguageQmlHandler(
        ViewEngine *engine,
        Project *,
        QmlJsSettings *settings,
        ProjectQmlExtension *projectHandler,
        ProjectDocument* document,
        CodeHandler *handler)
    : QObject(handler)
    , m_target(nullptr)
    , m_highlighter(new QmlJsHighlighter(settings, handler, nullptr))
    , m_settings(settings)
    , m_engine(engine)
    , m_completionContextFinder(new QmlCompletionContextFinder)
    , m_newScope(false)
    , m_editFragment(nullptr)
    , m_editContainer(new QmlEditFragmentContainer(this))
    , m_bindingChannels(nullptr)
    , m_importsFragment(nullptr)
    , m_rootFragment(nullptr)
    , d_ptr(new LanguageQmlHandlerPrivate)
{
    Q_D(LanguageQmlHandler);
    d->projectHandler = projectHandler;

    m_scopeTimer.setInterval(1000);
    m_scopeTimer.setSingleShot(true);
    connect(&m_scopeTimer, SIGNAL(timeout()), this, SLOT(__updateScope()));
    CodeHandler* dh = static_cast<CodeHandler*>(parent());
    connect(dh, SIGNAL(aboutToDeleteHandler()), this, SLOT(__aboutToDelete()));

    d->projectHandler->addLanguageQmlHandler(this);
    d->projectHandler->scanMonitor()->addScopeListener(this);
    connect(d->projectHandler->scanMonitor(), &QmlProjectMonitor::libraryScanQueueCleared,
            this, &LanguageQmlHandler::__whenLibraryScanQueueCleared);

    setDocument(document);

    m_bindingChannels = new DocumentQmlChannels(projectHandler->channelsDispatcher(), this);
}

/**
 * \brief LanguageQmlHandler destructor
 */
LanguageQmlHandler::~LanguageQmlHandler(){
    Q_D(LanguageQmlHandler);

    if ( d->projectHandler ){
        d->projectHandler->removeLanguageQmlHandler(this);
        d->projectHandler->scanMonitor()->removeScopeListener(this);
    }

    delete m_completionContextFinder;
}

/**
 * \brief Implementation of code completion assist.
 *
 * Handles bracket and paranthesis auto-completion together with suggestions population.
 */
void LanguageQmlHandler::assistCompletion(
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
void LanguageQmlHandler::setDocument(ProjectDocument *document){
    Q_D(LanguageQmlHandler);
    m_document      = document;
    m_target        = document->textDocument();
    m_highlighter->setTarget(m_target);

    if ( m_document ){
        connect(m_document->textDocument(), &QTextDocument::contentsChange,
                this, &LanguageQmlHandler::__documentContentsChanged);
        connect(m_document, &ProjectDocument::formatChanged, this, &LanguageQmlHandler::__documentFormatUpdate);
        connect(
            m_document->textDocument(), &QTextDocument::cursorPositionChanged,
            this, &LanguageQmlHandler::__cursorWritePositionChanged
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
void LanguageQmlHandler::__documentContentsChanged(int position, int, int){
    Q_D(LanguageQmlHandler);
    d->documentChanged();
    if ( !m_document->editingStateIs(ProjectDocument::Silent) ){
        if ( m_editFragment ){
            if ( position < m_editFragment->valuePosition() ||
                 position > m_editFragment->valuePosition() + m_editFragment->valueLength() )
            {
                cancelEdit();
            }
        }

        m_scopeTimer.start();
    }
}

void LanguageQmlHandler::__documentFormatUpdate(int position, int length){
    rehighlightSection(position, position + length);
}

/// \private
void LanguageQmlHandler::__cursorWritePositionChanged(QTextCursor cursor){
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

/**
 * \brief Called when a new scope is available from the scanMonitor
 */
void LanguageQmlHandler::__updateScope(){
    Q_D(LanguageQmlHandler);
    if ( !d->isDocumentBeingScanned() && d->projectHandler->scanMonitor()->hasProjectScope() && m_document ){
        d->projectHandler->scanMonitor()->queueDocumentScan(m_document->file()->path(), m_document->contentString(), this);
        d->documentQueuedForScanning();
    }
}

void LanguageQmlHandler::rehighlightSection(int start, int end){
    QTextBlock bl = m_target->findBlock(start);
    while ( bl.isValid() && bl.position() < end){
        rehighlightBlock(bl);
        bl = bl.next();
    }
}

void LanguageQmlHandler::resetProjectQmlExtension(){
    Q_D(LanguageQmlHandler);
    d->projectHandler = nullptr;
}

QString LanguageQmlHandler::getHelpEntity(int position){
    Q_D(LanguageQmlHandler);
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

void LanguageQmlHandler::addPropertiesAndFunctionsToModel(const QmlInheritanceInfo &typePath, QmlSuggestionModel *model, bool isForNode)
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

void LanguageQmlHandler::addObjectsToModel(const QmlScopeSnap &scope, QmlSuggestionModel *model)
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
void LanguageQmlHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_highlighter ){
        m_highlighter->rehighlightBlock(block);
    }
}

QmlDeclaration::Ptr LanguageQmlHandler::getDeclarationViaCompletionContext(int position){
    Q_D(const LanguageQmlHandler);

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
    } else if ( ctx->context() & QmlCompletionContext::InImport || ctx->context() & QmlCompletionContext::InImportVersion ){
        return createImportDeclaration();
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

                    bool isWritable = propref.property.isValid() ? propref.property.isWritable : false;

                    if ( !qlt.isEmpty() ){
                        return QmlDeclaration::create(
                            expression,
                            propref.resultType(),
                            propref.propertyObjectType(),
                            propertyPosition,
                            propertyLength,
                            m_document,
                            isWritable
                        );
                    }
                }
            }
        }
    } else { // check if this might be in imports
        if (position == 0){
            return createImportDeclaration();
        } else {
            bool isBlank = true;
            for (int i = 0; i<position; i++)
            {
                if (!m_document->textDocument()->characterAt(i).isSpace())
                {
                    isBlank = false;
                    break;
                }
            }
            if (isBlank){
                return createImportDeclaration();
            }
        }
    }
    return nullptr;
}

QList<QmlDeclaration::Ptr> LanguageQmlHandler::getDeclarationsViaParsedDocument(int position, int length){
    Q_D(LanguageQmlHandler);

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

            if ( propertyType.isEmpty() ){ // property is part of object
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
                        m_document,
                        propref.property.isValid() ? propref.property.isWritable : false
                    ));
                }
            } else {  // property declared in document
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
                        rp->name(),
                        qlt,
                        tr->prefereredType(),
                        rp->begin,
                        rp->propertyEnd - rp->begin,
                        m_document,
                        true
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
QList<QmlDeclaration::Ptr> LanguageQmlHandler::getDeclarations(int position, int length){
    QList<QmlDeclaration::Ptr> properties;
//    int length = position.selectionEnd() - position.selectionStart();

    if ( length == 0 ){
        QmlDeclaration::Ptr declaration = getDeclarationViaCompletionContext(position);
        if ( declaration )
            properties << declaration;

    } else { // multiple declarations were selected
        return getDeclarationsViaParsedDocument(position, length);
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
QmlEditFragment *LanguageQmlHandler::createInjectionChannel(QmlDeclaration::Ptr declaration, QmlEditFragment* parentEdit){
    Q_D(LanguageQmlHandler);

    if ( m_document ){
        d->syncParse(m_document);
        d->syncObjects(m_document);

        DocumentQmlInfo::TraversalResult tr = DocumentQmlInfo::findDeclarationPath(
            m_document, d->documentObjects()->root(), declaration
        );
        QmlBindingPath::Ptr bp = tr.bindingPath;

        if ( !bp )
            return nullptr;

        QString id = "";
        DocumentQmlValueObjects::RangeObject* rangeObject = nullptr;
        if ( tr.range && tr.range->rangeType() == DocumentQmlValueObjects::RangeItem::Object ){
            rangeObject = static_cast<DocumentQmlValueObjects::RangeObject*>(tr.range);
        } else if ( tr.range && tr.range->rangeType() == DocumentQmlValueObjects::RangeItem::Property ){
            DocumentQmlValueObjects::RangeProperty* rangeProperty = static_cast<DocumentQmlValueObjects::RangeProperty*>(tr.range);
            if ( rangeProperty->child )
                rangeObject = rangeProperty->child;
        }

        if ( rangeObject ){
            for (int k = 0; k < rangeObject->properties.size(); ++k){
                auto prop = rangeObject->properties[k];
                if (prop->name().size() == 1 && prop->name()[0] == "id"){
                    id = m_document->substring(prop->valueBegin, prop->end - prop->valueBegin);
                    break;
                }
            }
        }

        QmlEditFragment* ef = new QmlEditFragment(declaration, this);
        if ( !id.isEmpty() )
            ef->setObjectId(id);

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

        createChannelForFragment(parentEdit, ef, bp);

        if ( !ef->channel() ){
            delete ef;
            return nullptr;
        }
        return ef;
    }

    return nullptr;
}

void LanguageQmlHandler::__aboutToDelete()
{
    cancelEdit();

    if ( m_document ){
        m_editContainer->clearAllFragments();
    }
}

void LanguageQmlHandler::addItemToRunTimeImpl(QmlEditFragment *edit, const QString &ctype, const QJSValue &properties){
    Q_D(LanguageQmlHandler);

    if ( !edit )
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
        QString creationPath = m_document->file()->path();
        creationPath.replace(".qml", "_a.qml");

        QQmlContext* creationCtx = nullptr;
        if ( bc->type() == QmlBindingChannel::ListIndex || bc->type() == QmlBindingChannel::Object ){
            QObject* creationObj = bc->object();
            if ( creationObj )
                creationCtx = new QQmlContext(qmlContext(creationObj));
        }

        // Handle properties

        QList<std::tuple<QString, QString, QString> > props;
        if ( properties.isArray() ){
            QJSValueIterator it(properties);
            while ( it.hasNext() ){
                it.next();
                if ( it.name() != "length" ){
                    QJSValue propertyConfig = it.value();

                    if ( !propertyConfig.hasOwnProperty("name") || !propertyConfig.hasOwnProperty("type") ){
                        lv::Exception e = CREATE_EXCEPTION(
                            lv::Exception, "Property 'name' and 'type' are required.", lv::Exception::toCode("~Attributes")
                        );
                        m_engine->throwError(&e, this);
                        return;
                    }

                    QString name = propertyConfig.property("name").toString();
                    QString type = propertyConfig.property("type").toString();
                    QString value = "";
                    if ( propertyConfig.hasOwnProperty("value") ){
                        value = propertyConfig.property("value").toString();
                    } else {
                        value = QmlTypeInfo::typeDefaultValue(type);
                    }

                    props.append(std::make_tuple(type, name, value));
                }
            }
        }


        QObject* result = QmlEditFragment::createObject(
            d->documentInfo(), type, creationPath, nullptr, creationCtx, props
        );
        if ( !result )
            THROW_EXCEPTION(lv::Exception, "Failed to create object: " + type.toStdString(), Exception::toCode("~CreateObject"));

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
            edit->channel()->property().write(QVariant::fromValue(result));
        }
    }
}

QmlAddContainer* LanguageQmlHandler::getAddOptionsForFragment(QmlEditFragment *fragment, bool isReadOnly){
    Q_D(LanguageQmlHandler);
    if ( !m_document || !m_target )
        return nullptr;

    QmlScopeSnap scope = d->snapScope();
    d->syncObjects(m_document);
    scope.document->createRanges();

    int insertionPosition = 0;
    QmlDeclaration::Ptr declaration = nullptr;

    declaration = fragment->declaration();

    QmlInheritanceInfo typePath;
    DocumentQmlInfo::ValueReference documentValue = scope.document->valueAtPosition(declaration->position() + 1);
    // get declared type in the document first
    if ( !scope.document->isValueNull(documentValue) ){
        QmlTypeInfo::Ptr valueObject = scope.document->extractValueObject(documentValue);
        typePath.append(valueObject);
    }
    if ( !declaration->type().isEmpty() ){
        typePath.join(scope.getTypePath(declaration->type()));
    }

    auto p = fragment;
    if (isReadOnly){
        while (p && p->isOfFragmentType(QmlEditFragment::ReadOnly))
        {
            p = p->parentFragment();
        }
    }

    insertionPosition = p->valuePosition() + p->valueLength() - 1;

    QmlAddContainer* addContainer = new QmlAddContainer(insertionPosition, declaration->type());

    addContainer->model()->addPropertiesAndFunctionsToModel(typePath);

    if (addContainer->model()->supportsObjectNesting()){
        addContainer->model()->addObjectsToModel(scope);
    }
    return addContainer;
}

QmlAddContainer* LanguageQmlHandler::getAddOptionsForPosition(int position){

    Q_D(LanguageQmlHandler);
    if ( !m_document || !m_target )
        return nullptr;

    QmlScopeSnap scope = d->snapScope();
    d->syncObjects(m_document);
    scope.document->createRanges();

    QmlDeclaration::Ptr declaration = nullptr;

    QTextCursor cursor(m_target);
    cursor.setPosition(position);
    QmlInheritanceInfo typePath;
    QmlCompletionContext::ConstPtr ctx = m_completionContextFinder->getContext(cursor);

    ctx->expressionPath();

    QStringList expression;
    QStringList objectTypePath;
    int propertyPosition = 0;
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

    DocumentQmlInfo::ValueReference documentValue = scope.document->valueAtPosition(position);
    // get declared type in the document first
    if ( !scope.document->isValueNull(documentValue) ){
        QmlTypeInfo::Ptr valueObject = scope.document->extractValueObject(documentValue);
        typePath.append(valueObject);
    }

    QmlTypeReference objectType = typePath.languageType();

    if (!ctx->objectTypePath().empty()){
        QString type = ctx->objectTypeName();
        QString typeNamespace = ctx->objectTypePath().size() > 1 ? ctx->objectTypePath()[0] : "";

        QmlInheritanceInfo libtypePath = scope.getTypePath(typeNamespace, type);

        objectType = libtypePath.languageType();

        typePath.join(libtypePath);
    }

    QmlAddContainer* addContainer = new QmlAddContainer(position, objectType);

    addContainer->model()->addPropertiesAndFunctionsToModel(typePath);
    if ((addContainer->model()->supportsObjectNesting() || findRootPosition() == -1)){
        addContainer->model()->addObjectsToModel(scope);
    }

    return addContainer;
}


QmlDeclaration::Ptr LanguageQmlHandler::createImportDeclaration(){
    Q_D(LanguageQmlHandler);
    d->syncParse(m_document);

    DocumentQmlInfo::Ptr docinfo = d->documentInfo();
    if ( !docinfo->isParsedCorrectly() ){
        docinfo->tryExtractImports();
    }

    int identifierPosition = 0;
    int identifierLength = 0;

    if ( !docinfo->imports().isEmpty() ){
        int blockStart = docinfo->imports()[0].location().line() - 1;
        int blockEnd = docinfo->imports()[docinfo->imports().size()-1].location().line() - 1;

        identifierPosition = m_target->findBlockByNumber(blockStart).position();
        auto lastBlock = m_target->findBlockByNumber(blockEnd);
        identifierLength = lastBlock.position() + lastBlock.length() - 1 - identifierPosition;
    }

    return QmlDeclaration::create(
        QStringList(),
        QmlTypeReference(QmlTypeReference::Qml, "import"),
        QmlTypeReference(),
        identifierPosition, identifierLength,
        m_document
    );
}

/**
 * \brief Adds an editing fragment to the current document
 */
bool LanguageQmlHandler::addEditFragment(QmlEditFragment *edit){
    //TOMOVE
    return m_editContainer->addEdit(edit);
}

/**
 * \brief Removes an editing fragment from this document
 */
void LanguageQmlHandler::removeEditFragment(QmlEditFragment *edit){
    //TOMOVE
    m_editContainer->removeEdit(edit);
}

int LanguageQmlHandler::findImportsPosition(){
    Q_D(LanguageQmlHandler);
    d->syncParse(m_document);

    DocumentQmlInfo::Ptr docinfo = d->documentInfo();
    if ( !docinfo->isParsedCorrectly() ){
        docinfo->tryExtractImports();
    }

    if ( !docinfo->imports().isEmpty() ){
        return m_document->textDocument()->findBlockByNumber(
            docinfo->imports().first().location().line() - 1
        ).position() + docinfo->imports().first().location().column() + 7;

    }

    return 0;
}

int LanguageQmlHandler::findRootPosition(){
    Q_D(LanguageQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);

    if ( d->documentObjects()->root() ){
        return d->documentObjects()->root()->begin;
    }
    return -1;
}

lv::QmlEditFragment *LanguageQmlHandler::findObjectFragmentByPosition(int position)
{
    //TOMOVE
    return m_editContainer->findObjectFragmentByPosition(position);
}

lv::QmlEditFragment *LanguageQmlHandler::findFragmentByPosition(int position)
{
    //TOMOVE
    return m_editContainer->findFragmentByPosition(position);
}

QJSValue LanguageQmlHandler::editFragments(){
    //TOMOVE
    return m_editContainer->allEdits();
}


void LanguageQmlHandler::toggleComment(int position, int length)
{
    if ( !m_document )
        return;

    Q_D(LanguageQmlHandler);

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

void LanguageQmlHandler::suggestionsForProposedExpression(
        QmlDeclaration::Ptr declaration,
        const QString &expression,
        CodeCompletionModel *model,
        bool suggestFunctions) const
{
    Q_D(const LanguageQmlHandler);

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

bool LanguageQmlHandler::findBindingForExpression(lv::QmlEditFragment *edit, const QString &expression){
    Q_D(LanguageQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);
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

    if ( !expressionChain.isValid() && expression != "null" ){
        QmlError(
            m_engine,
            CREATE_EXCEPTION(
                lv::Exception,
                "Invalid expression: \'" + expression.toStdString() + "\'", lv::Exception::toCode("~Exprssion")),
                this
        ).jsThrow();
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

        QList<QmlDeclaration::Ptr> declarations = getDeclarations(end);

        if ( declarations.isEmpty() ){
            QmlError(
                m_engine,
                CREATE_EXCEPTION(
                    lv::Exception,
                    "Failed to match expression: " + expression.toStdString(), lv::Exception::toCode("~Expression")),
                    this
            ).jsThrow();
            return false;
        }

        DocumentQmlInfo::TraversalResult tr = DocumentQmlInfo::findDeclarationPath(
                    m_document,
                    d->documentObjects()->root(),
                    declarations.first());

        QmlBindingPath::Ptr newBp = tr.bindingPath;
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

        QmlBindingPath::Ptr editBp = edit->fullBindingPath();

        bp = editBp->parentObjectPath();
        if ( !bp ){
            bool isViewRoot = false;
            if ( edit->location() == QmlEditFragment::Property ){
                QmlEditFragment* editIt = edit;
                while ( true ){
                    if ( editIt->location() != QmlEditFragment::Property && !editIt->isGroup() ){
                        break;
                    }
                    if ( !editIt->parentFragment() )
                        break;

                    editIt = editIt->parentFragment();
                }

                if ( editIt->channel()->object() == edit->channel()->runnable()->viewRoot() ){
                    isViewRoot = true;
                }
            }

            if ( !isViewRoot)
                return false;

            bp = QmlBindingPath::create();
            bp->appendContextValue("runSpace");

        } else {
            for ( const QmlScopeSnap::PropertyReference& prop : expressionChain.propertyChain){
                bp->appendProperty(
                    prop.property.name,
                    QStringList() << prop.classTypePath.nodes.first()->prefereredType().join()
                );
            }
        }


    } else if ( expressionChain.hasProperty() ){ // <property...>
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
    } else if ( expression == "null" ){
        // disable the binding
        QmlBindingChannel::Ptr receivingChannel = edit->channel();
        if ( !receivingChannel->isEnabled() )
            return true;

        QmlBindingChannel::Ptr documentChannel = m_bindingChannels->selectedChannel();
        if ( !documentChannel )
            return true;

        if ( documentChannel->isBuilder() )
            return true;

        if ( receivingChannel->property().isValid() && receivingChannel->property().object() ){
            QQmlProperty receivingProperty = receivingChannel->property();
            QByteArray watcherName = "__" + receivingProperty.name().toUtf8() + "Watcher";
            QVariant previousWatcherVariant = receivingProperty.object()->property(watcherName);

            if ( previousWatcherVariant.isValid() ){
                QObject* previousWatcher = previousWatcherVariant.value<QObject*>();
                delete previousWatcher;
                receivingProperty.object()->setProperty(watcherName, QVariant());
            }

            QVariant val = receivingProperty.read();
            receivingProperty.write(val);

            return true;
        }
    } else if ( expression == "modelData" ){
        bp = QmlBindingPath::create();
        bp->appendContextValue("modelData");
    } else {
        QmlError(
            m_engine,
            CREATE_EXCEPTION(
                lv::Exception,
                "Failed to match expression: " + expression.toStdString(), lv::Exception::toCode("~Expression")),
                this
        ).jsThrow();
        return false;
    }

    QmlBindingChannel::Ptr receivingChannel = edit->channel();

    if ( receivingChannel->isEnabled() ){

        QmlBindingChannel::Ptr documentChannel = m_bindingChannels->selectedChannel();

        if ( documentChannel ){
            if ( documentChannel->isBuilder() )
                return true;

            QmlBindingChannel::Ptr writeChannel = DocumentQmlChannels::traverseBindingPathFrom(documentChannel, bp);

            if ( !writeChannel || !writeChannel->hasConnection() ){
                QmlError(
                    m_engine,
                    CREATE_EXCEPTION(
                        lv::Exception,
                        "Failed to find a binding channel at: " + bp->toString().toStdString(), lv::Exception::toCode("~Channel")),
                        this
                ).jsThrow();
                return false;
            } else {
                // width is the receiving, need to remove the previous listening channel

                if ( receivingChannel->property().isValid() && receivingChannel->property().object() ){

                    if ( writeChannel->type() == QmlBindingChannel::Property ){

                        QQmlProperty receivingProperty = receivingChannel->property();

                        QByteArray watcherName = "__" + receivingProperty.name().toUtf8() + "Watcher";
                        QVariant previousWatcherVariant = receivingProperty.object()->property(watcherName);
                        if ( previousWatcherVariant.isValid() ){
                            QObject* previousWatcher = previousWatcherVariant.value<QObject*>();
                            delete previousWatcher;
                            receivingProperty.object()->setProperty(watcherName, QVariant());
                        }

                        QmlPropertyWatcher* watcher = new QmlPropertyWatcher(writeChannel->property());
                        QVariant watcherValue = watcher->read();

                        bool receivingQJSValue = QString(receivingProperty.property().typeName()) == "QJSValue";
                        if ( watcherValue.canConvert<QJSValue>() ){ // qjsvalue needs to be unwrapped
                            if ( !receivingQJSValue ){ // unwrap if we don't need to receive QJSValue
                                QJSValue watcherJsValue = watcherValue.value<QJSValue>();
                                QList<Shared*> sharedObjects;
                                watcherValue = Shared::transfer(watcherJsValue, sharedObjects);
                            }
                        } else if ( receivingQJSValue ){ // wrap in QJSValue
                            watcherValue = QVariant::fromValue(Shared::transfer(watcherValue, m_engine->engine()));
                        }

                        receivingProperty.write(watcherValue);

                        QQmlEngine* captureEngine = m_engine->engine();

                        watcher->onChange([receivingProperty, captureEngine](const QmlPropertyWatcher& ww){
                            QVariant wwValue = ww.read();
                            bool receivingQJSValue = QString(receivingProperty.property().typeName()) == "QJSValue";
                            if ( wwValue.canConvert<QJSValue>() ){ // qjsvalue needs to be unwrapped
                                if ( !receivingQJSValue ){ // unwrap if we don't need to receive QJSValue
                                    QJSValue watcherJsValue = wwValue.value<QJSValue>();
                                    QList<Shared*> sharedObjects;
                                    wwValue = Shared::transfer(watcherJsValue, sharedObjects);
                                }
                            } else if ( receivingQJSValue ){ // wrap in QJSValue
                                wwValue = QVariant::fromValue(Shared::transfer(wwValue, captureEngine));
                            }
                            receivingProperty.write(wwValue);
                        });

                        // delete the watcher with the receiving channel
                        watcher->setParent(receivingChannel->property().object());
                        receivingProperty.object()->setProperty(watcherName, QVariant::fromValue(watcher));

                    } else if ( writeChannel->type() == QmlBindingChannel::ListIndex ){
                        QObject* writeValue = writeChannel->object();
                        receivingChannel->property().write(QVariant::fromValue(writeValue));
                    } else if ( writeChannel->type() == QmlBindingChannel::Object ){
                        QObject* writeValue = writeChannel->object();
                        receivingChannel->property().write(QVariant::fromValue(writeValue));
                    }

                }
            }
        }
    }

    return true;
}

bool LanguageQmlHandler::findFunctionBindingForExpression(QmlEditFragment *edit, const QString &expression){
    Q_D(LanguageQmlHandler);

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
        QmlError(
            m_engine,
            CREATE_EXCEPTION(
                lv::Exception,
                "Invalid expression: \'" + expression.toStdString() + "\'", lv::Exception::toCode("~Exprssion")),
                this
        ).jsThrow();
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

        QList<QmlDeclaration::Ptr> declarations = getDeclarations(end);

        d->syncParse(m_document);
        d->syncObjects(m_document);
        DocumentQmlInfo::TraversalResult tr = DocumentQmlInfo::findDeclarationPath(
                    m_document,
                    d->documentObjects()->root(),
                    declarations.first());

        QmlBindingPath::Ptr newBp = tr.bindingPath;

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

    } else if ( expressionChain.hasProperty() ) { // <property...>
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
    } else {
        QmlError(
            m_engine,
            CREATE_EXCEPTION(
                lv::Exception,
                "Failed to match expression: " + expression.toStdString(), lv::Exception::toCode("~Exprssion")),
                this
        ).jsThrow();
        return false;
    }

    if ( bp.isNull() ){
        QmlError(
            m_engine,
            CREATE_EXCEPTION(
                lv::Exception,
                "Failed to find path for expression: " + expression.toStdString(), lv::Exception::toCode("~Exprssion")),
                this
        ).jsThrow();
        return false;
    }

    QmlBindingChannel::Ptr signalChannel = edit->channel();
    if ( signalChannel->isEnabled() ){

        QmlBindingChannel::Ptr documentChannel = m_bindingChannels->selectedChannel();
        if ( documentChannel ){
            if ( documentChannel->isBuilder() )
                return true;

            QmlBindingChannel::Ptr functionChannel = DocumentQmlChannels::traverseBindingPathFrom(documentChannel, bp);
            if ( functionChannel.isNull() || !functionChannel->hasConnection() ){
                QmlError(
                    m_engine,
                    CREATE_EXCEPTION(
                        lv::Exception,
                        "Failed to find a binding channel at: " + bp->toString().toStdString(), lv::Exception::toCode("~Channel")),
                        this
                ).jsThrow();
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

QmlUsageGraphScanner *LanguageQmlHandler::createScanner(){
    Q_D(LanguageQmlHandler);
    return new QmlUsageGraphScanner(d->projectHandler->project(), d->snapScope());
}

QList<int> LanguageQmlHandler::languageFeatures() const{
    return {
        CodeHandler::LanguageHelp,
        CodeHandler::LanguageScope,
        CodeHandler::LanguageHighlighting,
        CodeHandler::LanguageCodeCompletion,
        CodeHandler::LanguageLayout
    };
}

QString LanguageQmlHandler::help(int position){
    return getHelpEntity(position);
}

QmlEditFragment *LanguageQmlHandler::openConnection(int position){
    if ( !m_document )
        return nullptr;

    Q_D(LanguageQmlHandler);

    d->syncParse(m_document);
    d->syncObjects(m_document);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(position);
    if ( properties.isEmpty() )
        return nullptr;

    QmlDeclaration::Ptr declaration = properties.first();

    auto test = findFragmentByPosition(declaration->position());
    if (test && test->declaration()->position() == declaration->position()) // it was already opened
    {
        return test;
    }

    QmlEditFragment* ef = nullptr;
    if ( declaration->isForImports() ){
        ef = new QmlEditFragment(declaration, this);
        QmlBindingChannel::Ptr documentChannel = m_bindingChannels->selectedChannel();
        if ( documentChannel ){
            QmlBindingPath::Ptr bp = QmlBindingPath::create();
            bp->appendContextValue("imports");
            auto ch = QmlBindingChannel::createForImports(bp, documentChannel->runnable());
            ef->setChannel(ch);
        }
    } else {
        ef = createInjectionChannel(declaration);
    }


    if ( !ef ){
        QmlError(m_engine, CREATE_EXCEPTION(
            lv::Exception,
            "Cannot create injection channel for declaration: " +
            QString::number(declaration->position()).toStdString(),
            lv::Exception::toCode("~Injection")),
        this).jsThrow();

        return nullptr;
    }

    ef->declaration()->setSection(m_document->createSection(
        QmlEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
    ));
    ef->declaration()->section()->setUserData(ef);
    ef->declaration()->section()->onTextChanged(
                [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
    {
        auto projectDocument = section->document();
        auto editFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

            int length = editFragment->declaration()->valueLength();
            editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

        } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
            removeEditFragment(editFragment);
        } else {
            int length = editFragment->declaration()->valueLength();
            editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
        }
    });

    QmlBindingChannel::Ptr inputChannel = ef->channel();
    if ( inputChannel && inputChannel->listIndex() == -1 ){
        inputChannel->property().connectNotifySignal(ef, SLOT(__updateValue()));
    }

    addEditFragment(ef);
    ef->setParent(this);

    rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

    CodeHandler* dh = static_cast<CodeHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(ef->valuePosition());

    return ef;
}

QmlEditFragment *LanguageQmlHandler::openNestedConnection(QmlEditFragment* editParent, int position){
    if ( !m_document || !editParent )
        return nullptr;
    Q_D(LanguageQmlHandler);
    d->syncParse(m_document);
    d->syncObjects(m_document);

    QList<QmlDeclaration::Ptr> properties = getDeclarations(position);
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
        auto editFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

            int length = editFragment->declaration()->valueLength();
            editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

        } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
            removeEditFragment(editFragment);
        } else {
            int length = editFragment->declaration()->valueLength();
            editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
        }
    });

    QmlBindingChannel::Ptr inputChannel = ef->channel();
    if ( inputChannel && inputChannel->listIndex() == -1 ){
        inputChannel->property().connectNotifySignal(ef, SLOT(__updateValue()));
    }

    editParent->addChildFragment(ef);

    rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

    CodeHandler* dh = static_cast<CodeHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(ef->valuePosition());

    return ef;
}

lv::QmlEditFragment *LanguageQmlHandler::openReadOnlyPropertyConnection(QmlEditFragment *parentFragment, QString propertyName)
{
    if ( !m_document )
        return nullptr;
    Q_D(LanguageQmlHandler);
    d->syncObjects(m_document);

    QmlScopeSnap scope = d->snapScope();

    int position = parentFragment->valuePosition() + parentFragment->valueLength() - 1;
    QList<QmlScopeSnap::PropertyReference> propChain = scope.getProperties(
        parentFragment->declaration()->type(), QStringList() << propertyName, position
    );

    if ( propChain.length() != 1 )
        return nullptr;

    QmlScopeSnap::PropertyReference& propref = propChain.last();
    QmlTypeReference qlt = propref.resultType();

    if ( qlt.isEmpty() )
        return nullptr;

    auto declaration = QmlDeclaration::create(
        QStringList() << propertyName,
        propref.resultType(),
        propref.propertyObjectType(),
        -1,
        0,
        m_document,
        false
    );
    declaration->setValuePositionOffset(0);

    QmlBindingPath::Ptr bp = parentFragment->fullBindingPath()->clone();
    bp->appendProperty(propertyName, QStringList() << parentFragment->type());

    auto result = new QmlEditFragment(declaration, this);
    createChannelForFragment(parentFragment, result, bp);

    result->checkIfGroup();

    result->addFragmentType(QmlEditFragment::FragmentType::ReadOnly);
    parentFragment->addChildFragment(result);

    return result;
}

QList<QObject *> LanguageQmlHandler::openNestedFragments(QmlEditFragment *edit, const QJSValue &options){
    Q_D(LanguageQmlHandler);

    QList<QObject*> fragments;
    d->syncParse(m_document);
    d->syncObjects(m_document);
    QmlScopeSnap scope = d->snapScope();

    bool iterateProperties = false;
    bool iterateObjects    = false;
    if ( options.isArray() ){
        QJSValueIterator it(options);
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                if ( it.value().toString() == "properties" ){
                    iterateProperties = true;
                }
                if ( it.value().toString() == "objects" ){
                    iterateObjects = true;
                }
            }
        }
    } else {
        iterateProperties = true;
        iterateObjects    = true;
    }

    if (edit->isGroup() && iterateProperties ){
        auto children = edit->childFragments();
        for (auto child: children)
            fragments.push_back(qobject_cast<QObject*>(child));
        return fragments;
    }

    DocumentQmlValueObjects::Ptr objects = d->documentObjects();

    int objectPosition = edit->position();
    if ( edit->location() == QmlEditFragment::Property ){
        objectPosition = edit->valuePosition();
    }

    DocumentQmlValueObjects::RangeObject* currentOb = objects->objectAtPosition(objectPosition);
    if ( !currentOb )
        return fragments;

    // iterate properties
    if ( iterateProperties ){

        for ( int i = 0; i < currentOb->properties.size(); ++i ){
            DocumentQmlValueObjects::RangeProperty* rp = currentOb->properties[i];

            if (rp->name().size() == 1 && rp->name()[0] == "id"){
                continue;
            }

            QmlEditFragment* parentEdit = edit;

            // iterate group properties
            for (int n = 0; n < rp->name().length(); ++n){
                QString propName = rp->name()[n];

                QmlEditFragment* child = nullptr;
                auto children = parentEdit->childFragments();
                for ( auto it = children.begin(); it != children.end(); ++it ){
                    if ( (*it)->identifier() == propName ){
                        child = *it;
                        break;
                    }
                }


                if (!child) {
                    if ( n == rp->name().length()-1 ){

                        QString propertyType = rp->type();

                        QList<QmlDeclaration::Ptr> properties = getDeclarations(rp->begin);
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
                                    m_document,
                                    propref.property.isValid() ? propref.property.isWritable : false
                                );
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

                                property = QmlDeclaration::create(
                                    rp->name(),
                                    qlt,
                                    tr->prefereredType(),
                                    rp->begin,
                                    rp->propertyEnd - rp->begin,
                                    m_document,
                                    true
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
                            auto editFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

                            if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

                                int length = editFragment->declaration()->valueLength();
                                editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

                            } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
                                removeEditFragment(editFragment);
                            } else {
                                int length = editFragment->declaration()->valueLength();
                                editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
                            }
                        });

                        QmlBindingChannel::Ptr inputChannel = ef->channel();
                        if ( inputChannel && inputChannel->listIndex() == -1 ){
                            inputChannel->property().connectNotifySignal(ef, SLOT(__updateValue()));
                        }
                        parentEdit->addChildFragment(ef);

                        rehighlightSection(ef->valuePosition(), ef->valuePosition() + ef->valueLength());

                        CodeHandler* dh = static_cast<CodeHandler*>(parent());
                        if ( dh )
                            dh->requestCursorPosition(ef->valuePosition());

                        child = ef;
                    } else {
                        child = openReadOnlyPropertyConnection(parentEdit, propName);
                    }
                }

                if ( n == 0 )
                    fragments.push_back(child);

                parentEdit = child;
            } // end of property range for
        }
    }

    if ( iterateObjects ){
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

                ef->declaration()->section()->setUserData(ef);
                ef->declaration()->section()->onTextChanged(
                            [this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText)
                {
                    auto projectDocument = section->document();
                    auto editFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

                    if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

                        int length = editFragment->declaration()->valueLength();
                        editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

                    } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
                        removeEditFragment(editFragment);
                    } else {
                        int length = editFragment->declaration()->valueLength();
                        editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
                    }
                });

                edit->addChildFragment(ef);

                fragments.append(ef);
            }
        }
    }

    return fragments;
}

void LanguageQmlHandler::removeConnection(QmlEditFragment *edit){
    m_editContainer->derefEdit(edit);
}

void LanguageQmlHandler::eraseObject(QmlEditFragment *edit, bool removeFragment){
    QList<QObject*> toRemove;

    bool objectProperty = false;
    const QmlBindingChannel::Ptr& bc = edit->channel();
    if ( bc->isEnabled() && !bc->isBuilder() ){

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

                auto parentFrag = edit->parentFragment();
                if (parentFrag){
                    for (auto cf: parentFrag->childFragments()){
                        if (!cf->channel()) continue;
                        if (cf->channel()->listIndex() <= bc->listIndex()) continue;
                        cf->channel()->updateConnection(cf->channel()->listIndex()-1);
                    }
                }
            }
        } else if ( bc->type() == QmlBindingChannel::Property ){
            objectProperty = (bc->property().propertyTypeCategory() != QQmlProperty::InvalidCategory);
            bc->property().write(QVariant::fromValue(nullptr));
        } else if ( bc->type() == QmlBindingChannel::Object ){
            bool isWatcher = false;
            QmlBindingPath::Ptr bp = m_bindingChannels->selectedChannel()->bindingPath();
            QmlBindingPath::Node* n = bp->root();
            while ( n ){
                if ( n->type() == QmlBindingPath::Node::Watcher ){
                    isWatcher = true;
                    break;
                }
                n = n->child;
            }
            if ( isWatcher ){
                QmlError(
                    m_engine,
                    CREATE_EXCEPTION(Exception, "QmlWatcher: Cannot erase an object that is part of a watcher. You need to delete the watcher first.", Exception::toCode("HasWatcher")),
                    this
                ).jsThrow();
                return;
            }

            // remove dangling pointer from binding channels
            if ( bc->object() == m_bindingChannels->selectedChannel()->object() ) {
                m_bindingChannels->selectedChannel()->clearConnection();
            }

            objectProperty = false;
            toRemove.append(bc->object());
        }
    }

    edit->writeCode(objectProperty ? "null" : "");
    edit->__updateValue();

    if (!removeFragment)
        return;

    removeEditFragment(edit);

    if ( !toRemove.isEmpty() ){
        for ( QObject* o : toRemove ){
            o->deleteLater();
        }
        m_bindingChannels->desyncInactiveChannels();
    }
}

QJSValue LanguageQmlHandler::findPalettesFromFragment(lv::QmlEditFragment* fragment){
    if (!fragment || !fragment->declaration())
        return QJSValue();

    return findPalettesForDeclaration(fragment->declaration());
}

/**
 * \brief Finds the available list of palettes at the current \p cursor position
 */
QJSValue LanguageQmlHandler::findPalettes(int position){
    if ( !m_document )
        return QJSValue();

    cancelEdit();

    QList<QmlDeclaration::Ptr> declarations = getDeclarations(position);
    if (declarations.isEmpty())
        return QJSValue();


    return findPalettesForDeclaration(declarations.first());
}

/**
 * \brief Removes a palette given its container object.
 */
lv::QmlEditFragment *LanguageQmlHandler::removePalette(lv::CodePalette *palette){
    if ( !palette )
        return nullptr;

    lv::QmlEditFragment* edit = palette->editFragment();
    if ( edit ){
        edit->removePalette(palette);
    }
    return edit;
}

QString LanguageQmlHandler::defaultPalette(QmlEditFragment *fragment){
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
CodePalette *LanguageQmlHandler::openBinding(QmlEditFragment *edit, QString paletteName){
    Q_D(LanguageQmlHandler);
    if ( !m_document || !edit )
        return nullptr;

    // check if duplicate
    PaletteLoader* paletteLoader = d->projectHandler->paletteContainer()->findPaletteByName(paletteName);
    if ( edit->bindingPalette() ){
        if ( edit->bindingPalette()->path() == PaletteContainer::palettePath(paletteLoader) )
            return edit->bindingPalette();
    }
    CodePalette* cp = edit->palette(PaletteContainer::palettePath(paletteLoader));
    if ( cp ){
        edit->setBindingPalette(cp);
        return cp;
    }

    CodePalette* palette = d->projectHandler->paletteContainer()->createPalette(paletteLoader);
    qmlEngine(this)->setObjectOwnership(palette, QQmlEngine::CppOwnership);
    palette->setEditFragment(edit);

    edit->setBindingPalette(palette);
    edit->initializePaletteValue(palette);

    connect(palette, &CodePalette::valueChanged, edit, &QmlEditFragment::__updateFromPalette);

    rehighlightSection(edit->position(), edit->valuePosition() + edit->valueLength());

    CodeHandler* dh = static_cast<CodeHandler*>(parent());
    if ( dh )
        dh->requestCursorPosition(edit->valuePosition());

    return palette;

}

/**
 * \brief Finds the boundaries of the code block containing the cursor position
 *
 * Mostly used for fragments
 */
QJSValue LanguageQmlHandler::contextBlockRange(int position){
    DocumentQmlValueScanner vs(m_document, position, 0);
    int start = vs.getBlockStart(position);
    int end   = vs.getBlockEnd(start + 1);
    QJSValue ob = m_engine->engine()->newObject();

    ob.setProperty("start", m_target->findBlock(start).blockNumber());
    ob.setProperty("end", m_target->findBlock(end).blockNumber());

    return ob;
}

lv::QmlImportsModel *LanguageQmlHandler::importsModel(){
    Q_D(LanguageQmlHandler);
    d->syncObjects(m_document);

    lv::QmlImportsModel* result = new QmlImportsModel(m_engine, this);

    DocumentQmlInfo::Ptr docinfo = d->documentInfo();
    if ( !docinfo->isParsedCorrectly() ){
        docinfo->tryExtractImports();
    }

    result->setImports(docinfo->imports());

    return result;
}

QJSValue LanguageQmlHandler::declarationInfo(int position, int length){
    if ( !m_document )
        return QJSValue();

    QList<QmlDeclaration::Ptr> properties = getDeclarations(position, length);
    if ( properties.isEmpty() )
        return QJSValue();

    auto declaration = properties.first();

    return declarationToQml(declaration);
}

/**
 * \brief Closes the bindings between the given position and length
 */
void LanguageQmlHandler::closeBinding(int position, int length){
    if ( !m_document )
        return;

    QList<QmlDeclaration::Ptr> properties = getDeclarations(position, length);
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

QJSValue LanguageQmlHandler::expand(QmlEditFragment *edit, const QJSValue &val){
    Q_D(LanguageQmlHandler);
    if ( val.hasProperty("palettes") ){
        QJSValue palettesVal = val.property("palettes");
        QJSValueIterator palettesIt(palettesVal);
        while ( palettesIt.next() ){
            QString paletteName = palettesIt.value().toString();

            PaletteLoader* paletteLoader = d->projectHandler->paletteContainer()->findPaletteByName(paletteName);
            if ( !paletteLoader )
                return QJSValue();

            if ( edit->bindingPalette() && edit->bindingPalette()->path() == PaletteContainer::palettePath(paletteLoader) ){
                edit->addPalette(edit->bindingPalette());
                return m_engine->engine()->newQObject(edit->bindingPalette());
            }

            if ( !PaletteContainer::configuresLayout(paletteLoader) ){
                CodePalette* palette = d->projectHandler->paletteContainer()->createPalette(paletteLoader);
                m_engine->engine()->setObjectOwnership(palette, QQmlEngine::CppOwnership);

                palette->setEditFragment(edit);

                edit->addPalette(palette);
                edit->initializePaletteValue(palette);

                connect(palette, &CodePalette::valueChanged, edit, &QmlEditFragment::__updateFromPalette);

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
lv::CodePalette* LanguageQmlHandler::edit(lv::QmlEditFragment *edit){
    Q_D(LanguageQmlHandler);
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
        removeEditFragment(*it);
    }
    if ( m_editFragment && m_editFragment != edit )
        removeEditFragment(m_editFragment);

    PaletteLoader* loader = d->projectHandler->paletteContainer()->findPalette("edit/qml");
    CodePalette* palette = d->projectHandler->paletteContainer()->createPalette(loader);

    edit->addPalette(palette);

    edit->declaration()->section()->onTextChanged([](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText){
        auto editFragment = reinterpret_cast<QmlEditFragment*>(section->userData());

        int length = editFragment->declaration()->valueLength();
        editFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());
    });

    palette->setEditFragment(edit);

    connect(palette, &CodePalette::valueChanged, [this, edit](){
        if ( edit->totalPalettes() > 0 ){
            CodePalette* cp = *edit->begin();
            edit->commit(cp->value());
        }
        m_document->removeEditingState(ProjectDocument::Overlay);
        removeEditFragment(edit);
    });

    m_document->addEditingState(ProjectDocument::Overlay);
    m_editFragment = edit;

    CodeHandler* dh = static_cast<CodeHandler*>(parent());
    rehighlightSection(edit->valuePosition(), edit->valuePosition() + edit->valueLength());
    if ( dh ){
        dh->requestCursorPosition(edit->valuePosition());
    }

    return palette;
}

/**
 * \brief Cancels the current editing palette
 */
void LanguageQmlHandler::cancelEdit(){
    m_document->removeEditingState(ProjectDocument::Overlay);
    if ( m_editFragment ){
        removeEditFragment(m_editFragment);
    }
}

/**
 * \brief Get the insertion options at the given \p position
 *
 * Returns an lv::QmlAddContainer for all the options
 */
QmlAddContainer *LanguageQmlHandler::getAddOptions(QJSValue value){
    if ( !value.isObject() )
        return nullptr;

    if ( value.hasProperty("position") ){
        return getAddOptionsForPosition(value.property("position").toInt());
    } else if ( value.hasProperty("editFragment") ){
        bool isReadOnly = value.hasProperty("isReadOnly") ? value.property("isReadOnly").toBool() : false;
        QmlEditFragment* ef = qobject_cast<QmlEditFragment*>(value.property("editFragment").toQObject());
        if ( !ef ){
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "getAddOptions: Cannot capture edit fragment.", lv::Exception::toCode("NullPointer"));
            m_engine->throwError(&e, this);
        }
        return getAddOptionsForFragment(ef, isReadOnly);
    }

    return nullptr;
}

/**
 * \brief Add a property given the \p addText at the specified \p position
 */
int LanguageQmlHandler::addPropertyToCode(
        int position,
        const QString &name,
        const QString& value,
        lv::QmlEditFragment* parentGroup)
{
    Q_D(LanguageQmlHandler);

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

    lv::DocumentQmlValueObjects::RangeObject* rangeObject = d->documentObjects()->objectThatWrapsPosition(position);
    if ( !rangeObject ){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "Failed to add property: " + name.toStdString() + " at position " + QString::number(position).toStdString(), lv::Exception::toCode("~Attributes")
        );
        m_engine->throwError(&e, this);
        return -1;
    }

    for ( auto it = rangeObject->properties.begin(); it != rangeObject->properties.end(); ++it ){
        lv::DocumentQmlValueObjects::RangeProperty* p = *it;
        QString propertyName = p->name().join(".");

        if ( propertyName == fullName ){ // property already exists, simply position the cursor accordingly
            lv::CodeHandler* dh = static_cast<CodeHandler*>(parent());
            if ( dh )
                dh->requestCursorPosition(p->valueBegin);
            return p->begin;
        }
    }

    // Check where to insert the property

    if ( tbStart == tbEnd ){ // inline object declaration
        insertionPosition = blockEnd;
        insertionText = "; " + fullName + ": " + value;
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

        insertionText = indent + (found ? "" : "    ")+ fullName + ": " + value + "\n";
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
    __updateScope();

    lv::CodeHandler* dh = static_cast<CodeHandler*>(parent());
    if ( dh ){
        dh->requestCursorPosition(cursorPosition);
    }

    return cursorPosition - fullName.size() - 2;
}

int LanguageQmlHandler::addEventToCode(int position, const QString &name){
    Q_D(LanguageQmlHandler);

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

    // Check whether the property is already added

    QTextCursor sourceSelection(m_target);
    sourceSelection.setPosition(blockStart);
    sourceSelection.setPosition(blockEnd, QTextCursor::KeepAnchor);

    lv::DocumentQmlValueObjects::RangeObject* rangeObject = d->documentObjects()->objectThatWrapsPosition(position);
    if ( !rangeObject ){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "Failed to add event: " + name.toStdString() + " at position " + QString::number(position).toStdString(), lv::Exception::toCode("~Attributes")
        );
        m_engine->throwError(&e, this);
        return -1;
    }

    for ( auto it = rangeObject->properties.begin(); it != rangeObject->properties.end(); ++it ){
        lv::DocumentQmlValueObjects::RangeProperty* p = *it;
        QString propertyName = p->name().join(".");

        if ( propertyName == name ){ // property already exists, simply position the cursor accordingly
            lv::CodeHandler* dh = static_cast<CodeHandler*>(parent());
            if ( dh )
                dh->requestCursorPosition(p->valueBegin);
            return p->begin;
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
    __updateScope();

    lv::CodeHandler* dh = static_cast<CodeHandler*>(parent());
    if ( dh ){
        dh->requestCursorPosition(cursorPosition);
    }

    return cursorPosition - name.size() - 2;
}

/**
 * \brief Adds an item given the \p addText at the specitied \p position
 */
int LanguageQmlHandler::addObjectToCode(int position, const QString &ctype, const QJSValue &properties){
    Q_D(LanguageQmlHandler);

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

    // Handle property insertions

    QStringList propertyDeclarations;
    if ( properties.isArray() ){
        QJSValueIterator it(properties);
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                QJSValue propertyConfig = it.value();

                if ( !propertyConfig.hasOwnProperty("name") || !propertyConfig.hasOwnProperty("type") ){
                    lv::Exception e = CREATE_EXCEPTION(
                        lv::Exception, "Property 'name' and 'type' are required.", lv::Exception::toCode("~Attributes")
                    );
                    m_engine->throwError(&e, this);
                    return -1;
                }

                QString name = propertyConfig.property("name").toString();
                QString type = propertyConfig.property("type").toString();
                QString value = "";
                if ( propertyConfig.hasOwnProperty("value") ){
                    value = propertyConfig.property("value").toString();
                } else {
                    value = QmlTypeInfo::typeDefaultValue(type);
                }

                propertyDeclarations.append("property " + type + " " + name + ": " + value);
            }
        }
    }


    QString insertionText;
    int insertionPosition = position;
    int cursorPosition = position;

    // Check where to insert the item

    if ( tbStart == tbEnd ){ // inline object declaration
        insertionPosition = blockEnd;
        insertionText = "; " + type + "{";
        if (id != "") {
            insertionText += " id: " + id + "; ";
        }
        if ( !propertyDeclarations.isEmpty() ){
            insertionText += propertyDeclarations.join("; ");
        }
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
        if (id != "") {
            insertionText += indent + "    id: " + id + "\n";
        }
        if ( !propertyDeclarations.isEmpty() ){
            insertionText += indent + "    " + propertyDeclarations.join("\n" + indent + "    ") + "\n";
        }
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

    d->documentChanged();

    __updateScope();

    lv::CodeHandler* dh = static_cast<CodeHandler*>(parent());
    if (dh){
        dh->requestCursorPosition(cursorPosition);
    }

    return cursorPosition - 1 - type.size();
}

void LanguageQmlHandler::addObjectForProperty(QmlEditFragment *propertyFragment)
{
    if (!propertyFragment)
        return;

    QString typeName = propertyFragment->typeName();
    auto block = m_target->findBlock(propertyFragment->position());
    QString indent = getBlockIndent(block);
    QString codeToWrite = typeName + "{\n" + indent + "    \n" + indent + "}\n";
    propertyFragment->writeCode(codeToWrite);
    m_scopeTimer.stop();
    __updateScope();
}

int LanguageQmlHandler::addRootObjectToCode(const QString &name)
{
    Q_D(LanguageQmlHandler);
    d->syncObjects(m_document);

    // add the root via code

    QString type; QString id;
    if (name.contains('#'))
    {
        auto spl = name.split('#');
        type = spl[0];
        id = spl[1];
    } else type = name;

    int insertionPosition = m_target->characterCount() - 1;

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
    __updateScope();

    d->syncObjects(m_document);

    QmlBindingChannel::Ptr channel = m_bindingChannels->selectedChannel();
    if ( !channel )
        return -1;

    Runnable* r = channel->runnable();
    if ( !r )
        return -1;

    QObject* newRoot = QmlEditFragment::createObject(
        d->documentInfo(), type, "temp", r->viewContext()
    );

    if ( !newRoot )
        return -1;

    r->swapViewRoot(newRoot);
    channel->updateConnection(newRoot);

    return insertionPosition + 2;
}

void LanguageQmlHandler::addItemToRuntime(QmlEditFragment *edit, const QString &ctype, const QJSValue &properties){
    try{
        addItemToRunTimeImpl(edit, ctype, properties);
    } catch ( lv::Exception& e ){
        m_engine->throwError(&e, this);
    }
}

QJSValue LanguageQmlHandler::getDocumentIds(){
    Q_D(LanguageQmlHandler);
    QmlScopeSnap scope = d->snapScope();
    QStringList ids = scope.document->extractIds();

    QJSValue result = m_engine->engine()->newArray(static_cast<quint32>(ids.length()));

    for ( int i = 0; i < ids.length(); ++i ){
        result.setProperty(static_cast<quint32>(i), ids[i]);
    }
    return result;
}

void LanguageQmlHandler::suggestCompletion(int cursorPosition){
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

int LanguageQmlHandler::checkPragma(int position)
{
    QString content = m_document->contentString();
    QString sub = content.left(position);
    int find = sub.lastIndexOf("pragma Singleton");
    if (find != -1)
        return find;
    return position;
}


QmlInheritanceInfo LanguageQmlHandler::inheritanceInfo(const QString &typeName){
    Q_D(LanguageQmlHandler);
    QmlScopeSnap snap = d->snapScope();

    return snap.getTypePath(QmlTypeReference::split(typeName));
}

QmlMetaTypeInfo *LanguageQmlHandler::typeInfo(const QString &typeName){
    QmlMetaTypeInfo* mti = new QmlMetaTypeInfo(inheritanceInfo(typeName), m_engine);
    QQmlEngine::setObjectOwnership(mti, QQmlEngine::CppOwnership);
    return mti;
}

/**
 * \brief Handler for when a new document scope is ready
 */
void LanguageQmlHandler::newDocumentScanReady(DocumentQmlInfo::Ptr documentInfo){
    Q_D(LanguageQmlHandler);
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

CodeHandler *LanguageQmlHandler::code() const{
    return qobject_cast<CodeHandler*>(parent());
}

void LanguageQmlHandler::__whenLibraryScanQueueCleared(){
    QLinkedList<QJSValue> callbacks = m_importsScannedListeners;
    m_importsScannedListeners.clear();

    for ( QJSValue cb : callbacks ){
        QJSValue res = cb.call();
        if ( res.isError() ){
            m_engine->throwError(res, this);
        }
    }
}

bool LanguageQmlHandler::areImportsScanned(){
    Q_D(LanguageQmlHandler);
    d->syncParse(m_document);
    QmlScopeSnap scope = d->snapScope();
    return scope.areDocumentLibrariesReady();
}

void LanguageQmlHandler::onDocumentParsed(QJSValue callback){
    Q_D(LanguageQmlHandler);
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

void LanguageQmlHandler::onImportsScanned(QJSValue callback){
    if ( areImportsScanned() ){
        QJSValue res = callback.call();
        if ( res.isError() ){
            m_engine->throwError(res, this);
        }
    } else {
        m_importsScannedListeners.append(callback);
    }
}

void LanguageQmlHandler::removeSyncImportsListeners(){
    m_importsScannedListeners.clear();
}

/**
 * \brief Handler for when a new project scope is ready
 */
void LanguageQmlHandler::__newProjectScopeReady(){
    m_newScope = true;
}

void LanguageQmlHandler::suggestionsForGlobalQmlContext(
        const QmlCompletionContext &,
        QList<CodeCompletionSuggestion> &suggestions
){
    suggestions << CodeCompletionSuggestion("import", "", "", "import ");
    suggestions << CodeCompletionSuggestion("pragma Singleton", "", "", "pragma Singleton");
}

void LanguageQmlHandler::suggestionsForImport(
        const QmlCompletionContext& context,
        QList<CodeCompletionSuggestion> &suggestions)
{
    foreach (const QString& importPath, m_engine->engine()->importPathList()){
        suggestionsForRecursiveImport(0, importPath, context.expressionPath(), suggestions);
    }
    std::sort(suggestions.begin(), suggestions.end(), &CodeCompletionSuggestion::compare);
}

void LanguageQmlHandler::suggestionsForStringImport(
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

void LanguageQmlHandler::suggestionsForRecursiveImport(
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

void LanguageQmlHandler::suggestionsForNamespaceTypes(
    const QString &typeNameSpace,
    QList<CodeCompletionSuggestion> &suggestions) const
{
    Q_D(const LanguageQmlHandler);
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

void LanguageQmlHandler::suggestionsForNamespaceImports(QList<CodeCompletionSuggestion> &suggestions){
    Q_D(LanguageQmlHandler);
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

void LanguageQmlHandler::suggestionsForDocumentsIds(QList<CodeCompletionSuggestion> &suggestions) const{
    Q_D(const LanguageQmlHandler);
    QStringList ids = d->documentInfo()->extractIds();
    ids.sort();
    foreach( const QString& id, ids ){
        suggestions << CodeCompletionSuggestion(id, "id", "", id);
    }
}

void LanguageQmlHandler::suggestionsForLeftBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(LanguageQmlHandler);
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

void LanguageQmlHandler::suggestionsForRightBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(LanguageQmlHandler);

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

void LanguageQmlHandler::suggestionsForLeftSignalBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion> &suggestions)
{
    Q_D(LanguageQmlHandler);
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

QString LanguageQmlHandler::extractQuotedString(const QTextCursor &cursor) const{
    QTextBlock block = cursor.block();
    int localCursorPosition = cursor.positionInBlock();
    QString blockString = block.text();

    int startOfQuote = blockString.lastIndexOf('\"', localCursorPosition - 1);
    if ( startOfQuote != -1 )
        return blockString.mid(startOfQuote + 1, localCursorPosition - startOfQuote - 1);
    else
        return blockString.mid(0, localCursorPosition);
}

QString LanguageQmlHandler::getBlockIndent(const QTextBlock &bl){
    QString blockText = bl.text();

    for ( int i = 0; i < blockText.length(); ++i ){
        if ( !blockText[i].isSpace() )
            return blockText.mid(0, i);
    }
    return blockText;
}

bool LanguageQmlHandler::isBlockEmptySpace(const QTextBlock &bl){
    QString blockText = bl.text();
    for ( auto it = blockText.begin(); it != blockText.end(); ++it )
        if ( !it->isSpace() )
            return false;

    return true;
}

QJSValue LanguageQmlHandler::findPalettesForDeclaration(QmlDeclaration::Ptr declaration)
{
    // every fragment has a declaration -> should end up here

    Q_D(LanguageQmlHandler);
    QmlScopeSnap scope = d->snapScope();

    QList<PaletteLoader*> result = d->projectHandler->paletteContainer()->findPalettes(declaration->type().join());

    if ( declaration->type().language() == QmlTypeReference::Qml ){
        QmlInheritanceInfo typePath = scope.getTypePath(declaration->type());
        for ( int i = 1; i < typePath.nodes.size(); ++i ){
            QmlTypeInfo::Ptr ti = typePath.nodes[i];
            if ( !ti->exportType().isEmpty() ){
                auto list = d->projectHandler->paletteContainer()->findPalettes(ti->exportType().join());
                result.append(list);
            }
        }
    }

    if  ( declaration->isForProperty() && declaration->parentType().language() == QmlTypeReference::Qml && !declaration->identifierChain().isEmpty() ){
        QmlInheritanceInfo typePath = scope.getTypePath(declaration->parentType());

        for ( auto it = typePath.nodes.begin(); it != typePath.nodes.end(); ++it ){
            QmlTypeInfo::Ptr ti = *it;
            QmlPropertyInfo pi = ti->propertyAt(declaration->identifierChain().last());

            if ( pi.isValid() && !ti->exportType().isEmpty() ){
                QString propSearch = ti->exportType().join() + "." + pi.name;
                auto list = d->projectHandler->paletteContainer()->findPalettes(propSearch);
                result.append(list);
            }
        }
    }


    if (declaration->isForComponent()){
        auto components = d->projectHandler->paletteContainer()->findPalettes("qml/Component");
        result.append(components);
    } else {
        if ( declaration->isForObject()){
            if (declaration->type().name()[0].isUpper() && declaration->type().language() == QmlTypeReference::Qml){
                auto object = d->projectHandler->paletteContainer()->findPalettes("qml/Object");
                result.append(object);
            }
        }

        if ( declaration->isForList() ){
            auto child = d->projectHandler->paletteContainer()->findPalettes("qml/childlist");
            result.append(child);
        } else {
            auto prop = d->projectHandler->paletteContainer()->findPalettes("qml/property");
            result.append(prop);
        }
    }

    QJSValue res = m_engine->engine()->newObject();
    QJSValue data = m_engine->engine()->newArray(result.length());
    for (int i = 0; i < result.length(); ++i){
        data.setProperty(i, d->projectHandler->paletteContainer()->paletteData(result[i]));
    }
    res.setProperty("data", data);

    QJSValue decl = declarationToQml(declaration);
    res.setProperty("declaration", decl);

    return res;
}

void LanguageQmlHandler::createChannelForFragment(QmlEditFragment *parentFragment, QmlEditFragment *fragment, QmlBindingPath::Ptr bindingPath){
    QmlBindingChannel::Ptr documentChannel = m_bindingChannels->selectedChannel();

    if ( documentChannel ){

        if ( parentFragment ){
            auto parentBp = parentFragment->fullBindingPath();

            int copyLength = bindingPath->length() - parentBp->length();
            QmlBindingPath::Ptr relativeBp = QmlBindingPath::create();

            while ( copyLength > 0){
                relativeBp = QmlBindingPath::join(bindingPath->tailPath(), relativeBp);
                bindingPath = bindingPath->headPath();
                copyLength--;
            }

            QmlBindingChannel::Ptr newChannel = DocumentQmlChannels::traverseBindingPathFrom(parentFragment->channel(), relativeBp);
            if ( !newChannel ){
                //TODO
                qWarning("Warning: Failed to get new channel at: %s from %s", qPrintable(relativeBp->toString()), qPrintable(parentBp->toString()));
            } else {
                newChannel->setEnabled(true);
                fragment->setChannel(newChannel);
            }

        } else {
            QmlBindingChannel::Ptr newChannel = DocumentQmlChannels::traverseBindingPathFrom(documentChannel, bindingPath);
            if ( !newChannel ){
                //TODO
                qWarning("Warning: Failed to get new channel from document at: %s", qPrintable(bindingPath->toString()));
            } else {
                newChannel->setEnabled(true);
                fragment->setChannel(newChannel);
            }
        }
    }

}

QJSValue LanguageQmlHandler::declarationToQml(QmlDeclaration::Ptr declaration)
{
    QJSValue result = m_engine->engine()->newObject();
    result.setProperty("position", declaration->position());
    result.setProperty("length", declaration->length());
    result.setProperty("type", declaration->type().join());
    result.setProperty("parentType", declaration->parentType().join());
    if ( declaration->isForComponent() ){
        result.setProperty("type", "component");
    } else if ( declaration->isForImports() ){
        result.setProperty("type", "imports");
    } else if ( declaration->isForList() ){
        result.setProperty("type", "list");
    } else if ( declaration->isForProperty() ){
        result.setProperty("type", "property");
    } else if ( declaration->isForSlot() ){
        result.setProperty("type", "slot");
    }
    result.setProperty("hasObject", declaration->isForObject());
    return result;
}

QmlEditFragment *LanguageQmlHandler::rootFragment() const
{
    return m_rootFragment;
}

void LanguageQmlHandler::setRootFragment(QmlEditFragment *rootFragment)
{
    if (m_rootFragment == rootFragment)
        return;

    m_rootFragment = rootFragment;
    emit rootFragmentChanged();
}

QmlEditFragment *LanguageQmlHandler::importsFragment() const
{
    return m_importsFragment;
}

void LanguageQmlHandler::setImportsFragment(QmlEditFragment *importsFragment)
{
    if (m_importsFragment == importsFragment)
        return;
    m_importsFragment = importsFragment;
    emit importsFragmentChanged();
}

}// namespace

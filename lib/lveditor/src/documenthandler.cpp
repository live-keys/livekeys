/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "live/documenthandler.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/codepalette.h"
#include "live/palettecontainer.h"
#include "live/editorglobalobject.h"
#include "live/project.h"
#include "live/viewengine.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/palettelist.h"
#include "live/palettecontainer.h"
#include "live/extensions.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextList>
#include <QTimer>
#include <QFileInfo>
#include <QJSValue>
#include <QJSValueList>

#include "textedit_p.h"
#include "textedit_p_p.h"


namespace lv{

//TODO: Add object type on code properties
//TODO: Add object type when looking for palettes

const QChar DocumentHandler::ParagraphSeparator = QChar(8233);
const QChar DocumentHandler::NewLine            = QChar('\n');

DocumentHandler::DocumentHandler(QObject *parent)
    : QObject(parent)
    , m_targetDoc(nullptr)
    , m_completionModel(new CodeCompletionModel)
    , m_codeHandler(nullptr)
    , m_projectDocument(nullptr)
    , m_indentSize(0)
    , m_project(nullptr)
    , m_engine(nullptr)
    , m_textEdit(nullptr)
    , m_fragmentStart(ProjectDocumentMarker::create())
    , m_fragmentEnd(ProjectDocumentMarker::create())
    , m_fragmentStartLine(-1)
    , m_fragmentEndLine(-1)
{
    setIndentSize(4);
}

DocumentHandler::~DocumentHandler(){
    delete m_codeHandler;
}

void DocumentHandler::setTextEdit(TextEdit *te){
    m_textEdit = te;
    if (m_targetDoc) {
        te->setTextDocument(m_targetDoc);
    }
}

void DocumentHandler::requestCursorPosition(int position){
    emit cursorPositionRequest(position);
}

void DocumentHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_codeHandler )
        m_codeHandler->rehighlightBlock(block);
}

void DocumentHandler::componentComplete(){
    QQmlContext* ctx = qmlEngine(this)->rootContext();
    EditorGlobalObject* editor = static_cast<EditorGlobalObject*>(ctx->contextProperty("editor").value<QObject*>());
    if ( !editor ){
        qWarning("Failed to find editor global object.");
        return;
    }

    m_project = editor->project();

    QObject* lg = ctx->contextProperty("livecv").value<QObject*>();
    if ( !lg ){
        qWarning("Failed to find live global object.");
        return;
    }

    m_engine     = static_cast<ViewEngine*>(lg->property("engine").value<lv::ViewEngine*>());
    m_extensions = static_cast<Extensions*>(lg->property("extensions").value<QQmlPropertyMap*>()->parent());

    findCodeHandler();
}


void DocumentHandler::readContent(){
    m_targetDoc->setPlainText(m_projectDocument->content());
}

void DocumentHandler::updateFragments(){
    if ( m_fragmentStartLine != -1 && m_fragmentEndLine > 0 && m_targetDoc ){
        m_fragmentStart = m_projectDocument->addMarker(
            m_targetDoc->findBlockByLineNumber(m_fragmentStartLine).position()
        );
        m_fragmentEnd   = m_projectDocument->addMarker(
            m_targetDoc->findBlockByLineNumber(m_fragmentEndLine).position()
        );
    }
}

void DocumentHandler::findCodeHandler(){

    if ( m_project && m_engine && m_projectDocument ){
        vlog("editor-documenthandler").v() << "Looking up language handler for: " << m_projectDocument->file()->path();

        QString fileExtension = QFileInfo(m_projectDocument->file()->path()).suffix();

        QJSValueList interceptorArgs;
        interceptorArgs << m_engine->engine()->newQObject(m_projectDocument);
        interceptorArgs << m_engine->engine()->newQObject(this);
        interceptorArgs << fileExtension;
        QQmlEngine::setObjectOwnership(m_projectDocument, QQmlEngine::CppOwnership);
        QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

        for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
            LiveExtension* le = it.value();
            if ( le->hasLanguageInterceptor() ){
                QObject* o = le->callLanguageInterceptor(interceptorArgs);

                AbstractCodeHandler* ach = qobject_cast<AbstractCodeHandler*>(o);
                if ( ach ){
                    vlog("editor-documenthandler").v() << "Found in extension: " << le->name();

                    QQmlEngine::setObjectOwnership(ach, QQmlEngine::CppOwnership);
                    m_codeHandler = ach;
                    m_codeHandler->setDocument(m_projectDocument);

                    emit codeHandlerChanged();

                    return;
                }
            }
        }
    }
}

void DocumentHandler::rehighlightSection(int position, int length){
    if ( !m_codeHandler )
        return;

    QTextBlock bl = m_targetDoc->findBlock(position);
    int end = position + length;
    while ( bl.isValid() ){
        m_codeHandler->rehighlightBlock(bl);
        if (bl.position() > end )
            break;
        bl = bl.next();
    }
}

void DocumentHandler::insertCompletion(int from, int to, const QString &completion){
    if ( m_targetDoc ){
        m_projectDocument->addEditingState(ProjectDocument::Assisted);
        QTextCursor cursor(m_targetDoc);
        cursor.beginEditBlock();
        cursor.setPosition(from);
        cursor.setPosition(to, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.insertText(completion);
        cursor.endEditBlock();
        m_completionModel->disable();
        m_projectDocument->removeEditingState(ProjectDocument::Assisted);
    }
}

void DocumentHandler::documentContentsChanged(int position, int charsRemoved, int charsAdded){
    AbstractCodeHandler::ContentsTrigger cst = AbstractCodeHandler::Engine;
    if ( m_codeHandler )
         cst = m_codeHandler->documentContentsChanged(position, charsRemoved, charsAdded);

    if ( !m_projectDocument || m_projectDocument->editingStateIs(ProjectDocument::Read) )
        return;

    m_lastChar = QChar();
    if ( !m_projectDocument->editingStateIs(ProjectDocument::Silent) && m_editorFocus ){
        if ( charsAdded == 1 )
            m_lastChar = m_targetDoc->characterAt(position);

        if ( cst == AbstractCodeHandler::Engine )
            emit contentsChangedManually();
    }
}

void DocumentHandler::cursorWritePositionChanged(QTextCursor cursor){
    if ( m_codeHandler && m_editorFocus && cursor.position() == m_textEdit->cursorPosition() &&
         !m_projectDocument->editingStateIs(ProjectDocument::Assisted) &&
         !m_projectDocument->editingStateIs(ProjectDocument::Silent)
        )
    {
        m_projectDocument->addEditingState(ProjectDocument::Assisted);
        QTextCursor newCursor;
        m_codeHandler->assistCompletion(
            cursor,
            m_lastChar,
            false,
            m_completionModel,
            newCursor
        );
        m_projectDocument->removeEditingState(ProjectDocument::Assisted);
        if ( !newCursor.isNull() ){
            emit cursorPositionRequest(newCursor.position());
        }
    }
}

void DocumentHandler::setDocument(ProjectDocument *document, QJSValue options){
    if ( m_projectDocument ){
        disconnect(m_projectDocument, SIGNAL(contentChanged()), this, SLOT(documentUpdatedContent()));
    }

    m_projectDocument = document;
    if ( document )
        connect(m_projectDocument, SIGNAL(contentChanged()), this, SLOT(documentUpdatedContent()));

    m_fragmentStartLine = -1;
    m_fragmentEndLine   = -1;

    if ( m_codeHandler ){
        delete m_codeHandler;
        m_codeHandler = nullptr;
    }

    if (m_projectDocument) {
        m_targetDoc = m_projectDocument->textDocument();

        connect(
            m_targetDoc, SIGNAL(cursorPositionChanged(QTextCursor)),
            this, SLOT(cursorWritePositionChanged(QTextCursor))
        );

        if (m_textEdit) {
            m_textEdit->setTextDocument(m_targetDoc);
        }

        updateFragments();

        connect(
            m_targetDoc, SIGNAL(contentsChange(int,int,int)),
            this, SLOT(documentContentsChanged(int,int,int))
        );
    }
    else
    {
        if (m_textEdit)
        {
            m_textEdit->setTextDocument(nullptr);
        }
    }

    emit targetChanged();

    findCodeHandler();

    if ( m_targetDoc )
        m_targetDoc->clearUndoRedoStacks();

    if ( options.isObject() ){
        if ( options.hasOwnProperty("fragmentStartLine") && options.hasOwnProperty("fragmentEndLine") ){
            m_fragmentStartLine = options.property("fragmentStartLine").toInt();
            m_fragmentEndLine   = options.property("fragmentEndLine").toInt();
            updateFragments();
        }
    }
}

void DocumentHandler::documentUpdatedContent(){
    if ( m_fragmentEndLine > 0 ){
        if ( m_fragmentStart->isValid() && m_fragmentEnd->isValid() && m_targetDoc ){
            int startLine = m_targetDoc->findBlock(m_fragmentStart->position()).firstLineNumber();
            int endLine = m_targetDoc->findBlock(m_fragmentEnd->position()).firstLineNumber();

            if ( startLine != m_fragmentStartLine || endLine != m_fragmentEndLine ){
                m_fragmentStartLine = startLine;
                m_fragmentEndLine = endLine;
                emit fragmentLinesChanged(m_fragmentStartLine, m_fragmentEndLine);
            }
        } else {
            if ( m_fragmentEndLine != 0 ){
                m_fragmentEndLine = 0;
                m_fragmentStartLine = 0;
                emit fragmentLinesChanged(0, 0);
            }
        }
    }
}

void DocumentHandler::generateCompletion(int cursorPosition){
    if ( m_targetDoc && m_codeHandler ){
        m_lastChar = QChar();
        QTextCursor cursor(m_targetDoc);
        cursor.setPosition(cursorPosition);
        QTextCursor newCursor;
        m_codeHandler->assistCompletion(
            cursor,
            m_lastChar,
            true,
            m_completionModel,
            newCursor
        );
    }
}

void DocumentHandler::manageIndent(int from, int length, bool undo){
    QTextBlock bl = m_targetDoc->findBlock(from);
    while ( bl.isValid() ){
        QTextCursor cs(bl);
        cs.beginEditBlock();
        if ( undo ){
            if ( bl.text().startsWith(m_indentContent) ){
                cs.setPosition(cs.position() + m_indentSize, QTextCursor::KeepAnchor);
                cs.removeSelectedText();
                if ( from >= m_indentSize )
                    from -= m_indentSize;
                else {
                    from = 0;
                    length = length >= m_indentSize ? length - m_indentSize : 0;
                }
            }
        } else {
            cs.insertText(m_indentContent);
            if ( cs.position() > from )
                length += m_indentSize;
            else
                from += m_indentSize;
        }
        cs.endEditBlock();

        if ( bl.position() + bl.length() >= from + length)
            return;

        bl = bl.next();
    }
}

QJSValue DocumentHandler::contextBlockRange(int cursorPosition){
    if ( !m_codeHandler || !m_engine )
        return QJSValue();

    QPair<int, int> v = m_codeHandler->contextBlock(cursorPosition);
    QJSValue ob = m_engine->engine()->newObject();
    ob.setProperty("start", m_targetDoc->findBlock(v.first).blockNumber());
    ob.setProperty("end", m_targetDoc->findBlock(v.second).blockNumber());
    return ob;
}

}// namespace

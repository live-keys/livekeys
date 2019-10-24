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
#include <QMetaType>
#include <QJSValueList>

#include "textedit_p.h"
#include "textedit_p_p.h"

/**
 * \class lv::DocumentHandler
 * \brief Complements TextEdit in handling documents.
 *
 * Offers infrastructure for code completion, highlighting and other language specifics.
 *
 * \ingroup lveditor
 */
namespace lv{

const QChar DocumentHandler::ParagraphSeparator = QChar(8233);
const QChar DocumentHandler::NewLine            = QChar('\n');

/**
 * \brief Default constructor of the DocumentHandler
 *
 * Initialized from Editor.qml
 */
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
{
    setIndentSize(4);
}

/**
 * \brief Default destructor of DocumentHandler
 */
DocumentHandler::~DocumentHandler(){
    delete m_codeHandler;
}

/** Set the target doc */
void DocumentHandler::setTarget(QTextDocument *target){
    m_targetDoc = target;
}

/**
 * \brief TextEdit setter
 *
 * Usually from Editor.qml
 */
void DocumentHandler::setTextEdit(TextEdit *te){
    m_textEdit = te;
    if (m_targetDoc) {
        te->setTextDocument(m_targetDoc);
    }
}

int DocumentHandler::currentCursorPosition() const{
    return m_textEdit->cursorPosition();
}

const QChar &DocumentHandler::lastAddedChar() const{
    return m_lastChar;
}


/**
 * \fn lv::DocumentHandler::classBegin
 * \brief Implementation of the respective function from QQmlParserStatus
 */

/**
 * \brief Cursor position request signal emitter
 *
 */
void DocumentHandler::requestCursorPosition(int position){
    emit cursorPositionRequest(position);
}

/**
 * \brief Used to add and position the palette inside the editor
 */
void DocumentHandler::lineBoxAdded(int lineStart, int lineEnd, int height, QQuickItem *box){
    m_textEdit->linePaletteAdded(lineStart, lineEnd, height, box);
}

/**
 * \brief Used to remove a specific palette in the editor
 */
void DocumentHandler::lineBoxRemoved(QQuickItem *palette)
{
    m_textEdit->linePaletteRemoved(palette);
}

/**
 * \brief Used to resize a given palette
 */
void DocumentHandler::lineBoxResized(QQuickItem *palette, int newHeight)
{
    m_textEdit->linePaletteHeightChanged(palette, newHeight);
}


/**
 * \brief Implementation of the respective function from QQmlParserStatus
 */
void DocumentHandler::componentComplete(){
    QQmlContext* ctx = qmlEngine(this)->rootContext();
    EditorGlobalObject* editor = static_cast<EditorGlobalObject*>(ctx->contextProperty("editor").value<QObject*>());
    if ( !editor ){
        qWarning("Failed to find editor global object.");
        return;
    }

    m_project = editor->project();

    QObject* lg = ctx->contextProperty("lk").value<QObject*>();
    if ( !lg ){
        qWarning("Failed to find live global object.");
        return;
    }

    m_engine     = static_cast<ViewEngine*>(lg->property("engine").value<lv::ViewEngine*>());

    QObject* workspace = lg->property("layers").value<QQmlPropertyMap*>()->property("workspace").value<QObject*>();

    m_extensions = static_cast<Extensions*>(workspace->property("extensions").value<QQmlPropertyMap*>()->parent());

    findCodeHandler();
}

void DocumentHandler::readContent(){
    m_targetDoc->setPlainText(m_projectDocument->content());
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
            WorkspaceExtension* le = it.value();
            if ( le->hasLanguageInterceptor() ){
                QObject* o = le->callLanguageInterceptor(interceptorArgs);
                if ( o ){
                    vlog("editor-documenthandler").v() << "Found in extension: " << le->name();

                    QQmlEngine::setObjectOwnership(o, QQmlEngine::CppOwnership);
                    m_codeHandler = o;

                    QList<int> features;
                    QMetaObject::invokeMethod(
                        o, "languageFeatures", Qt::DirectConnection, Q_RETURN_ARG(QList<int>, features)
                    );

                    for ( int feature : features ){
                        m_languageFeatures.insert(feature);
                    }

                    emit codeHandlerChanged();

                    return;
                }
            }
        }
    }
}

/**
 * \brief Adds specific completion that the user picked between given positions
 */
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

/**
 * \brief Slot that is connected to document changes
 */
void DocumentHandler::documentContentsChanged(int position, int, int charsAdded){
    if ( !m_projectDocument || m_projectDocument->editingStateIs(ProjectDocument::Read) )
        return;

    m_lastChar = QChar();
    if ( !m_projectDocument->editingStateIs(ProjectDocument::Silent) && m_editorFocus ){
        if ( charsAdded == 1 )
            m_lastChar = m_targetDoc->characterAt(position);

        if ( !m_projectDocument->editingStateIs(ProjectDocument::Overlay) )
            emit contentsChangedManually();
    }
}

/**
 * \brief Document that the document handler is operating on
 *
 * It's a pre-requisite to set the document in order to have any functionality
 */
void DocumentHandler::setDocument(ProjectDocument *document, QJSValue){


    if (m_projectDocument && m_textEdit)
    {
        m_projectDocument->setLastCursorPosition(m_textEdit->cursorPosition());
    }

    m_projectDocument = document;

    if ( m_codeHandler ){
        delete m_codeHandler;
        m_codeHandler = nullptr;
    }

    if (m_projectDocument) {
        m_targetDoc = m_projectDocument->textDocument();

        if (m_textEdit) {
            m_textEdit->setTextDocument(m_targetDoc);
            if (m_projectDocument->lastCursorPosition() != -1)
            {
                m_textEdit->setCursorPosition(m_projectDocument->lastCursorPosition());
            }
        }

        connect(
            m_targetDoc, SIGNAL(contentsChange(int,int,int)),
            this, SLOT(documentContentsChanged(int,int,int))
        );
    } else {
        if (m_textEdit){
            m_textEdit->setTextDocument(nullptr);
        }
    }

    emit targetChanged();

    m_languageFeatures.clear();

    findCodeHandler();
}

/**
 * \brief Used to manage indentation of selected text
 *
 * When a user presses the Tab button, the whole selected text should move
 */
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

void DocumentHandler::insertTab(int position)
{
    QTextBlock bl = m_targetDoc->findBlock(position);
    QTextCursor cs(bl);
    cs.beginEditBlock();
    cs.setPosition(position);
    cs.insertText("    ");
    cs.endEditBlock();

}

void DocumentHandler::handleClosingBrace(int position)
{
    if (!m_textEdit || !m_targetDoc || position < 4) return;

    QTextCursor cursor(m_targetDoc);
    cursor.beginEditBlock();

    if (m_textEdit->text().mid(position - 4, 4) == "    ")
    {
        cursor.setPosition(position-4);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 4);
        cursor.removeSelectedText();
    } else {
        cursor.setPosition(position);
    }

    cursor.insertText("}");
    cursor.endEditBlock();

}

bool DocumentHandler::has(int feature){
    return m_languageFeatures.contains(feature);
}

}// namespace

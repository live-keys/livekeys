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

#include "live/codehandler.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/codepalette.h"
#include "live/paletteloader.h"
#include "live/editorglobalobject.h"
#include "live/project.h"
#include "live/viewengine.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/paletteloader.h"
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

const QChar CodeHandler::ParagraphSeparator = QChar(8233);
const QChar CodeHandler::NewLine            = QChar('\n');

/**
 * \brief Default constructor of the DocumentHandler
 *
 * Initialized from Editor.qml
 */
CodeHandler::CodeHandler(QObject *parent)
    : QObject(parent)
    , m_completionModel(new CodeCompletionModel)
    , m_language(nullptr)
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
CodeHandler::~CodeHandler(){
    delete m_language;
}

/**
 * \brief TextEdit setter
 *
 * Usually from Editor.qml
 */
void CodeHandler::setTextEdit(TextEdit *te){
    m_textEdit = te;
    if (m_projectDocument) {
        te->setTextDocument(textDocument());
    }
}

int CodeHandler::currentCursorPosition() const{
    return m_textEdit->cursorPosition();
}

const QChar &CodeHandler::lastAddedChar() const{
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
void CodeHandler::requestCursorPosition(int position){
    emit cursorPositionRequest(position);
}

/**
 * \brief Used to add and position the palette inside the editor
 */
void CodeHandler::lineBoxAdded(int lineStart, int lineEnd, int height, QQuickItem *box, int start, int end){
    m_textEdit->linePaletteAdded(lineStart, lineEnd, height, box, start, end);
}

///**
// * \brief Used to remove a specific palette in the editor
// */
//void DocumentHandler::lineBoxRemoved(QQuickItem *palette)
//{
//    m_textEdit->linePaletteRemoved(palette);
//}

///**
// * \brief Used to resize a given palette
// */
//void DocumentHandler::lineBoxResized(QQuickItem *palette, int newHeight)
//{
//    m_textEdit->linePaletteHeightChanged(palette, newHeight);
//}


/**
 * \brief Implementation of the respective function from QQmlParserStatus
 */
void CodeHandler::componentComplete(){
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

    if (workspace){
        m_extensions = static_cast<Extensions*>(workspace->property("extensions").value<QQmlPropertyMap*>()->parent());
        findCodeHandler();
    }
}

void CodeHandler::findCodeHandler(){
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
                    m_language = o;

                    QList<int> features;
                    QMetaObject::invokeMethod(
                        o, "languageFeatures", Qt::DirectConnection, Q_RETURN_ARG(QList<int>, features)
                    );

                    for ( int feature : features ){
                        m_languageFeatures.insert(feature);
                    }

                    emit languageChanged();

                    return;
                }
            }
        }
    }
}

/**
 * \brief Adds specific completion that the user picked between given positions
 */
void CodeHandler::insertCompletion(int from, int to, const QString &completion){
    if ( textDocument() ){
        m_projectDocument->addEditingState(ProjectDocument::Assisted);
        QTextCursor cursor(textDocument());
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
void CodeHandler::documentContentsChanged(int position, int, int charsAdded){
    if ( !m_projectDocument || m_projectDocument->editingStateIs(ProjectDocument::Read) )
        return;

    m_lastChar = QChar();
    if ( !m_projectDocument->editingStateIs(ProjectDocument::Silent) && m_editorFocus ){
        if ( charsAdded == 1 )
            m_lastChar = textDocument()->characterAt(position);

        if ( !m_projectDocument->editingStateIs(ProjectDocument::Overlay) )
            emit contentsChangedManually();
    }
}

/**
 * \brief Document that the document handler is operating on
 *
 * It's a pre-requisite to set the document in order to have any functionality
 */
void CodeHandler::setDocument(ProjectDocument *document, QJSValue){

    if (m_projectDocument && m_textEdit)
    {
        m_projectDocument->setLastCursorPosition(m_textEdit->cursorPosition());
    }

    if ( m_language ){
        emit aboutToDeleteHandler();
        delete m_language;
        m_language = nullptr;
    }

    m_projectDocument = document;

    if (m_projectDocument) {
        if (m_textEdit) {
            m_textEdit->setTextDocument(textDocument());
            if (m_projectDocument->lastCursorPosition() != -1)
            {
                m_textEdit->setCursorPosition(m_projectDocument->lastCursorPosition());
            }
        }

        connect(
            textDocument(), SIGNAL(contentsChange(int,int,int)),
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
void CodeHandler::manageIndent(int from, int length, bool undo){
    QTextBlock bl = textDocument()->findBlock(from);
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

void CodeHandler::insertTab(int position)
{
    QTextBlock bl = textDocument()->findBlock(position);
    QTextCursor cs(bl);
    cs.beginEditBlock();
    cs.setPosition(position);
    cs.insertText("    ");
    cs.endEditBlock();

}

void CodeHandler::frameBox(QQuickItem *box, int position, int length){
    QTextBlock tb = m_projectDocument->textDocument()->findBlock(position);
    QTextBlock tbend = m_projectDocument->textDocument()->findBlock(position + length);

    lineBoxAdded(tb.blockNumber() + 1, tbend.blockNumber() + 1, static_cast<int>(box->height()), box);
}


bool CodeHandler::has(int feature){
    return m_languageFeatures.contains(feature);
}

}// namespace

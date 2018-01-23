/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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
#include "live/livepalette.h"
#include "live/livepalettecontainer.h"
#include "live/documenteditfragment.h"
#include "live/documenthandlerstate.h"
#include "live/codedeclaration.h"
#include "live/coderuntimebinding.h"
#include "live/editorglobalobject.h"
#include "live/project.h"
#include "live/engine.h"
#include "live/visuallog.h"
#include "live/projectextension.h"

#include <QQuickTextDocument>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextCursor>
#include <QTextBlock>
#include <QTimer>

#include <QTextList>

namespace lv{

//TODO: Add object type on code properties
//TODO: Add object type when looking for palettes

const QChar DocumentHandler::ParagraphSeparator = QChar(8233);
const QChar DocumentHandler::NewLine            = QChar('\n');

DocumentHandler::DocumentHandler(QObject *parent)
    : QObject(parent)
    , m_target(0)
    , m_targetDoc(0)
    , m_completionModel(new CodeCompletionModel)
    , m_codeHandler(0)
    , m_projectDocument(0)
    , m_editingState(0)
    , m_indentSize(0)
    , m_paletteContainer(0)
    , m_project(0)
    , m_engine(0)
    , m_fragmentStart(ProjectDocumentMarker::create())
    , m_fragmentEnd(ProjectDocumentMarker::create())
    , m_fragmentStartLine(-1)
    , m_fragmentEndLine(-1)
    , m_state(new DocumentHandlerState)
{
    setIndentSize(4);
    m_timer.setInterval(1000);
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateScope()));
}

DocumentHandler::~DocumentHandler(){
    if ( m_projectDocument )
        m_projectDocument->assignEditingDocument(0, 0);
    delete m_codeHandler;
}

void DocumentHandler::setTarget(QQuickTextDocument *target){
    if ( m_target != target ){
        m_target    = target;
        m_targetDoc = 0;
        if ( target ){
            m_targetDoc = target->textDocument();
            if ( m_targetDoc ){
                connect(
                    m_targetDoc, SIGNAL(cursorPositionChanged(QTextCursor)),
                    this, SLOT(cursorWritePositionChanged(QTextCursor))
                );
                connect(
                    m_targetDoc, SIGNAL(contentsChange(int,int,int)),
                    this, SLOT(documentContentsChanged(int,int,int))
                );
                if ( m_projectDocument ){
                    addEditingState(DocumentHandler::Read);
                    m_targetDoc->setPlainText(m_projectDocument->content());
                    m_projectDocument->assignEditingDocument(m_targetDoc, this);
                    removeEditingState(DocumentHandler::Read);
                    updateFragments();
                }
            }
        }
        emit targetChanged();
    }
}

LivePalette *DocumentHandler::palette(){
    if ( !m_state->editingFragment() )
        return 0;
    return qobject_cast<LivePalette*>(m_state->editingFragment()->converter());
}

void DocumentHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_codeHandler )
        m_codeHandler->rehighlightBlock(block);
}

void DocumentHandler::componentComplete(){
    if ( m_target == 0 ){
        qWarning("Target has not been set for document handler. Highlighting and code completion will be disabled.");
    }

    QQmlContext* ctx = qmlEngine(this)->rootContext();
    EditorGlobalObject* editor = static_cast<EditorGlobalObject*>(ctx->contextProperty("editor").value<QObject*>());
    if ( !editor ){
        qWarning("Failed to find editor global object.");
        return;
    }

    m_paletteContainer = editor->paletteContainer();
    m_project = editor->project();

    QObject* lg = ctx->contextProperty("livecv").value<QObject*>();
    if ( !lg ){
        qWarning("Failed to find live global object.");
        return;
    }

    m_engine = static_cast<Engine*>(lg->property("engine").value<lv::Engine*>());

    findCodeHandler();
}

void DocumentHandler::commitEdit(){
    if ( m_state->editingFragment() ){
        int position = m_state->editingFragment()->valuePosition();
        int length   = m_state->editingFragment()->valueLength();

        CodeConverter* converter = m_state->editingFragment()->converter();
        if ( converter ){
            QTextCursor tc(m_targetDoc);
            tc.setPosition(position);
            tc.setPosition(position + length, QTextCursor::KeepAnchor);
            QString commitText = tc.selectedText();
            m_state->editingFragment()->commit(
                converter->serialize()->fromCode(commitText, m_state->editingFragment())
            );
            vlog_debug("editor-documenthandler", "Commited edit of size: " + QString::number(commitText.size()));

            if ( m_state->editingFragment()->actionType() == DocumentEditFragment::Adjust ){
                palette()->setValueFromCode(converter->serialize()->fromCode(commitText, m_state->editingFragment()));
                return;
            }

        } else {
            emit contentsChangedManually();
            m_timer.start();
        }

        if ( palette() )
            disconnect(palette(), SIGNAL(valueChanged()), this, SLOT(paletteValueChanged()));
        m_state->clearEditingFragment();
        rehighlightSection(position, length);
        emit paletteChanged();
        resetEditingState();
    }
}

void DocumentHandler::cancelEdit(){
    if ( m_state->editingFragment() ){
        int position = m_state->editingFragment()->valuePosition();
        int length   = m_state->editingFragment()->valueLength();
        if ( palette() )
            disconnect(palette(), SIGNAL(valueChanged()), this, SLOT(paletteValueChanged()));
        m_state->clearEditingFragment();
        emit paletteChanged();
        rehighlightSection(position, length);
        resetEditingState();
    }
}

bool DocumentHandler::isEditing(){
    return (m_state->editingFragment() != 0);
}

void DocumentHandler::paletteValueChanged(){
    QString code = palette()->serialize()->toCode(palette()->value(), m_state->editingFragment());

    if ( code != "" ){
        addEditingState(DocumentHandler::Palette);

        DocumentEditFragment* frg = m_state->editingFragment();
        QTextCursor tc(m_targetDoc);
        tc.setPosition(frg->valuePosition());
        tc.setPosition(frg->valuePosition() + frg->valueLength(), QTextCursor::KeepAnchor);
        tc.beginEditBlock();
        tc.removeSelectedText();
        tc.insertText(code);
        tc.endEditBlock();

        removeEditingState(DocumentHandler::Palette);
    }
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
        for ( auto it = m_project->extensions().begin(); it != m_project->extensions().end(); ++it ){
            lv::ProjectExtension* ext = *it;
            m_codeHandler = ext->createHandler(m_projectDocument, m_project, m_engine, this);
            if ( m_codeHandler ){
                m_codeHandler->setTarget(m_targetDoc, m_state);
                m_codeHandler->setDocument(m_projectDocument);
                return;
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
        addEditingState(DocumentHandler::Assisted);
        QTextCursor cursor(m_targetDoc);
        cursor.beginEditBlock();
        cursor.setPosition(from);
        cursor.setPosition(to, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.insertText(completion);
        cursor.endEditBlock();
        m_completionModel->disable();
        removeEditingState(DocumentHandler::Assisted);
    }
}

void DocumentHandler::documentContentsChanged(int position, int charsRemoved, int charsAdded){
    if ( !m_targetDoc )
        return;
    if ( editingStateIs(DocumentHandler::Read) )
        return;

    m_lastChar = QChar();

    QString addedText = "";
    if ( charsAdded == 1 ){
        QChar c = m_targetDoc->characterAt(position);
        if ( c == DocumentHandler::ParagraphSeparator )
            c = DocumentHandler::NewLine;
        addedText = c;
    } else if ( charsAdded > 0 ){
        QTextCursor cursor(m_targetDoc);
        cursor.setPosition(position);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
        addedText = cursor.selection().toPlainText();
    }

    if ( !editingStateIs(DocumentHandler::Silent) ){
        if ( charsAdded == 1 )
            m_lastChar = m_targetDoc->characterAt(position);

        if ( m_projectDocument )
            m_projectDocument->documentContentsChanged(this, position, charsRemoved, addedText);

        emit contentsChangedManually();
        m_timer.start();

    } else if ( m_projectDocument ){
        DocumentEditFragment* frg = m_state->editingFragment();
        if ( frg ){
            if ( position >= frg->valuePosition() && position <= frg->valuePosition() + frg->valueLength() ){
                frg->declaration()->setValueLength(frg->valueLength() - charsRemoved + charsAdded);
                if ( frg->actionType() == DocumentEditFragment::Adjust ){
                    if ( editingStateIs(DocumentHandler::Palette) ){ // palette edit
                        CodeRuntimeBinding* binding = frg->runtimeBinding();
                        if ( binding ){
                            vlog_debug("editor-documenthandler", "Breaking binding at: " + QString::number(binding->position()));
                            frg->setRuntimeBinding(0);
                            if (m_projectDocument->removeBindingAt(binding->position()) )
                                m_codeHandler->rehighlightBlock(m_targetDoc->findBlock(frg->declaration()->position()));
                        }
                        m_state->editingFragment()->commit(palette()->value());
                    } else if ( editingStateIs(DocumentHandler::Runtime )){ // runtime edit
                        QTextCursor tc(m_targetDoc);
                        tc.setPosition(frg->valuePosition());
                        tc.setPosition(frg->valuePosition() + frg->valueLength(), QTextCursor::KeepAnchor);
                        palette()->setValueFromCode(palette()->serialize()->fromCode(tc.selectedText(), frg));
                    } else { // text edit
                        CodeRuntimeBinding* binding = frg->runtimeBinding();
                        if ( binding ){
                            vlog_debug("editor-documenthandler", "Breaking binding at: " + QString::number(binding->position()));
                            frg->setRuntimeBinding(0);
                            if (m_projectDocument->removeBindingAt(binding->position()) )
                                m_codeHandler->rehighlightBlock(m_targetDoc->findBlock(frg->declaration()->position()));
                        }
                    }
                }

            } else {
                cancelEdit();
            }
        }
        m_projectDocument->documentContentsSilentChanged(this, position, charsRemoved, addedText);
    }
}

void DocumentHandler::cursorWritePositionChanged(QTextCursor cursor){
    if ( m_codeHandler &&
         !editingStateIs(DocumentHandler::Assisted) &&
         !editingStateIs(DocumentHandler::Silent)
        )
    {
        addEditingState(DocumentHandler::Assisted);
        QTextCursor newCursor;
        m_codeHandler->assistCompletion(
            cursor,
            m_lastChar,
            false,
            m_completionModel,
            newCursor
        );
        removeEditingState(DocumentHandler::Assisted);
        if ( !newCursor.isNull() ){
            emit cursorPositionRequest(newCursor.position());
        }
    }
}

void DocumentHandler::setDocument(ProjectDocument *document, QJSValue options){
    cancelEdit();
    if ( m_projectDocument ){
        m_projectDocument->assignEditingDocument(0, 0);
        disconnect(m_projectDocument, SIGNAL(contentChanged(QObject*)), this, SLOT(documentUpdatedContent(QObject*)));
    }

    m_projectDocument = document;
    if ( document )
        connect(m_projectDocument, SIGNAL(contentChanged(QObject*)), this, SLOT(documentUpdatedContent(QObject*)));

    m_fragmentStartLine = -1;
    m_fragmentEndLine   = -1;

    if ( m_codeHandler ){
        delete m_codeHandler;
        m_codeHandler = 0;
    }
    findCodeHandler();

    if ( m_projectDocument && m_targetDoc ){
        addEditingState(DocumentHandler::Read);
        m_targetDoc->setPlainText(m_projectDocument->content());
        m_projectDocument->assignEditingDocument(m_targetDoc, this);
        removeEditingState(DocumentHandler::Read);
    }
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

void DocumentHandler::documentUpdatedContent(QObject *author){
    if ( author != this ){
        addEditingState(DocumentHandler::Read);
        m_targetDoc->setPlainText(m_projectDocument->content());
        removeEditingState(DocumentHandler::Read);
    }

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
    if ( m_target && m_codeHandler ){
        m_lastChar = QChar();
        QTextCursor cursor(m_target->textDocument());
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

void DocumentHandler::updateScope(){
    if ( m_codeHandler && m_projectDocument )
        m_codeHandler->updateScope(m_projectDocument->content());
}


void DocumentHandler::bind(int position, int length, QObject *object){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);

    QList<CodeRuntimeBinding*> addedBindings;
    for ( QList<CodeDeclaration::Ptr>::iterator it = properties.begin(); it != properties.end(); ++it ){
        CodeDeclaration::Ptr cde = *it;
        //is there converter available for binding
        CodeConverter* cvt = m_paletteContainer->findPalette(cde->type(), cde->parentType());
        if ( cvt ){

            CodeRuntimeBinding* addedBinding = m_projectDocument->addNewBinding(cde);
            if (addedBinding){
                addedBinding->setConverter(cvt);
                addedBindings.append(addedBinding);
                if ( m_state->editingFragment() && m_state->editingFragment()->declaration()->position() == addedBinding->position()){
                    m_state->editingFragment()->setRuntimeBinding(addedBinding);
                    vlog_debug("editor-documenthandler", "Linking binding to editing fragment: " + QString::number(m_state->editingFragment()->declaration()->position()));
                }
            }
        }
    }

    // bind runtime bindings to engine and rehighlight
    if ( m_targetDoc ){
        if ( object )
            m_codeHandler->connectBindings(addedBindings, object);

        for ( QList<CodeRuntimeBinding*>::iterator it = addedBindings.begin(); it != addedBindings.end(); ++it ){
            m_codeHandler->rehighlightBlock(m_targetDoc->findBlock((*it)->position()));
        }
    }

}


void DocumentHandler::unbind(int position, int length){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);
    for ( QList<CodeDeclaration::Ptr>::iterator it = properties.begin(); it != properties.end(); ++it ){
        if ( m_projectDocument->removeBindingAt((*it)->position()) )
            m_codeHandler->rehighlightBlock(m_targetDoc->findBlock((*it)->position()));
    }
}

void DocumentHandler::edit(int position, QObject *currentApp){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    cancelEdit();

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);
    if ( properties.isEmpty() )
        return;

    CodeDeclaration::Ptr declaration = properties.first();

    CodeConverter* converter = m_paletteContainer->findPalette(
        declaration->type(), declaration->parentType().isEmpty() ? "" : declaration->parentType().first()
    );
    if ( converter ){
        vlog_debug("editor-documenthandler", "Found Converter for type: \'" + declaration->type()+ "\'");
        DocumentEditFragment* ef = m_codeHandler->createInjectionChannel(declaration, currentApp, converter);
        m_state->setEditingFragment(ef);
    }

    //TODO: Check value extend(doesn't work all the time)

    if ( !m_state->editingFragment() ){
        vlog_debug("editor-documenthandler", "Channel or converter missing for type: \'" + declaration->type() + "\'");

        int propertyValue    = -1;
        int propertyValueEnd = -1;
        m_codeHandler->findDeclarationValue(
            declaration->position(),
            declaration->identifierLength(),
            propertyValue,
            propertyValueEnd
        );
        if ( propertyValue == -1 || propertyValueEnd == -1 ){
            qWarning("Failed to parse document: unable to identify value offset.");
            return;
        }

        declaration->setValuePositionOffset(propertyValue - declaration->position() - declaration->identifierLength());
        declaration->setValueLength(propertyValueEnd - propertyValue);
        m_state->setEditingFragment(new DocumentEditFragment(declaration));
    }

    if ( m_state->editingFragment() ){
        addEditingState(DocumentHandler::Silent);
        rehighlightSection(m_state->editingFragment()->valuePosition(), m_state->editingFragment()->valueLength());
        emit cursorPositionRequest(m_state->editingFragment()->valuePosition());
    }
}

void DocumentHandler::adjust(int position, QObject *currentApp){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    cancelEdit();

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);
    if ( properties.isEmpty() )
        return;

    CodeDeclaration::Ptr declaration = properties.first();

    CodeConverter* converter = m_paletteContainer->findPalette(declaration->type());
    LivePalette* palette = qobject_cast<LivePalette*>(converter);
    if ( palette ){

        DocumentEditFragment* ef = m_codeHandler->createInjectionChannel(declaration, currentApp, converter);
        if ( !ef )
            return;

        ef->setActionType(DocumentEditFragment::Adjust);
        QTextCursor codeCursor(m_targetDoc);
        codeCursor.setPosition(ef->valuePosition());
        codeCursor.setPosition(ef->valuePosition() + ef->valueLength(), QTextCursor::KeepAnchor);

        palette->init(palette->serialize()->fromCode(codeCursor.selectedText(), ef));
        connect(palette, SIGNAL(valueChanged()), this, SLOT(paletteValueChanged()));

        CodeRuntimeBinding* bind = m_projectDocument->bindingAt(ef->declaration()->position());
        if ( bind ){
            vlog_debug("editor-documenthandler", "Linking binding to editing fragment: " + QString::number(ef->declaration()->position()));
            ef->setRuntimeBinding(bind);
        }

        m_state->setEditingFragment(ef);
        emit paletteChanged();

        emit cursorPositionRequest(m_state->editingFragment()->valuePosition());
        addEditingState(DocumentHandler::Silent);
        rehighlightSection(ef->valuePosition(), ef->valueLength());
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

DocumentCursorInfo *DocumentHandler::cursorInfo(int position, int length){
    bool canBind = false, canUnbind = false, canEdit = false, canAdjust = false;

    if ( !m_projectDocument || !m_codeHandler )
        return new DocumentCursorInfo(canBind, canUnbind, canEdit, canAdjust);

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);
    if ( properties.isEmpty() )
        return new DocumentCursorInfo(canBind, canUnbind, canEdit, canAdjust);

    if ( properties.size() == 1 ){
        CodeDeclaration::Ptr firstdecl = properties.first();
        canEdit = true;

        CodeConverter* converter = m_paletteContainer->findPalette(firstdecl->type());
        LivePalette* palette = qobject_cast<LivePalette*>(converter);
        canAdjust = (palette != 0);
    }

    for ( QList<CodeDeclaration::Ptr>::iterator it = properties.begin(); it != properties.end(); ++it ){
        if ( !m_projectDocument->bindingAt((*it)->position()) )
            canBind = true;
        if ( m_projectDocument->bindingAt((*it)->position()) )
            canUnbind = true;

        if ( canBind && canUnbind )
            break;
    }

    return new DocumentCursorInfo(canBind, canUnbind, canEdit, canAdjust);

}

}// namespace

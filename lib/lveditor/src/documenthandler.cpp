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
#include "live/livepalette.h"
#include "live/livepalettecontainer.h"
#include "live/documenteditfragment.h"
#include "live/documenthandlerstate.h"
#include "live/codedeclaration.h"
#include "live/coderuntimebinding.h"
#include "live/editorglobalobject.h"
#include "live/project.h"
#include "live/viewengine.h"
#include "live/visuallog.h"
#include "live/projectextension.h"
#include <QQmlContext>
#include <QQmlEngine>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextList>
#include <QTimer>

#include "textedit_p.h"
#include "textedit_p_p.h"


namespace lv{

//TODO: Add object type on code properties
//TODO: Add object type when looking for palettes
//TODO: Store editor and palette state, in order to stack palettes properly
//   * Palettes need to be stored by name and offset in document. The offset may change, so
//     it must be acquired from the palette itself in order to receive it's updated value

//TODO: Code completion model should only be visible when in focus

const QChar DocumentHandler::ParagraphSeparator = QChar(8233);
const QChar DocumentHandler::NewLine            = QChar('\n');

DocumentHandler::DocumentHandler(QObject *parent)
    : QObject(parent)
    , m_targetDoc(nullptr)
    , m_completionModel(new CodeCompletionModel)
    , m_codeHandler(nullptr)
    , m_projectDocument(nullptr)
    , m_indentSize(0)
    , m_paletteContainer(nullptr)
    , m_project(nullptr)
    , m_engine(nullptr)
    , m_textEdit(nullptr)
    , m_fragmentStart(ProjectDocumentMarker::create())
    , m_fragmentEnd(ProjectDocumentMarker::create())
    , m_fragmentStartLine(-1)
    , m_fragmentEndLine(-1)
    , m_state(new DocumentHandlerState)
    , m_editingFragment(nullptr)
{
    setIndentSize(4);
    m_timer.setInterval(1000);
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateScope()));
}

DocumentHandler::~DocumentHandler(){
    if ( m_projectDocument )
        m_projectDocument->assignDocumentHandler(nullptr);
    delete m_codeHandler;
}

// this shouldn't exist, should be handled in setDocument
// management within the TextEdit
// null pointer!!!
// 1. setDocument method, check for null pointer
// 2. setTextDocument for testing
// 3. try with setting back to null
// 4. we start moving to livecv
//      1) everything works as before?
//      2) switch to livecv passing the document
//

void DocumentHandler::setTextEdit(TextEdit *te)
{
    m_textEdit = te;
    if (m_targetDoc) {
        te->setTextDocument(m_targetDoc);
    }
}

void DocumentHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_codeHandler )
        m_codeHandler->rehighlightBlock(block);
}

void DocumentHandler::componentComplete(){
    /*if ( m_target == 0 ){
        qWarning("Target has not been set for document handler. Highlighting and code completion will be disabled.");
    }*/

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

    m_engine = static_cast<ViewEngine*>(lg->property("engine").value<lv::ViewEngine*>());

    findCodeHandler();
}

bool DocumentHandler::addEditingPalette(DocumentEditFragment *palette){
    auto it = m_palettes.begin();
    while ( it != m_palettes.end() ){
        DocumentEditFragment* itPalette = *it;

        if ( itPalette->declaration()->position() < palette->declaration()->position() ){
            break;

        } else if ( itPalette->declaration()->position() == palette->declaration()->position() ){
            LivePalette* itlp = static_cast<LivePalette*>(itPalette->converter());
            LivePalette* lp   = static_cast<LivePalette*>(palette->converter());
            if ( itlp->path() == lp->path() ){
                return false;
            } else {
                break;
            }
        }

        ++it;
    }

    m_palettes.insert(it, palette);

    return true;
}

void DocumentHandler::removeEditingPalette(DocumentEditFragment *palette){
    auto it = m_palettes.begin();

    while( it != m_palettes.end() ){
        DocumentEditFragment* itPalette = *it;

        if ( itPalette == palette ){
            LivePalette* pl = static_cast<LivePalette*>(palette->converter());
            emit paletteAboutToRemove(pl);

            m_projectDocument->removeSection(palette->declaration()->m_section);
            m_palettes.erase(it);
            delete palette;
            return;
        }

        ++it;
    }
}

void DocumentHandler::commitEdit(){
    if ( m_editingFragment ){
        int position = m_editingFragment->valuePosition();
        int length   = m_editingFragment->valueLength();

        CodeConverter* converter = m_editingFragment->converter();
        if ( converter ){
            QTextCursor tc(m_targetDoc);
            tc.setPosition(position);
            tc.setPosition(position + length, QTextCursor::KeepAnchor);
            QString commitText = tc.selectedText();
            m_editingFragment->commit(converter->serialize()->fromCode(commitText, m_editingFragment));
            vlog_debug("editor-documenthandler", "Commited edit of size: " + QString::number(commitText.size()));

        } else {
            emit contentsChangedManually();
            m_timer.start();
        }

        m_projectDocument->removeSection(m_editingFragment->declaration()->m_section);
        delete m_editingFragment;
        m_editingFragment = nullptr;

        emit editingStateChanged(false);
    }
}

void DocumentHandler::cancelEdit(){
    if ( m_editingFragment ){
        m_projectDocument->removeSection(m_editingFragment->declaration()->m_section);
        delete m_editingFragment;
        m_editingFragment = nullptr;

        emit editingStateChanged(false);
    }
}

void DocumentHandler::paletteValueChanged(lv::DocumentEditFragment *frg){
    LivePalette* palette = static_cast<lv::LivePalette*>(frg->converter());
    QString code = palette->serialize()->toCode(palette->value(), frg);

    if ( code != "" ){
        m_projectDocument->addEditingState(ProjectDocument::Palette);

        QTextCursor tc(m_targetDoc);
        tc.setPosition(frg->valuePosition());
        tc.setPosition(frg->valuePosition() + frg->valueLength(), QTextCursor::KeepAnchor);
        tc.beginEditBlock();
        tc.removeSelectedText();
        tc.insertText(code);
        tc.endEditBlock();

        m_projectDocument->removeEditingState(ProjectDocument::Palette);
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
                m_codeHandler->setTarget(m_targetDoc);
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

void DocumentHandler::documentContentsChanged(int position, int, int charsAdded){
    if ( !m_projectDocument || m_projectDocument->editingStateIs(ProjectDocument::Read) )
        return;

    m_lastChar = QChar();
    if ( !m_projectDocument->editingStateIs(ProjectDocument::Silent) && m_editorFocus ){
        if ( charsAdded == 1 )
            m_lastChar = m_targetDoc->characterAt(position);

        if ( m_editingFragment ){
            if ( position < m_editingFragment->valuePosition() ||
                 position > m_editingFragment->valuePosition() + m_editingFragment->valueLength() )
            {
                cancelEdit();
            } else {
                return;
            }
        }
        emit contentsChangedManually();
        m_timer.start();
    }
}

void DocumentHandler::cursorWritePositionChanged(QTextCursor cursor){
    if ( m_codeHandler && m_editorFocus &&
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
    cancelEdit();
    if ( m_projectDocument ){
        m_projectDocument->assignDocumentHandler(nullptr);
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


        m_projectDocument->assignDocumentHandler(this);
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

void DocumentHandler::updateScope(){
    if ( m_codeHandler && m_projectDocument )
        m_codeHandler->updateScope(m_projectDocument->content());
}


void DocumentHandler::bind(int position, int length, QObject *object){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    QTextCursor cursor(m_targetDoc);
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
            CodeRuntimeBinding* addedBinding = new CodeRuntimeBinding(cde);
            if (m_projectDocument->addNewBinding(addedBinding)){
                addedBinding->setConverter(cvt);
                addedBindings.append(addedBinding);

            } else {
                delete addedBinding;
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

    QTextCursor cursor(m_targetDoc);
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);
    for ( QList<CodeDeclaration::Ptr>::iterator it = properties.begin(); it != properties.end(); ++it ){
        if ( m_projectDocument->removeBindingAt((*it)->position()) )
            m_codeHandler->rehighlightBlock(m_targetDoc->findBlock((*it)->position()));
    }
}

bool DocumentHandler::edit(int position, QObject *currentApp){
    if ( !m_projectDocument || !m_codeHandler )
        return false;

    cancelEdit();

    QTextCursor cursor(m_targetDoc);
    cursor.setPosition(position);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);
    if ( properties.isEmpty() )
        return false;

    CodeDeclaration::Ptr declaration = properties.first();

    CodeConverter* converter = m_paletteContainer->findPalette(
        declaration->type(), declaration->parentType().isEmpty() ? "" : declaration->parentType().first()
    );

    if ( converter ){
        vlog_debug("editor-documenthandler", "Found Converter for type: \'" + declaration->type()+ "\'");
        DocumentEditFragment* ef = m_codeHandler->createInjectionChannel(declaration, currentApp, converter);
        m_editingFragment = ef;

        ef->declaration()->m_section = m_projectDocument->createSection(
            DocumentEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
        );
        ef->declaration()->m_section->setUserData(ef);
        ef->declaration()->m_section->onTextChanged([](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText){
            DocumentEditFragment* sectionEf = reinterpret_cast<DocumentEditFragment*>(section->userData());
            if ( !section->document()->editingStateIs(ProjectDocument::Silent) ){
                int length = sectionEf->declaration()->valueLength();
                sectionEf->declaration()->setValueLength(length - charsRemoved + addedText.size());
            }
        });

        rehighlightSection(ef->valuePosition(), ef->valueLength());


        emit cursorPositionRequest(ef->valuePosition());
        emit editingStateChanged(true);

        return true;
    } else {

        vlog_debug("editor-documenthandler", "Channel or converter missing for type: \'" + declaration->type() + "\'");
        return false;
    }
}

LivePaletteList* DocumentHandler::findPalettes(int position){
    if ( !m_projectDocument || !m_codeHandler )
        return nullptr;

    cancelEdit();

    QTextCursor cursor(m_targetDoc);
    cursor.setPosition(position);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);
    if ( properties.isEmpty() )
        return nullptr;

    CodeDeclaration::Ptr declaration = properties.first();

    return m_paletteContainer->findPalettes(declaration->type());
}

void DocumentHandler::openPalette(lv::LivePalette* palette, int position, QObject *currentApp){

    if ( !m_projectDocument || !m_codeHandler )
        return;

    QTextCursor cursor(m_targetDoc);
    cursor.setPosition(position);

    QList<CodeDeclaration::Ptr> properties = m_codeHandler->getDeclarations(cursor);
    if ( properties.isEmpty() )
        return;

    CodeDeclaration::Ptr declaration = properties.first();

    DocumentEditFragment* ef = m_codeHandler->createInjectionChannel(declaration, currentApp, palette);
    if ( !ef )
        return;

    ef->setActionType(DocumentEditFragment::Adjust);

    QTextCursor codeCursor(m_targetDoc);
    codeCursor.setPosition(ef->valuePosition());
    codeCursor.setPosition(ef->valuePosition() + ef->valueLength(), QTextCursor::KeepAnchor);

    ef->declaration()->m_section = m_projectDocument->createSection(
        DocumentEditFragment::Section, ef->declaration()->position(), ef->declaration()->length()
    );
    ef->declaration()->m_section->setUserData(ef);
    ef->declaration()->m_section->onTextChanged([this](ProjectDocumentSection::Ptr section, int, int charsRemoved, const QString& addedText){
        auto projectDocument = section->document();
        auto editingFragment = reinterpret_cast<DocumentEditFragment*>(section->userData());

        if ( projectDocument->editingStateIs(ProjectDocument::Runtime) ){

            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

            LivePalette* lp = static_cast<LivePalette*>(editingFragment->converter());

            QTextCursor tc(projectDocument->textDocument());
            tc.setPosition(editingFragment->valuePosition());
            tc.setPosition(editingFragment->valuePosition() + editingFragment->valueLength(), QTextCursor::KeepAnchor);

            lp->setValueFromCode(lp->serialize()->fromCode(tc.selectedText(), editingFragment));

        } else if ( !projectDocument->editingStateIs(ProjectDocument::Silent) ){
            removeEditingPalette(editingFragment);
        } else {
            int length = editingFragment->declaration()->valueLength();
            editingFragment->declaration()->setValueLength(length - charsRemoved + addedText.size());

            if ( projectDocument->editingStateIs(ProjectDocument::Palette) ){
                LivePalette* lp = static_cast<LivePalette*>(editingFragment->converter());
                if ( lp ){
                    editingFragment->commit(lp->value());
                }
            }
        }
    });

    palette->init(palette->serialize()->fromCode(codeCursor.selectedText(), ef));
    connect(palette, &LivePalette::valueChanged, [this, ef](){
        paletteValueChanged(ef);
    });

    CodeRuntimeBinding* bind = m_projectDocument->bindingAt(ef->declaration()->position());
    if ( bind ){
        vlog_debug("editor-documenthandler", "Linking binding to editing fragment: " + QString::number(ef->declaration()->position()));
        ef->setRuntimeBinding(bind);
    }

    addEditingPalette(ef);

    emit cursorPositionRequest(ef->valuePosition());
    rehighlightSection(ef->valuePosition(), ef->valueLength());
}

void DocumentHandler::removePalette(QObject *paletteContainer){
    auto it = m_palettes.begin();

    while( it != m_palettes.end() ){

        DocumentEditFragment* itPalette = *it;
        LivePalette* lp = static_cast<LivePalette*>(itPalette->converter());

        if ( lp->item() == paletteContainer ){
            removeEditingPalette(itPalette);
            return;
        }

        ++it;
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

    QTextCursor cursor(m_targetDoc);
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
        canAdjust = (palette != nullptr);
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

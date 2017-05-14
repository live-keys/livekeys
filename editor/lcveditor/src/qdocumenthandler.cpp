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

#include "qdocumenthandler.h"
#include "qprojectdocument.h"
#include "qprojectfile.h"
#include "qlivepalette.h"
#include "qlivepalettecontainer.h"
#include "qdocumenteditfragment.h"
#include "qdocumentcodestate.h"
#include <QQuickTextDocument>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextCursor>
#include <QTextBlock>

#include <QTextList>

#define QCODE_BASIC_HANDLER_DEBUG_FLAG
#ifdef QCODE_BASIC_HANDLER_DEBUG_FLAG
#define QCODE_BASIC_HANDLER_DEBUG(_param) qDebug() << "CODE HANDLER:" << (_param)
#else
#define QCODE_BASIC_HANDLER_DEBUG(_param)
#endif

namespace lcv{

//TODO: Add object type on code properties
//TODO: Add object type when looking for palettes
//TODO: Connect added bindings to palettes so it can be disabled when needed

QDocumentHandler::QDocumentHandler(QAbstractCodeHandler *handler,
        QLivePaletteContainer *paletteContainer,
        QObject *parent)
    : QObject(parent)
    , m_target(0)
    , m_targetDoc(0)
    , m_completionModel(new QCodeCompletionModel)
    , m_codeHandler(handler)
    , m_projectDocument(0)
    , m_paletteContainer(paletteContainer)
    , m_editingState(0)
    , m_state(new QDocumentCodeState)
{
}

QDocumentHandler::~QDocumentHandler(){
    if ( m_projectDocument )
        m_projectDocument->assignEditingDocument(0, 0);
}

void QDocumentHandler::setTarget(QQuickTextDocument *target){
    if ( m_target != target ){
        m_target = target;
        if ( m_codeHandler ){
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
                }
            }
            m_codeHandler->setTarget(m_targetDoc, m_state);
            if ( m_projectDocument ){
                addEditingState(QDocumentHandler::Silent);
                m_targetDoc->setPlainText(m_projectDocument->content());
                m_projectDocument->assignEditingDocument(m_targetDoc, this);
                removeEditingState(QDocumentHandler::Silent);
                emit cursorPositionRequest(0);
            }
        }
        emit targetChanged();
    }
}

QLivePalette *QDocumentHandler::palette(){
    if ( !m_state->editingFragment() )
        return 0;
    return qobject_cast<QLivePalette*>(m_state->editingFragment()->converter());
}

void QDocumentHandler::rehighlightBlock(const QTextBlock &block){
    if ( m_codeHandler )
        m_codeHandler->rehighlightBlock(block);
}

void QDocumentHandler::commitEdit(){
    if ( m_state->editingFragment() ){
        int position = m_state->editingFragment()->position();
        int length   = m_state->editingFragment()->length();

        QCodeConverter* converter = m_state->editingFragment()->converter();
        if ( converter ){
            QTextCursor tc(m_targetDoc);
            tc.setPosition(position);
            tc.setPosition(position + length, QTextCursor::KeepAnchor);
            QString commitText = tc.selectedText();
            m_state->editingFragment()->commit(
                converter->serialize()->fromCode(commitText)
            );
            QCODE_BASIC_HANDLER_DEBUG("Commited edit of size: " + QString::number(commitText.size()));
        } else {
            emit contentsChangedManually();
        }

        disconnect(palette(), SIGNAL(valueChanged()), this, SLOT(paletteValueChanged()));
        m_state->clearEditingFragment();
        rehighlightSection(position, length);
        emit paletteChanged();
        resetEditingState();
    }
}

void QDocumentHandler::cancelEdit(){
    if ( m_state->editingFragment() ){
        int position = m_state->editingFragment()->position();
        int length   = m_state->editingFragment()->length();
        disconnect(palette(), SIGNAL(valueChanged()), this, SLOT(paletteValueChanged()));
        m_state->clearEditingFragment();
        emit paletteChanged();
        rehighlightSection(position, length);
        resetEditingState();
    }
}

bool QDocumentHandler::isEditing(){
    return (m_state->editingFragment() != 0);
}

void QDocumentHandler::paletteValueChanged(){
    QString code = palette()->serialize()->toCode(palette()->value());

    addEditingState(QDocumentHandler::Palette);

    QDocumentEditFragment* frg = m_state->editingFragment();
    QTextCursor tc(m_targetDoc);
    tc.setPosition(frg->position());
    tc.setPosition(frg->position() + frg->length(), QTextCursor::KeepAnchor);
    tc.beginEditBlock();
    tc.removeSelectedText();
    tc.insertText(code);
    tc.endEditBlock();

    removeEditingState(QDocumentHandler::Palette);
}

void QDocumentHandler::rehighlightSection(int position, int length){
    QTextBlock bl = m_targetDoc->findBlock(position);
    int end = position + length;
    while ( bl.isValid() ){
        m_codeHandler->rehighlightBlock(bl);
        if (bl.position() > end )
            break;
        bl = bl.next();
    }
}

void QDocumentHandler::insertCompletion(int from, int to, const QString &completion){
    if ( m_targetDoc ){
        addEditingState(QDocumentHandler::Assisted);
        QTextCursor cursor(m_targetDoc);
        cursor.beginEditBlock();
        cursor.setPosition(from);
        cursor.setPosition(to, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.insertText(completion);
        cursor.endEditBlock();
        m_completionModel->disable();
        removeEditingState(QDocumentHandler::Assisted);
    }
}

void QDocumentHandler::documentContentsChanged(int position, int charsRemoved, int charsAdded){
    if ( !m_targetDoc )
        return;

    m_lastChar = QChar();

    QString addedText = "";
    if ( charsAdded == 1 ){
        addedText = m_targetDoc->characterAt(position);
    } else if ( charsAdded > 0 ){
        QTextCursor cursor(m_targetDoc);
        cursor.setPosition(position);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
        addedText = cursor.selection().toPlainText();
    }

    if ( !editingStateIs(QDocumentHandler::Silent) ){
        if ( charsAdded == 1 )
            m_lastChar = m_targetDoc->characterAt(position);

        if ( m_projectDocument )
            m_projectDocument->documentContentsChanged(position, charsRemoved, addedText);

        emit contentsChangedManually();

    } else if ( m_projectDocument ){
        QDocumentEditFragment* frg = m_state->editingFragment();
        if ( frg ){
            if ( position >= frg->position() && position <= frg->position() + frg->length() ){
                frg->updateLength(frg->length() - charsRemoved + charsAdded);
                if ( frg->actionType() == QDocumentEditFragment::Adjust ){
                    if ( editingStateIs(QDocumentHandler::Palette) ){
                        m_state->editingFragment()->commit(palette()->value());
                        QProjectDocumentBinding* binding = frg->runtimeBinding();
                        if ( binding ){
                            frg->setRuntimeBinding(0);
                            m_projectDocument->removeBindingAt(binding->propertyPosition);
                        }
                    } else {
                        // TODO: Mb add timer
                        QTextCursor tc(m_targetDoc);
                        tc.setPosition(frg->position());
                        tc.setPosition(frg->position() + frg->length(), QTextCursor::KeepAnchor);
                        palette()->setValueFromCode(palette()->serialize()->fromCode(tc.selectedText()));

                        //TODO: Treat code coming from binding
                    }
                }



            } else {
                cancelEdit();
            }
        }
        m_projectDocument->documentContentsSilentChanged(position, charsRemoved, addedText);
    }
}

void QDocumentHandler::cursorWritePositionChanged(QTextCursor cursor){
    if ( m_codeHandler &&
         !editingStateIs(QDocumentHandler::Assisted) &&
         !editingStateIs(QDocumentHandler::Silent)
        )
    {
        addEditingState(QDocumentHandler::Assisted);
        QTextCursor newCursor;
        m_codeHandler->assistCompletion(
            cursor,
            m_lastChar,
            false,
            m_completionModel,
            newCursor
        );
        removeEditingState(QDocumentHandler::Assisted);
        if ( !newCursor.isNull() ){
            emit cursorPositionRequest(newCursor.position());
        }
    }
}

void QDocumentHandler::setDocument(QProjectDocument *document){
    if ( m_projectDocument )
        m_projectDocument->assignEditingDocument(0, 0);

    if ( document && m_targetDoc ){
        addEditingState(QDocumentHandler::Silent);
        document->assignEditingDocument(m_targetDoc, this);
        m_targetDoc->setPlainText(document->content());
        removeEditingState(QDocumentHandler::Silent);
    }
    m_projectDocument = document;
    m_codeHandler->setDocument(document);
}

void QDocumentHandler::generateCompletion(int cursorPosition){
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

void QDocumentHandler::updateScope(const QString &data){
    if ( m_codeHandler )
        m_codeHandler->updateScope(data);
}

bool QDocumentHandler::canBind(int position, int length){
    if ( !m_projectDocument || !m_codeHandler )
        return false;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);

    int unboundProperties = 0;
    for ( QList<QAbstractCodeHandler::CodeProperty>::iterator it = properties.begin(); it != properties.end(); ++it ){
        if ( !m_projectDocument->bindingAt(it->position) )
            ++unboundProperties;
    }

    return unboundProperties > 0;
}

void QDocumentHandler::bind(int position, int length, QObject *object){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);

    QList<QProjectDocumentBinding*> addedBindings;
    for ( QList<QAbstractCodeHandler::CodeProperty>::iterator it = properties.begin(); it != properties.end(); ++it ){
        //check if converter available for binding

        QCodeConverter* cvt = m_paletteContainer->findPalette(it->type, it->parentType);
        if ( cvt ){
            QProjectDocumentBinding* addedBinding = m_projectDocument->addNewBinding(it->position, it->length, it->name);
            if (addedBinding){
                addedBinding->setConverter(cvt);
                addedBindings.append(addedBinding);
                if ( m_state->editingFragment() ){
                    m_state->editingFragment()->setRuntimeBinding(addedBinding);
                }
            }
        }
    }

    // Bind properties to engine and rehighlight
    if ( m_targetDoc ){
        if ( object )
            m_codeHandler->connectBindings(addedBindings, object);

        for ( QList<QProjectDocumentBinding*>::iterator it = addedBindings.begin(); it != addedBindings.end(); ++it ){
            m_codeHandler->rehighlightBlock(m_targetDoc->findBlock((*it)->propertyPosition));
        }
    }

}

bool QDocumentHandler::canUnbind(int position, int length){
    if ( !m_projectDocument || !m_codeHandler )
        return false;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);
    for ( QList<QAbstractCodeHandler::CodeProperty>::iterator it = properties.begin(); it != properties.end(); ++it ){
        if ( m_projectDocument->bindingAt(it->position) )
            return true;
    }
    return false;
}

void QDocumentHandler::unbind(int position, int length){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);
    for ( QList<QAbstractCodeHandler::CodeProperty>::iterator it = properties.begin(); it != properties.end(); ++it ){
        if ( m_projectDocument->removeBindingAt(it->position) )
            m_codeHandler->rehighlightBlock(m_targetDoc->findBlock(it->position));
    }
}

bool QDocumentHandler::canEdit(int position){
    if ( !m_projectDocument || !m_codeHandler )
        return false;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);
    if ( properties.isEmpty() )
        return false;

    return true;
}

void QDocumentHandler::edit(int position, QObject *currentApp){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    cancelEdit();

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);
    if ( properties.isEmpty() )
        return;

    QCodeConverter* converter = m_paletteContainer->findPalette(properties.first().type);
    if ( converter ){
        QCODE_BASIC_HANDLER_DEBUG("Found Converter for type: \'" + properties.first().type + "\'");
        QDocumentEditFragment* ef = m_codeHandler->createInjectionChannel(properties.first(), currentApp, converter);
        m_state->setEditingFragment(ef);
    }

    //TODO: Check value extend(doesn't work all the time)

    if ( !m_state->editingFragment() ){
        QCODE_BASIC_HANDLER_DEBUG("Channel or converter missing for type: \'" + properties.first().type + "\'");

        int propertyValue    = -1;
        int propertyValueEnd = -1;
        m_codeHandler->findPropertyValue(
            properties.first().position,
            properties.first().length,
            propertyValue,
            propertyValueEnd
        );
        if ( propertyValue == -1 || propertyValueEnd == -1 ){
            qWarning("Failed to parse document: unable to identify value offset.");
            return;
        }

        m_state->setEditingFragment(new QDocumentEditFragment(propertyValue, propertyValueEnd - propertyValue));
    }

    if ( m_state->editingFragment() ){
        emit cursorPositionRequest(m_state->editingFragment()->position());
        addEditingState(QDocumentHandler::Silent);
        rehighlightSection(properties.first().position, properties.first().length);
    }

}

bool QDocumentHandler::canAdjust(int position){
    if ( !m_projectDocument || !m_codeHandler )
        return false;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);
    if ( properties.isEmpty() )
        return false;

    QCodeConverter* converter = m_paletteContainer->findPalette(properties.first().type);
    QLivePalette* palette = qobject_cast<QLivePalette*>(converter);
    return (palette != 0);
}

void QDocumentHandler::adjust(int position, QObject *currentApp){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    cancelEdit();

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);
    if ( properties.isEmpty() )
        return;

    QCodeConverter* converter = m_paletteContainer->findPalette(properties.first().type);
    QLivePalette* palette = qobject_cast<QLivePalette*>(converter);
    if ( palette ){
        QDocumentEditFragment* ef = m_codeHandler->createInjectionChannel(properties.first(), currentApp, converter);
        ef->setActionType(QDocumentEditFragment::Adjust);
        QTextCursor codeCursor(m_targetDoc);
        codeCursor.setPosition(ef->position());
        codeCursor.setPosition(ef->position() + ef->length(), QTextCursor::KeepAnchor);

        palette->init(palette->serialize()->fromCode(codeCursor.selectedText()));
        connect(palette, SIGNAL(valueChanged()), this, SLOT(paletteValueChanged()));

        m_state->setEditingFragment(ef);
        emit paletteChanged();

        emit cursorPositionRequest(m_state->editingFragment()->position());
        addEditingState(QDocumentHandler::Silent);
        rehighlightSection(properties.first().position, properties.first().length);
    }

}

}// namespace

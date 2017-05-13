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

#include "qdocumentcodeinterface.h"
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

#define QDOCUMENT_CODE_INTERFACE_DEBUG_FLAG
#ifdef QDOCUMENT_CODE_INTERFACE_DEBUG_FLAG
#define QDOCUMENT_CODE_INTERFACE_DEBUG(_param) qDebug() << "DOCUMENT HANDLER:" << (_param)
#else
#define QDOCUMENT_CODE_INTERFACE_DEBUG(_param)
#endif

namespace lcv{

QDocumentCodeInterface::QDocumentCodeInterface(QAbstractCodeHandler *handler,
        QLivePaletteContainer *paletteContainer,
        QObject *parent)
    : QObject(parent)
    , m_target(0)
    , m_targetDoc(0)
    , m_completionModel(new QCodeCompletionModel)
    , m_codeHandler(handler)
    , m_projectDocument(0)
    , m_paletteContainer(paletteContainer)
    , m_autoInserting(false)
    , m_silentEditing(false)
    , m_paletteEditing(false)
    , m_state(new QDocumentCodeState)
{
}

QDocumentCodeInterface::~QDocumentCodeInterface(){
    if ( m_projectDocument )
        m_projectDocument->assignEditingDocument(0, 0);
}

void QDocumentCodeInterface::setTarget(QQuickTextDocument *target){
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
                enableSilentEditing();
                m_targetDoc->setPlainText(m_projectDocument->content());
                m_projectDocument->assignEditingDocument(m_targetDoc, this);
                disableSilentEditing();
                emit cursorPositionRequest(0);
            }
        }
        emit targetChanged();
    }
}

QLivePalette *QDocumentCodeInterface::palette(){
    if ( !m_state->editingFragment() )
        return 0;
    return qobject_cast<QLivePalette*>(m_state->editingFragment()->converter());
}

void QDocumentCodeInterface::rehighlightBlock(const QTextBlock &block){
    if ( m_codeHandler )
        m_codeHandler->rehighlightBlock(block);
}

void QDocumentCodeInterface::commitEdit(){
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
            QDOCUMENT_CODE_INTERFACE_DEBUG("Commited edit of size: " + QString::number(commitText.size()));
        } else {
            emit contentsChangedManually();
        }

        disconnect(palette(), SIGNAL(valueChanged()), this, SLOT(paletteValueChanged()));
        m_state->clearEditingFragment();
        rehighlightSection(position, length);
        emit paletteChanged();
        disableSilentEditing();
    }
}

void QDocumentCodeInterface::cancelEdit(){
    if ( m_state->editingFragment() ){
        int position = m_state->editingFragment()->position();
        int length   = m_state->editingFragment()->length();
        disconnect(palette(), SIGNAL(valueChanged()), this, SLOT(paletteValueChanged()));
        m_state->clearEditingFragment();
        emit paletteChanged();
        rehighlightSection(position, length);
        disableSilentEditing();
    }
}

bool QDocumentCodeInterface::isEditing(){
    return (m_state->editingFragment() != 0);
}

void QDocumentCodeInterface::paletteValueChanged(){
    QString code = palette()->serialize()->toCode(palette()->value());

    m_paletteEditing = true;
    QDocumentEditFragment* frg = m_state->editingFragment();
    QTextCursor tc(m_targetDoc);
    tc.setPosition(frg->position());
    tc.setPosition(frg->position() + frg->length(), QTextCursor::KeepAnchor);
    tc.beginEditBlock();
    tc.removeSelectedText();
    tc.insertText(code);
    tc.endEditBlock();
    m_paletteEditing = false;
}

void QDocumentCodeInterface::rehighlightSection(int position, int length){
    QTextBlock bl = m_targetDoc->findBlock(position);
    int end = position + length;
    while ( bl.isValid() ){
        m_codeHandler->rehighlightBlock(bl);
        if (bl.position() > end )
            break;
        bl = bl.next();
    }
}

void QDocumentCodeInterface::insertCompletion(int from, int to, const QString &completion){
    if ( m_targetDoc ){
        m_autoInserting = true;
        QTextCursor cursor(m_targetDoc);
        cursor.beginEditBlock();
        cursor.setPosition(from);
        cursor.setPosition(to, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.insertText(completion);
        cursor.endEditBlock();
        m_completionModel->disable();
        m_autoInserting = false;
    }
}

void QDocumentCodeInterface::documentContentsChanged(int position, int charsRemoved, int charsAdded){
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

    if ( !m_silentEditing ){
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
                    if ( m_paletteEditing ){
                        m_state->editingFragment()->commit(palette()->value());
                    } else {
                        // TODO: Mb add timer
                        QTextCursor tc(m_targetDoc);
                        tc.setPosition(frg->position());
                        tc.setPosition(frg->position() + frg->length(), QTextCursor::KeepAnchor);
                        palette()->setValueFromCode(palette()->serialize()->fromCode(tc.selectedText()));
                    }
                }
            } else {
                cancelEdit();
            }
        }
        m_projectDocument->documentContentsSilentChanged(position, charsRemoved, addedText);
    }
}

void QDocumentCodeInterface::cursorWritePositionChanged(QTextCursor cursor){
    if ( m_codeHandler && !m_autoInserting && !m_silentEditing ){
        m_autoInserting = true;
        QTextCursor newCursor;
        m_codeHandler->assistCompletion(
            cursor,
            m_lastChar,
            false,
            m_completionModel,
            newCursor
        );
        m_autoInserting = false;
        if ( !newCursor.isNull() ){
            emit cursorPositionRequest(newCursor.position());
        }
    }
}

void QDocumentCodeInterface::setDocument(QProjectDocument *document){
    if ( m_projectDocument )
        m_projectDocument->assignEditingDocument(0, 0);

    if ( document && m_targetDoc ){
        enableSilentEditing();
        document->assignEditingDocument(m_targetDoc, this);
        m_targetDoc->setPlainText(document->content());
        disableSilentEditing();
    }
    m_projectDocument = document;
    m_codeHandler->setDocument(document);
}

void QDocumentCodeInterface::generateCompletion(int cursorPosition){
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

void QDocumentCodeInterface::updateScope(const QString &data){
    if ( m_codeHandler )
        m_codeHandler->updateScope(data);
}

bool QDocumentCodeInterface::canBind(int position, int length){
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

void QDocumentCodeInterface::bind(int position, int length, QObject *object){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);
    if ( length != 0 )
        cursor.setPosition(position + length, QTextCursor::KeepAnchor);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);

    QList<QProjectDocumentBinding*> addedBindings;
    for ( QList<QAbstractCodeHandler::CodeProperty>::iterator it = properties.begin(); it != properties.end(); ++it ){
        QProjectDocumentBinding* addedBinding = m_projectDocument->addNewBinding(it->position, it->length, it->name);
        if ( addedBinding )
            addedBindings.append(addedBinding);
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

bool QDocumentCodeInterface::canUnbind(int position, int length){
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

void QDocumentCodeInterface::unbind(int position, int length){
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

bool QDocumentCodeInterface::canEdit(int position){
    if ( !m_projectDocument || !m_codeHandler )
        return false;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);
    if ( properties.isEmpty() )
        return false;

    return true;
}

void QDocumentCodeInterface::edit(int position, QObject *currentApp){
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
        QDOCUMENT_CODE_INTERFACE_DEBUG("Found Converter for type: \'" + properties.first().type + "\'");
        QDocumentEditFragment* ef = m_codeHandler->createInjectionChannel(properties.first(), currentApp, converter);
        m_state->setEditingFragment(ef);
    }

    //TODO: Check value extend(doesn't work all the time)

    if ( !m_state->editingFragment() ){
        QDOCUMENT_CODE_INTERFACE_DEBUG("Channel or converter missing for type: \'" + properties.first().type + "\'");

        //TODO: Find value offset + length
//        m_state->setEditingFragment(new QDocumentEditFragment(properties.first().position, properties.first().length));
    }

    if ( m_state->editingFragment() ){
        emit cursorPositionRequest(m_state->editingFragment()->position());
        enableSilentEditing();
        rehighlightSection(properties.first().position, properties.first().length);
    }

}

bool QDocumentCodeInterface::canAdjust(int position){
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

void QDocumentCodeInterface::adjust(int position, QObject *currentApp){
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
        enableSilentEditing();
        rehighlightSection(properties.first().position, properties.first().length);
    }

}

}// namespace

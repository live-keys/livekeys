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
#include <QQuickTextDocument>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextCursor>
#include <QTextBlock>

#include <QTextList>

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
            m_codeHandler->setTarget(m_targetDoc);
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

void QDocumentCodeInterface::rehighlightBlock(const QTextBlock &block){
    if ( m_codeHandler )
        m_codeHandler->rehighlightBlock(block);
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

    //TODO: Lookup converter
    return true;
}

void QDocumentCodeInterface::edit(int position){
    if ( !m_projectDocument || !m_codeHandler )
        return;

    QTextCursor cursor(m_target->textDocument());
    cursor.setPosition(position);

    QList<QAbstractCodeHandler::CodeProperty> properties = m_codeHandler->getProperties(cursor);
    if ( properties.isEmpty() )
        return;

    //TODO: Lookup converter
    qDebug() << "LOOK FOR CONVERTER:" << properties.first().type;
}

}// namespace
